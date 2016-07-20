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

typedef uint32_t ADDRESS;
typedef uint32_t TAG;
typedef uint32_t SET_INDEX;
typedef uint32_t LINE_OFFSET;

#define BITS_IN_BYTE 8

class Address {
public:
  /**
   * Returns the number of bits required to represent n_sets.
   */
  static const uint8_t GetSetBitCount(uint32_t n_sets) {
    return (uint8_t) ceil(log2(n_sets));
  }

  /**
   * Returns the number of bits required to represent the byte offset
   * of an address with line_size_B byte cache lines.
   */
  static const uint8_t GetOffsetBitCount(uint32_t line_size_B) {
    return ((uint8_t) ceil(log2(line_size_B)));
  }

  /**
   * Returns the number of bits required to represent the tag of an
   * address with n_bits_set and n_bits_offset.
   */
  static const uint8_t GetTagBitCount(uint8_t n_bits_set, uint8_t n_bits_offset) {
    // sizeof returns bytes: convert to bits.
    return sizeof(ADDRESS) * BITS_IN_BYTE - (n_bits_set + n_bits_offset);
  }

  /**
   * Returns the number of sets required in a cache of capacity_B bytes
   * with *-way associativity and line_size_B byte cache lines.
   */
  static const uint32_t GetSetCount(uint64_t capacity_B, uint16_t associativity,
      uint16_t line_size_B) {
    return (uint32_t) ceil(
        (long double) capacity_B / (long double) (associativity * line_size_B));
  }

  /**
   * Returns a bitmask that is the size of the address type.
   */
  static const uint64_t GetAddressMask() {
    return (((uint64_t) 1) << (sizeof(ADDRESS) * BITS_IN_BYTE)) - 1;
  }
};

#endif /* ADDRESS_H_ */

