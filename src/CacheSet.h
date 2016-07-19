/*
 * CacheSet.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef CACHESET_H_
#define CACHESET_H_

#include <boost/unordered_map.hpp>
#include <vector>

#include "Address.h"
#include "CacheLine.h"
#include "Cache.h"

// Forward declaration
class Cache;

class CacheSet {
private:
  const Cache* const cache;
  boost::unordered_map<TAG, CacheLine*> lines_map;
  std::list<CacheLine*> lines_list;

public:
  CacheSet(const Cache* const cache);
  virtual ~CacheSet();

  /**
   * Inserts the CacheLine into the CacheSet. Returns true on success and false if the cache is full.
   *
   * If the CacheLine is already be mapped in the CacheSet, moves line to the LRU position and returns true.
   */
  bool Insert(CacheLine& line);

  /**
   * Evicts the least recently used line.
   */
  CacheLine* const EvictLRU();

  /**
   * Returns true iff the cache set contains a line for address.
   * Only examines the TAG field of the address.
   */
  bool Contains(const ADDRESS address) const;

  /**
   * Returns the CacheLine for address or NULL if the line is not mapped.
   * Only examines the TAG field of the address.
   */
  CacheLine* const GetLine(const ADDRESS address) const;

  /**
   * Removes the line corresponding to address.
   * Only examines the TAG field of the address.
   */
  void RemoveLine(const ADDRESS address);
};

#endif /* CACHESET_H_ */
