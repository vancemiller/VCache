/*! @file
 *  This is an example of the PIN tool that demonstrates some basic PIN APIs 
 *  and could serve as the starting point for developing your first PIN tool
 */

#include "pin.H"
#include "MultilevelCache.cpp"
#include "Cache.cpp"
#include "CacheSet.cpp"
#include "CacheLine.cpp"
#include <iostream>
#include <vector>
#include <fstream>

/* ================================================================== */
// Global variables 
/* ================================================================== */

MultilevelCache* mlc;
uint32_t line_size_B;
PIN_MUTEX lock;

/* ===================================================================== */
/* Command line Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "dcache.out",
    "specify dcache file name");
KNOB<BOOL> KnobTrackLoads(KNOB_MODE_WRITEONCE, "pintool", "l", "0",
    "track individual loads -- increases profiling time");
KNOB<BOOL> KnobTrackStores(KNOB_MODE_WRITEONCE, "pintool", "s", "0",
    "track individual stores -- increases profiling time");
KNOB<UINT32> KnobThresholdHit(KNOB_MODE_WRITEONCE, "pintool", "rh", "100",
    "only report memops with hit count above threshold");
KNOB<UINT32> KnobThresholdMiss(KNOB_MODE_WRITEONCE, "pintool", "rm", "100",
    "only report memops with miss count above threshold");
KNOB<BOOL> KnobIgnoreStores(KNOB_MODE_WRITEONCE, "pintool", "ns", "0",
    "ignore all stores");
KNOB<BOOL> KnobIgnoreSize(KNOB_MODE_WRITEONCE, "pintool", "z", "0",
    "ignore size of all references (default size is 4 bytes)");
KNOB<UINT32> KnobLineSize(KNOB_MODE_WRITEONCE, "pintool", "b", "64",
    "cache line size in bytes");

KNOB<UINT32> KnobCacheLevels(KNOB_MODE_WRITEONCE, "pintool", "n", "1",
    "number of cache levels");

KNOB<UINT32> KnobCacheSize(KNOB_MODE_APPEND, "pintool", "c", "",
    "list of cache sizes in bytes");
KNOB<UINT32> KnobAssociativity(KNOB_MODE_APPEND, "pintool", "a", "",
    "list of cache associativities (1 for direct mapped)");

/* ===================================================================== */
// Utilities
/* ===================================================================== */

/*!
 *  Print out help message.
 */
INT32 Usage() {
  cerr << "This tool represents a cache simulator.\n";
  cerr << KNOB_BASE::StringKnobSummary();
  cerr << endl;
  return -1;
}

/* ===================================================================== */
// Analysis routines
/* ===================================================================== */

/* ===================================================================== */
// Instrumentation callbacks
/* ===================================================================== */

void Read(ADDRESS addr, UINT32 size) {
  PIN_MutexLock(&lock);
  std::vector<CacheLine*>* lines = mlc->Access(addr, size);

  uint32_t bytes_to_read = size;
  for (std::vector<CacheLine*>::iterator it = lines->begin(); it != lines->end();
      it++) {
    ADDRESS access_begin = addr > (*it)->address ? addr : (*it)->address;
    uint32_t byte_begin = access_begin - (*it)->address;
    ADDRESS access_size =
        byte_begin + bytes_to_read > line_size_B ?
            line_size_B - byte_begin : bytes_to_read;

    (*it)->Read(access_begin, access_size);
  }
  delete lines;
  PIN_MutexUnlock(&lock);
}

void Write(ADDRESS addr, UINT32 size) {
  PIN_MutexLock(&lock);
  std::vector<CacheLine*>* lines = mlc->Access(addr, size);

  uint32_t bytes_to_read = size;
  for (std::vector<CacheLine*>::iterator it = lines->begin(); it != lines->end();
      it++) {
    ADDRESS access_begin = addr > (*it)->address ? addr : (*it)->address;
    uint32_t byte_begin = access_begin - (*it)->address;
    ADDRESS access_size =
        byte_begin + bytes_to_read > line_size_B ?
            line_size_B - byte_begin : bytes_to_read;

    (*it)->Write(access_begin, access_size);
    bytes_to_read -= access_size;
  }
  delete lines;
  PIN_MutexUnlock(&lock);
}

void Instruction(INS ins, void *v) {
  if (LEVEL_CORE::INS_IsMemoryRead(ins)) {
    if (KnobTrackLoads) {
      INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Read,
          IARG_MEMORYREAD_EA, IARG_MEMORYREAD_SIZE, IARG_END);
    }
  }

  if (LEVEL_CORE::INS_IsMemoryWrite(ins) && !KnobIgnoreStores) {
    if (KnobTrackStores) {
      INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR) Write,
          IARG_MEMORYWRITE_EA, IARG_MEMORYWRITE_SIZE, IARG_END);
    }
  }
}

/*!
 * This function is called for every thread created by the application when it is
 * about to start running (including the root thread).
 * @param[in]   threadIndex     ID assigned by PIN to the new thread
 * @param[in]   ctxt            initial register state for the new thread
 * @param[in]   flags           thread creation flags (OS specific)
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddThreadStartFunction function call
 */
void ThreadStart(THREADID threadIndex, CONTEXT *ctxt, INT32 flags, void *v) {
}

/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddFiniFunction function call
 */
void Fini(INT32 code, void *v) {
  std::ofstream out(KnobOutputFile.Value().c_str());

  out << "===============================================" << std::endl;
  out << "VCache_Pintool analysis results: " << std::endl;
  for (uint32_t i = 0; i < line_size_B; i++) {
    out << "Utilization " << unsigned(i + 1) << ": " << unsigned(mlc->byte_utilizations.at(i))
        << std::endl;
  }
  out << "===============================================" << std::endl;

  out.close();

  PIN_MutexFini(&lock);
}

bool Validate_Knobs() {
  UINT32 levels = KnobCacheLevels.Value();
  return (levels == KnobCacheSize.NumberOfValues()
      && levels == KnobAssociativity.NumberOfValues());
}

/*!
 * The main procedure of the tool.
 * This function is called when the application image is loaded but not yet started.
 * @param[in]   argc            total number of elements in the argv array
 * @param[in]   argv            array of command line arguments, 
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
    return -1;
  }
  std::vector<uint32_t> capacities_B;
  std::vector<uint32_t> associativities;
  line_size_B = KnobLineSize.Value();

  for (uint8_t i = 0; i < KnobCacheLevels.Value(); i++) {
    capacities_B.push_back(KnobCacheSize.Value(i));
    associativities.push_back(KnobAssociativity.Value(i));
  }

  mlc = new MultilevelCache(capacities_B, associativities, line_size_B);

  INS_AddInstrumentFunction(Instruction, 0);
  PIN_AddFiniFunction(Fini, 0);

  PIN_MutexInit(&lock);
  // Never returns

  PIN_StartProgram();

  return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
