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
  std::vector<CacheLine*>& SplitAccess(const ADDRESS address,
      const uint8_t n_bytes);

  CacheLine& SearchExclusive(const ADDRESS address, const uint8_t size_B);
  CacheLine& SearchInclusive(const ADDRESS address, const uint8_t size_B);

public:
  const uint8_t n_levels;
  const uint32_t line_size_B;

public:
  MultilevelCache(const std::vector<uint32_t>& capacities_B,
      const std::vector<uint32_t>& assocativities, const uint32_t line_size_B =
      DEFAULT_LINE_SIZE);
  virtual ~MultilevelCache();

  std::vector<CacheLine*>& Access(const ADDRESS address, const uint8_t n_bytes);

  void PrintSummary();
};

#endif /* MULTILEVELCACHE_H_ */
