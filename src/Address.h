/*
 * Address.h
 *
 *  Created on: Jun 28, 2016
 *      Author: vance
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <stdint.h>
#include <math.h>

#define ADDRESS uint32_t
#define TAG uint32_t
#define SET_INDEX uint32_t
#define LINE_OFFSET uint32_t

class Address {
public:
  static const uint8_t n_bits_set(uint32_t n_sets) {
    return (uint8_t) ceil(log2(n_sets));
  }

  static const uint8_t n_bits_offset(uint32_t line_size_B) {
    return ((uint8_t) ceil(log2(line_size_B)));
  }

  static const uint8_t n_bits_tag(uint8_t n_bits_set, uint8_t n_bits_offset) {
    return sizeof(ADDRESS) * 8 - (n_bits_set + n_bits_offset);
  }

  static const uint32_t n_sets(uint32_t capacity_B, uint32_t associativity,
      uint32_t line_size_B) {
    return (uint32_t) ceil(capacity_B / (associativity * line_size_B));
  }

  static const uint64_t address_mask() {
    return (((uint64_t) 1) << (sizeof(ADDRESS) * 8)) - 1;
  }
};

#endif /* ADDRESS_H_ */

