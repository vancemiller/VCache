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
  /* Members */
private:
  std::vector<CacheSet*> sets;

public:
  const uint32_t associativity;
  const uint32_t n_bits_tag;
  const uint8_t n_bits_set;
  const uint8_t n_bits_offset;
  const uint64_t address_mask;

  /* Methods */
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
    const uint8_t n_bits_tag = Address::GetTagBitCount(n_bits_set, n_bits_offset);
    const uint64_t address_mask = Address::GetAddressMask();
    return new Cache(n_sets, associativity, n_bits_tag, n_bits_set,
        n_bits_offset, address_mask);
  }

  virtual ~Cache();

  bool Insert(CacheLine& line);
  CacheLine* const Evict(const ADDRESS address);
  bool Contains(const ADDRESS address) const;
  CacheLine* const GetLine(const ADDRESS address) const;
  void RemoveLine(const ADDRESS address);

  const TAG GetTag(const ADDRESS address) const;
  const LINE_OFFSET GetLineOffset(const ADDRESS address) const;
  const SET_INDEX GetSetIndex(const ADDRESS address) const;
};

#endif /* CACHE_H_ */
