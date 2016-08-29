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

bool CacheSet::Insert(CacheLine& line) {
  boost::unordered_map<TAG, CacheLine*>::const_iterator it = lines_map.find(
      cache->GetTag(line.address));
  if (it != lines_map.end() && it->second == &line) {
    // Line was already mapped. Move it to front of LRU list.
    lines_list.remove(&line);
    lines_list.push_front(&line);
    return true;
  }
  if (lines_list.size() == cache->associativity) {
    // Set is full. Insert failed.
    return false;
  }
  lines_map.insert(
      std::pair<TAG, CacheLine*>(cache->GetTag(line.address), &line));
  lines_list.push_front(&line);
  return true;
}

CacheLine* const CacheSet::EvictLRU() {
  CacheLine* line = NULL;
  if (lines_list.size() == cache->associativity) {
    line = lines_list.back();
    if (line != NULL) {
      lines_map.erase(cache->GetTag(line->address));
      lines_list.remove(line);
    }
  }
  return line;
}

bool CacheSet::Contains(const ADDRESS address) const {
  TAG tag = cache->GetTag(address);
  boost::unordered_map<TAG, CacheLine*>::const_iterator it = lines_map.find(
      tag);
  return it != lines_map.end();
}

CacheLine* const CacheSet::AccessLine(const ADDRESS address, const uint8_t n_bytes) const {
  CacheLine* line = NULL;
  TAG tag = cache->GetTag(address);
  boost::unordered_map<TAG, CacheLine*>::const_iterator it = lines_map.find(
      tag);
  if (it != lines_map.end()) {
    line = it->second;
    line->Access(address, n_bytes);
  }
  return line;
}

void CacheSet::RemoveLine(const ADDRESS address) {
  TAG tag = cache->GetTag(address);
  boost::unordered_map<TAG, CacheLine*>::const_iterator it = lines_map.find(
      tag);
  if (it != lines_map.end()) {
    lines_list.remove(it->second);
    lines_map.erase(tag);
  }
}

