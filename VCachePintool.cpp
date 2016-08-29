/*
 * VCachePintool.cpp
 *
 *
 *  Created on: July 21, 2016
 *      Author: vance
 */

#include <fstream>
#include <iostream>
#include <vector>

#include "pin.H"

// It's too complicated to get the special pin makefile to work,
// so just include these files directly.
#include "MultilevelCache.cpp"
#include "Cache.cpp"
#include "CacheSet.cpp"
#include "CacheLine.cpp"

#define PROGRESS_INDICATOR 1000000
#define PROGRESS_THRESHOLD 1

/* ================================================================== */
/*                           Global variables                         */
/* ================================================================== */

bool printed;
bool fast_forward;
bool execute;

uint32_t thread_count;
uint64_t skipped;
uint64_t executed;
uint64_t skip_max;
uint64_t execute_max;

int _argc;
char** _argv;

MultilevelCache* mlc;
uint32_t line_size_B;
PIN_MUTEX count_lock;
PIN_MUTEX cache_lock;
PIN_MUTEX print_lock;
PIN_MUTEX thread_lock;

ofstream out;

/* From pin/source/tools/SimpleExamples/inscount2_mt.cpp */
// The running count of instructions is kept here
// We let each thread's count be in its own data cache line so that
// multiple threads do not contend for the same data cache line.
// This has shown to improve the performance of inscount2_mt by up to 6X
// on SPECOMP2001.
#define PADSIZE 48  // 64byte linesize : 64 - 8 - 8
struct THREAD_DATA {
  UINT64 executed;
  UINT64 skipped;
  UINT8 _pad[PADSIZE];
};

#define MAX_THREAD_COUNT 1024
THREAD_DATA counts[MAX_THREAD_COUNT];

/* ===================================================================== */
/*                          Command line Switches                        */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "vcache.out",
    "specify output file name");

KNOB<BOOL> KnobTrackLoads(KNOB_MODE_WRITEONCE, "pintool", "l", "0",
    "track loads");
KNOB<BOOL> KnobTrackStores(KNOB_MODE_WRITEONCE, "pintool", "s", "0",
    "track stores");

KNOB<UINT32> KnobLineSize(KNOB_MODE_WRITEONCE, "pintool", "b", "64",
    "cache line size in bytes");
KNOB<UINT32> KnobCacheLevels(KNOB_MODE_WRITEONCE, "pintool", "n", "1",
    "number of cache levels");
KNOB<UINT64> KnobCacheSize(KNOB_MODE_APPEND, "pintool", "c", "",
    "list of cache sizes in bytes");
KNOB<UINT32> KnobAssociativity(KNOB_MODE_APPEND, "pintool", "a", "",
    "list of cache associativities (1 for direct mapped)");

KNOB<UINT64> KnobFastForward(KNOB_MODE_WRITEONCE, "pintool", "f", "0",
    "begin profiling after the specified number of instructions");
KNOB<UINT64> KnobExecuteCount(KNOB_MODE_WRITEONCE, "pintool", "e", "1000000",
    "total number of instructions to execute after fast forwarding");

/* ===================================================================== */
/*                                Utilities                              */
/* ===================================================================== */

/*
 *  Usage message.
 */
INT32 Usage() {
  cerr << "This tool represents a cache simulator.\n";
  cerr << KNOB_BASE::StringKnobSummary();
  cerr << endl;
  return -1;
}

/*
 * Print out analysis results. Call immediately before the program terminates.
 */
void PrintStats() {
  PIN_MutexLock(&print_lock);
  if (printed) return;
  printed = true;
  PIN_MutexUnlock(&print_lock);
  out << "===============================================" << std::endl;
  out << "Program: ";
  for (int i = 0; i < _argc; i++) {
    out << _argv[i] << " ";
  }
  out << std::endl;
  out << "Fast-forward: " << skipped << "/" << skip_max << std::endl;
  out << "Execute: " << executed << "/" << execute_max << std::endl;
  out << "VCache_Pintool analysis results: " << std::endl;
  out << "Hits: " << mlc->hits << std::endl;
  out << "Hit ratio: "
      << (double) mlc->hits / (double) (mlc->hits + mlc->misses) << std::endl;
  out << "Misses: " << mlc->misses << std::endl;
  out << "Miss ratio: "
      << (double) mlc->misses / (double) (mlc->hits + mlc->misses) << std::endl;
  for (uint32_t i = 0; i < line_size_B; i++) {
    out << "Utilization " << unsigned(i + 1) << ": "
        << unsigned(mlc->byte_utilizations.at(i)) << std::endl;
  }
  out << "===============================================" << std::endl;

}


/* ===================================================================== */
/*                       Instrumentation callbacks                       */
/* ===================================================================== */

/*
 * This function is called for every thread created by the application
 * before it begins.
 * @param   threadid        ID assigned by PIN to the new thread
 * @param   ctxt            initial register state for the new thread
 * @param   flags           thread creation flags (OS specific)
 * @param   v               value specified by the tool in the
 *                              PIN_AddThreadStartFunction function call
 */
void ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, void *v) {
  PIN_MutexLock(&thread_lock);
  thread_count++;
  ASSERT(thread_count <= MAX_THREAD_COUNT, "Maximum thread count exceeded\n");
  std::cerr << "Thread: " << threadid << " started." << std::endl;
  PIN_MutexUnlock(&thread_lock);
}
/*
 * This function is called for every thread that exits.
 * @param   threadid        ID assigned by PIN to the new thread
 * @param   ctxt            initial register state for the new thread
 * @param   v               value specified by the tool in the
 *                              PIN_AddThreadFiniFunction function call
 */
void ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, void *v) {
  std::cerr << "Thread " << threadid << " exits with code " << code << "."
      << std::endl;
}

/*!
 * This function is called when the application exits.
 * @param   code            exit code of the application
 * @param   v               value specified by the tool in the
 *                              PIN_AddFiniFunction function call
 */
void Fini(INT32 code, void *v) {
  PrintStats();
  out.close();
  PIN_MutexFini(&cache_lock);
  PIN_MutexFini(&count_lock);
  PIN_MutexFini(&thread_lock);
  PIN_MutexFini(&print_lock);
  exit(code);
}

/* ===================================================================== */
/*                         Analysis routines                             */
/* ===================================================================== */

void PIN_FAST_ANALYSIS_CALL DoCount(THREADID threadid) {
  if (fast_forward) {
    PIN_MutexLock(&count_lock);
    skipped++;
    PIN_MutexUnlock(&count_lock);
    counts[threadid].skipped++;
    if (skipped > skip_max) {
      fast_forward = false;
      execute = true;
    }
    if (skipped % (PROGRESS_INDICATOR * 10) == 0) {
      PIN_MutexLock(&print_lock);
      std::cerr << "Fast Forwarded: " << skipped << "/" << skip_max
          << std::endl;
      PIN_MutexUnlock(&print_lock);
    }
  } else {
    PIN_MutexLock(&count_lock);
    executed++;
    PIN_MutexUnlock(&count_lock);
    counts[threadid].executed++;
    if (executed % PROGRESS_INDICATOR == 0) {
      PIN_MutexLock(&print_lock);
      std::cerr << "executed: " << executed << "/" << execute_max << std::endl;
      PIN_MutexUnlock(&print_lock);
    }
    if (executed > execute_max) {
      execute = false;
    }
  }
}

void Read(THREADID threadid, ADDRESS addr, UINT32 size) {
  if (execute) {
    PIN_MutexLock(&cache_lock);
    std::vector<CacheLine*> lines = mlc->Access(addr, size);
    PIN_MutexUnlock(&cache_lock);
  }
}

void Write(THREADID threadid, ADDRESS addr, UINT32 size) {
  if (execute) {
    PIN_MutexLock(&cache_lock);
    std::vector<CacheLine*> lines = mlc->Access(addr, size);
    PIN_MutexUnlock(&cache_lock);
  }
}

void Instruction(INS ins, void *v) {
  INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) DoCount, IARG_FAST_ANALYSIS_CALL,
      IARG_THREAD_ID, IARG_END);

  if (LEVEL_CORE::INS_IsMemoryRead(ins) && KnobTrackLoads) {
    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Read, IARG_THREAD_ID,
        IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
  }

  if (LEVEL_CORE::INS_IsMemoryWrite(ins) && KnobTrackStores) {
    INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Write,
        IARG_THREAD_ID, IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);
  }
  if (!fast_forward && !execute) {
    PrintStats();
    PIN_ExitApplication(0);
  }
}
bool Validate_Knobs() {
  UINT32 levels = KnobCacheLevels.Value();
  return (levels == KnobCacheSize.NumberOfValues()
      && levels == KnobAssociativity.NumberOfValues());
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param   argc            total number of elements in the argv array
 * @param   argv            array of command line arguments,
 *                              including pin -t <toolname> -- ...
 */
int main(int argc, char *argv[]) {
  if (PIN_Init(argc, argv)) {
    return Usage();
  }
  if (!Validate_Knobs()) {
    std::cerr
        << "ERROR: Cache levels must equal number of cache size and associativity arguments."
        << std::endl;
    return Usage();
  }

  fast_forward = (KnobFastForward.Value() > 0);
  execute = !fast_forward;
  printed = false;
  skipped = 0;
  executed = 0;

  _argc = argc;
  _argv = argv;

  out.open(KnobOutputFile.Value().c_str());
  skip_max = KnobFastForward.Value();
  execute_max = KnobExecuteCount.Value();

  std::vector<uint64_t> capacities_B;
  std::vector<uint16_t> associativities;
  line_size_B = KnobLineSize.Value();

  for (uint8_t i = 0; i < KnobCacheLevels.Value(); i++) {
    capacities_B.push_back(KnobCacheSize.Value(i));
    associativities.push_back(KnobAssociativity.Value(i));
  }

  mlc = new MultilevelCache(capacities_B, associativities, line_size_B);

  INS_AddInstrumentFunction(Instruction, 0);
  PIN_AddFiniFunction(Fini, 0);

  PIN_AddThreadStartFunction(ThreadStart, 0);
  PIN_AddThreadFiniFunction(ThreadFini, 0);
  PIN_MutexInit(&cache_lock);
  PIN_MutexInit(&count_lock);
  PIN_MutexInit(&thread_lock);
  PIN_MutexInit(&print_lock);
// Never returns

  PIN_StartProgram();

  return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
