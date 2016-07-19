/*
 * Cache.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef CACHE_H_
#define CACHE_H_

#include <stdint.h>
#include <vector>
#include <math.h>

#include "Address.h"
#include "CacheSet.h"
#include "CacheLine.h"

// Forward Declaration
class CacheSet;

class Cache {
private:
  std::vector<CacheSet*> sets;

public:
  const uint32_t associativity;
  const uint32_t n_bits_tag;
  const uint8_t n_bits_set;
  const uint8_t n_bits_offset;
  const uint64_t address_mask;

private:
  Cache(const uint32_t n_sets, const uint32_t associativity,
      const uint8_t n_bits_tag, const uint8_t n_bits_set,
      const uint8_t n_bits_offset, const uint64_t address_mask);

public:
  /**
   * Factory constructor for a Cache.
   */
  static Cache* const Create(const uint32_t capacity_B,
      const uint32_t associativity, const uint32_t line_size_B) {
    const uint32_t n_sets = Address::GetSetCount(capacity_B, associativity,
        line_size_B);
    const uint8_t n_bits_set = Address::GetSetBitCount(n_sets);
    const uint8_t n_bits_offset = Address::GetOffsetBitCount(line_size_B);
    const uint8_t n_bits_tag = Address::GetTagBitCount(n_bits_set,
        n_bits_offset);
    const uint64_t address_mask = Address::GetAddressMask();
    return new Cache(n_sets, associativity, n_bits_tag, n_bits_set,
        n_bits_offset, address_mask);
  }

  virtual ~Cache();

  /**
   * Returns true iff the line was successfully inserted into the cache.
   * A failure occurs when there is no space available in the cache. To
   * handle failures, call Evict(line.address) to make space for the line.
   */
  bool Insert(CacheLine& line);

  /**
   * Evicts the least recently used line mapped to by address.
   * Returns the evicted line or NULL if no line was evicted.
   * Examines only the SET portion of the address.
   */
  CacheLine* const EvictLRU(const ADDRESS address);

  /**
   * Returns true iff the cache contains a line matching address.
   * Examines the SET and TAG portions of the address.
   */
  bool Contains(const ADDRESS address) const;

  /**
   * Returns a line mapped to by address or NULL if a line is not mapped.
   * Examines the SET and TAG portions of the address.
   */
  CacheLine* const GetLine(const ADDRESS address) const;

  /**
   * Removes the line corresponding to address or does nothing if a line
   * is not mapped.
   * Examines the SET and TAG portions of the address.
   */
  void RemoveLine(const ADDRESS address);

  /**
   * Given an address, returns the tag portion of the address.
   */
  const TAG GetTag(const ADDRESS address) const;

  /**
   * Given an address, returns the line offset portion of the address.
   */
  const LINE_OFFSET GetLineOffset(const ADDRESS address) const;

  /**
   * Given an address, returns the set index portion of the address.
   */
  const SET_INDEX GetSetIndex(const ADDRESS address) const;
};

#endif /* CACHE_H_ */
