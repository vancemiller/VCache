/*
 * MultilevelCache.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef MULTILEVELCACHE_H_
#define MULTILEVELCACHE_H_

#include <vector>

#include "Address.h"
#include "Cache.h"

#define DEFAULT_LINE_SIZE 64   // 64 Bytes per block

class MultilevelCache {
private:
  std::vector<Cache*> caches;

private:
  /**
   * Splits the access request if it spans multiple CacheLines.
   * Returns a vector of accessed CacheLines.
   */
  std::vector<CacheLine*>& SplitAccess(const ADDRESS address,
      const uint8_t n_bytes);

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
  CacheLine& SearchInclusive(const ADDRESS address, const uint8_t size_B);

public:
  std::vector<uint64_t> byte_utilizations;
  const uint8_t n_levels;
  const uint32_t line_size_B;

public:
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
  MultilevelCache(const std::vector<uint64_t>& capacities_B,
      const std::vector<uint16_t>& assocativities, const uint16_t line_size_B =
      DEFAULT_LINE_SIZE);
  virtual ~MultilevelCache();

  /**
   * Access the cache for a load or store operation.
   * Returns a vector of CacheLines that contain the address requested.
   */
  std::vector<CacheLine*>& Access(const ADDRESS address, const uint8_t n_bytes);
};

#endif /* MULTILEVELCACHE_H_ */
