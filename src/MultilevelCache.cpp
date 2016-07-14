/*
 * MultilevelCache.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#include "MultilevelCache.h"

#include <stdexcept>
#include <stddef.h>
#include <iostream>
#include <math.h>

/**
 * Constructs a multilevel cache.
 *
 * First level cache must be specified in the first entry of vector, second
 * in second entry and so on.
 *
 * @param capacities_B Cache capacities for each level of the cache, in bytes.
 * @param associativities Cache associativities for each level of the cache.
 * @param line_size_B The number of bytes each cache line will hold, defaults to 64.
 */
MultilevelCache::MultilevelCache(const std::vector<uint32_t>& capacities_B,
    const std::vector<uint32_t>& associativities, const uint32_t line_size_B) :
    n_levels(capacities_B.size()), line_size_B(line_size_B) {
  if (capacities_B.size() != associativities.size()) {
    throw std::invalid_argument(
        "Capacity and associativity arguments must be the same length.");
  }
  std::vector<uint32_t>::const_iterator cap = capacities_B.begin();
  std::vector<uint32_t>::const_iterator ass = associativities.begin();
  int level = 1;
  while (cap != capacities_B.end()) {
    caches.push_back(Cache::Create(*cap, *ass, line_size_B));
    std::cerr << "------------------  L" << unsigned(level)
        << " cache  ------------------" << std::endl << "Dimensions: " << *cap
        << "B " << *ass << "-way set associative, " << line_size_B << "B lines]"
        << std::endl;
    cap++;
    ass++;
    level++;
  }
}

MultilevelCache::~MultilevelCache() {
  for (std::vector<Cache*>::iterator it = caches.begin(); it < caches.end();
      it++) {
    delete (*it);
  }
}

/**
 * Searches the cache for the CacheLine containing the requested address.
 *
 * Algorithm:
 * 1.  If the line is in the first level cache, fetch it and return.
 * 2.  Else, if the first level cache is full, evict a line.
 * 3.  Search the next level cache for the line. If we find it, remove it.
 * 4.  If we previously evicted a line and the cache is full, evict a line.
 * 4.1.Insert the previously evicted line into the cache.
 * 5.  Repeat 3-4.1 until the end of the cache hierarchy.
 * 6.  If we evicted a line from the LLC, destroy it.
 * 7.  If we have not found the requested cache line, create it (ie. fetch from main memory).
 * 8.  Insert the requested cache line into the L1.
 *
 */
std::vector<CacheLine*>& MultilevelCache::Access(const ADDRESS address,
    const uint8_t n_bytes) {
  std::cerr << "Access: " << std::hex << address << " : " << unsigned(n_bytes)
      << std::dec << std::endl;
  return SplitAccess(address, n_bytes);
}

std::vector<CacheLine*>& MultilevelCache::SplitAccess(const ADDRESS address,
    const uint8_t size) {
  uint8_t bytes_accessed = 0;
  uint8_t bytes_remaining = size;
  std::vector<CacheLine*>* accessed_lines = new std::vector<CacheLine*>();

  // We need to compute the line offset for the access address. This is computable
  // via a Cache object. The line offset for an address is the same across all caches
  // because the line size is fixed, so we can choose any cache for this. Choose L1.
  const Cache* const L1 = caches.at(0);
  const uint32_t line_offset = L1->GetLineOffset(address);
  do {
    uint32_t bytes_to_end_of_line = line_size_B - line_offset;
    uint8_t access_size = (
        bytes_remaining < bytes_to_end_of_line ?
            bytes_remaining : bytes_to_end_of_line);
    accessed_lines->push_back(
        &SearchInclusive(address + bytes_accessed, access_size));
    bytes_accessed += access_size;
    bytes_remaining -= access_size;
  } while (bytes_remaining > 0);
  return *accessed_lines;
}

/**
 * Searches the cache for the CacheLine containing the requested address.
 *
 * Algorithm:
 * 1.  If the line is in the first level cache, read it and return.
 * 2.  Else, if the first level cache is full, evict a line.
 * 3.  Search the next level cache for the line.
 * 4.  If we previously evicted a line and the cache is full, evict a line.
 * 4.1.Insert the previously evicted line into the cache.
 * 5.  Repeat 3-4.1 until the end of the cache hierarchy.
 * 6.  If we evicted a line from the LLC, destroy it and all references to it in other levels.
 * 7.  If we have not found the requested cache line, create it (ie. fetch from main memory).
 * 8.  Insert the requested cache line into the L1.
 *
 */
CacheLine& MultilevelCache::SearchExclusive(const ADDRESS address,
    const uint8_t size_B) {
  CacheLine* requested = NULL;

  std::vector<Cache*>::iterator cache = caches.begin();

  // First, search the L1 for the line
  requested = (*cache)->GetLine(address);

  CacheLine* evicted = NULL;
  // If the line was not found in L1, evict a line if the cache is full
  if (requested == NULL) {
    // Evict returns NULL if there is space available.
    evicted = (*cache)->Evict(address);
  }

  cache++;

  // If we have evicted something from the previous level, insert it into the current level
  // while continuing to search for the requested line.
  while (cache < caches.end() && requested == NULL && evicted != NULL) {
    // Search for the requested line
    if ((*cache)->Contains(address)) {
      requested = (*cache)->GetLine(address);
      // Remove the line from the cache and move it to L1 (exclusive cache).
      (*cache)->RemoveLine(address);
    }
    // Evict a line if the cache is full to make room for the evicted line
    CacheLine* const tmp = (*cache)->Evict(evicted->address);
    // Insert the previously evicted line
    (*cache)->Insert(*evicted);
    // Repeat if necessary...
    evicted = tmp;
    cache++;
  }

  // If we are not holding any evicted line and have not found the requested line,
  // continue searching.
  while (cache < caches.end() && requested == NULL && evicted == NULL) {
    if ((*cache)->Contains(address)) {
      requested = (*cache)->GetLine(address);
      // Remove the line from the cache and move it to L1 (exclusive cache).
      (*cache)->RemoveLine(address);
    }

    cache++;
  }

  // We have reached the end of the cache hierarchy.
  if (requested == NULL) {
    // Line was not mapped in cache. Create it and insert it in L1.
    requested = new CacheLine(line_size_B, address);
    caches.front()->Insert(*requested);
  }

  if (evicted != NULL) {
    // We have evicted a line from the cache hierarchy. Delete it.
    delete evicted;
  }
  return *requested;
}

CacheLine& MultilevelCache::SearchInclusive(const ADDRESS address,
    const uint8_t size_B) {
  CacheLine* requested = NULL;

  std::vector<Cache*>::iterator cache = caches.begin();

  // First, search the L1 for the line
  requested = (*cache)->GetLine(address);

  CacheLine* evicted = NULL;
  // If the line was not found in L1, evict a line if the cache is full
  if (requested == NULL) {
    // Evict returns NULL if there is space available.
    evicted = (*cache)->Evict(address);
  }

  cache++;

  // If we have evicted something from the previous level, insert it into the current level
  // while continuing to search for the requested line.
  while (cache < caches.end() && requested == NULL && evicted != NULL) {
    // Search for the requested line
    requested = (*cache)->GetLine(address);
    // Evict a line if the cache is full to make room for the evicted line
    CacheLine* const tmp = (*cache)->Evict(evicted->address);
    // Insert the previously evicted line
    (*cache)->Insert(*evicted);
    // Repeat if necessary...
    evicted = tmp;
    cache++;
  }

  // If we are not holding any evicted line and have not found the requested line,
  // continue searching.
  while (cache < caches.end() && requested == NULL && evicted == NULL) {
    requested = (*cache)->GetLine(address);

    cache++;
  }

  // We have reached the end of the cache hierarchy.
  if (requested == NULL) {
    // Line was not mapped in cache. Create it and insert it in L1.
    requested = new CacheLine(line_size_B, address);
    caches.front()->Insert(*requested);
  }

  if (evicted != NULL) {
    // Remove line from all other levels of the heirarchy (inclusive cache).
    for (std::vector<Cache*>::iterator it = caches.begin(); it != caches.end();
        it++) {
    }
    // We have evicted a line from the cache hierarchy. Delete it.
    delete evicted;
  }
  return *requested;
}

void MultilevelCache::PrintSummary() {

}
