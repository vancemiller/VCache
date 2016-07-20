/*
 * Cache.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */
#include "Cache.h"

#include <stddef.h>
#include <iterator>

#include "CacheLine.h"
#include "CacheSet.h"

Cache::Cache(const uint32_t n_sets, const uint32_t associativity,
    const uint8_t n_bits_tag, const uint8_t n_bits_set,
    const uint8_t n_bits_offset, const uint64_t address_mask) :
    associativity(associativity), n_bits_tag(n_bits_tag), n_bits_set(
        n_bits_set), n_bits_offset(n_bits_offset), address_mask(address_mask) {
  // Create cache sets
  sets.resize(n_sets, NULL);
}

Cache::~Cache() {
  for (std::vector<CacheSet*>::iterator cacheSet = sets.begin();
      cacheSet != sets.end(); cacheSet++) {
    if (*cacheSet != NULL) {
      delete *cacheSet;
    }
  }
}

bool Cache::Insert(CacheLine& line) {
  SET_INDEX set_index = GetSetIndex(line.address);
  CacheSet* set = sets.at(set_index);
  if (set == NULL) {
    sets[set_index] = new CacheSet(this);
    set = sets[set_index];
  }
  return set->Insert(line);
}

CacheLine* const Cache::EvictLRU(const ADDRESS address) {
  CacheSet* set = sets.at(GetSetIndex(address));
  if (set != NULL) {
    return set->EvictLRU();
  } else {
    return NULL;
  }
}

bool Cache::Contains(const ADDRESS address) const {
  SET_INDEX set_index = GetSetIndex(address);
  CacheSet* set = sets.at(set_index);
  if (set == NULL) {
    return false;
  } else {
    return set->Contains(address);
  }
}


CacheLine* const Cache::AccessLine(const ADDRESS address, const uint8_t n_bytes) const {
  SET_INDEX set_index = GetSetIndex(address);
  CacheSet* set = sets.at(set_index);
  if (set == NULL) {
    return NULL;
  } else {
    return set->AccessLine(address, n_bytes);
  }
}

void Cache::RemoveLine(const ADDRESS address) {
  SET_INDEX set_index = GetSetIndex(address);
  CacheSet* set = sets.at(set_index);
  if (set != NULL) {
    set->RemoveLine(address);
  }
}

const SET_INDEX Cache::GetSetIndex(const ADDRESS address) const {
  return ((address << n_bits_tag) & address_mask)
      >> (n_bits_tag + n_bits_offset);
}

const LINE_OFFSET Cache::GetLineOffset(const ADDRESS address) const {
  return (address & ((1 << n_bits_offset) - 1)) & address_mask;
}

const TAG Cache::GetTag(const ADDRESS address) const {
  return (address >> (n_bits_set + n_bits_offset)) & address_mask;
}
