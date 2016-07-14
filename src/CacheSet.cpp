/*
 * CacheSet.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#include "CacheSet.h"

#include <stddef.h>
#include <set>
#include <iterator>

CacheSet::CacheSet(const Cache* const cache) :
    cache(cache) {
}

CacheSet::~CacheSet() {
}

/**
 * Inserts the CacheLine into the CacheSet. Returns true on success and false if the cache is full.
 *
 * If the CacheLine is already be mapped in the CacheSet, moves line to the LRU position and returns true.
 */
bool CacheSet::Insert(CacheLine& line) {
  if (lines_map.find(cache->GetTag(line.address))->second == &line) {
    lines_list.remove(&line);
    lines_list.push_front(&line);
    return true;
  }
  if (lines_list.size() == cache->associativity) {
    return false;
  }
  lines_map.insert(
      std::pair<TAG, CacheLine*>(cache->GetTag(line.address), &line));
  lines_list.push_front(&line);
  return true;
}

/**
 * Evicts the least recently used line.
 */
CacheLine* const CacheSet::Evict() {
  if (lines_list.size() == cache->associativity) {
    CacheLine* const line = lines_list.back();
    if (line != NULL) {
      lines_map.erase(cache->GetTag(line->address));
      lines_list.remove(line);
    }
    return line;
  } else {
    return NULL;
  }
}

bool CacheSet::Contains(const ADDRESS address) const {
  TAG tag = cache->GetTag(address);
  std::map<TAG, CacheLine*>::const_iterator it = lines_map.find(tag);
  if (it != lines_map.end()) {
    return true;
  } else {
    return false;
  }
}

CacheLine* const CacheSet::GetLine(const ADDRESS address) const {
  TAG tag = cache->GetTag(address);
  std::map<TAG, CacheLine*>::const_iterator it = lines_map.find(tag);
  if (it != lines_map.end()) {
    return it->second;
  } else {
    return NULL;
  }
}

void CacheSet::RemoveLine(const ADDRESS address) {
  TAG tag = cache->GetTag(address);
  std::map<TAG, CacheLine*>::const_iterator it = lines_map.find(tag);
  if (it != lines_map.end()) {
    lines_list.remove(it->second);
    lines_map.erase(tag);
  }
}

