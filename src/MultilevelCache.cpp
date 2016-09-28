/*
 * MultilevelCache.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#include "MultilevelCache.h"

#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <math.h>
#include <stdexcept>
#include <stddef.h>

MultilevelCache::MultilevelCache(const std::vector<uint64_t>& capacities_B,
    const std::vector<uint16_t>& associativities, const uint16_t line_size_B) :
    n_levels(capacities_B.size()), line_size_B(line_size_B) {
//  if (capacities_B.size() != associativities.size()) {
//    throw std::invalid_argument(
//        "Capacity and associativity arguments must be the same length.");
//  }
  hits = 0;
  misses = 0;
  std::vector<uint64_t>::const_iterator cap = capacities_B.begin();
  std::vector<uint16_t>::const_iterator ass = associativities.begin();
  int level = 1;
  while (cap != capacities_B.end()) {
    caches.push_back(Cache::Create(*cap, *ass, line_size_B));
    cap++;
    ass++;
    level++;
  }

  byte_utilizations.resize(line_size_B, 0l);
}

MultilevelCache::~MultilevelCache() {
  for (std::vector<Cache*>::iterator it = caches.begin(); it < caches.end();
      it++) {
    delete (*it);
  }
}

std::vector<CacheLine*>& MultilevelCache::Access(const ADDRESS address,
    const uint8_t n_bytes) {
  return SplitAccess(address, n_bytes);
}

std::vector<CacheLine*>& MultilevelCache::SplitAccess(const ADDRESS address,
    const uint8_t size) {
  std::vector<CacheLine*>* accessed_lines = new std::vector<CacheLine*>;

  // We need to compute the line offset for the access address. This is computable
  // via a Cache object. The line offset for an address is the same across all caches
  // because the line size is fixed, so we can choose any cache for this. Choose L1.
  const Cache* const L1 = caches.at(0);
  uint32_t bytes_to_end_of_line = line_size_B - L1->GetLineOffset(address);
  uint8_t bytes_accessed = 0;
  uint8_t bytes_remaining = size;

  do {
    uint8_t access_size = (
        bytes_remaining < bytes_to_end_of_line ?
            bytes_remaining : bytes_to_end_of_line);

    accessed_lines->push_back(
        &InclusiveAccess(address + bytes_accessed, access_size));

    bytes_accessed += access_size;
    bytes_remaining -= access_size;
    bytes_to_end_of_line =
        bytes_remaining < line_size_B ? bytes_remaining : line_size_B;
  } while (bytes_remaining > 0);
  return *accessed_lines;
}

CacheLine& MultilevelCache::InclusiveAccess(const ADDRESS address,
    const uint8_t size_B) {
  CacheLine* requested = NULL;

  std::vector<Cache*>::iterator cache = caches.begin();

  // First, search the L1 for the line
  requested = (*cache)->AccessLine(address, size_B);

  CacheLine* evicted = NULL;
  // If the line was not found in L1, evict a line if the cache is full
  if (requested == NULL) {
    // Evict returns NULL if there is space available.
    evicted = (*cache)->EvictLRU(address);
  }

  cache++;

  // If we have evicted something from the previous level, insert it into the current level
  // while continuing to search for the requested line.
  while (cache < caches.end() && requested == NULL && evicted != NULL) {
    // Search for the requested line
    requested = (*cache)->AccessLine(address, size_B);
    // Evict a line if the cache is full to make room for the evicted line
    CacheLine* const tmp = (*cache)->EvictLRU(evicted->address);
    // Insert the previously evicted line
    (*cache)->Insert(*evicted);
    // Repeat if necessary...
    evicted = tmp;
    cache++;
  }

  // If we are not holding any evicted line and have not found the requested line,
  // continue searching.
  while (cache < caches.end() && requested == NULL && evicted == NULL) {
    requested = (*cache)->AccessLine(address, size_B);

    cache++;
  }

  // We have reached the end of the cache hierarchy.
  if (requested == NULL) {
    // Line was not mapped in cache. Create it and insert it in L1.
    requested = new CacheLine(line_size_B,
        address - caches.front()->GetLineOffset(address));
    requested->Access(address, size_B);
    caches.front()->Insert(*requested);
    misses++;
  } else {
    hits++;
  }

  if (evicted != NULL) {
    // Remove line from all other levels of the hierarchy (inclusive cache).
    for (std::vector<Cache*>::iterator it = caches.begin(); it != caches.end();
        it++) {
      (*it)->RemoveLine(evicted->address);
    }
    // Count utilized bits
    boost::dynamic_bitset<> accessedBytes = evicted->getAccessedBytes();
    int utilization = accessedBytes.count();
    if (utilization) {
      byte_utilizations.at(utilization - 1)++;
    }
    // We have evicted a line from the cache hierarchy. Delete it.
    std::cout << "Evicted: " << *evicted << std::endl;
    delete evicted;
  }
  return *requested;
}
