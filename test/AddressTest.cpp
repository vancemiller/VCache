/*
 * AddressTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/Cache.h"

#include "gtest/gtest.h"

namespace {

static const uint32_t LINE_SIZE_B = 64;

TEST(AddressTest, AddressSets) {
  ASSERT_EQ(2, Address::n_sets(128, 1, LINE_SIZE_B));
  ASSERT_EQ(2, Address::n_sets(256, 2, LINE_SIZE_B));
  ASSERT_EQ(1, Address::n_sets(256, 4, LINE_SIZE_B));
  ASSERT_EQ(4, Address::n_sets(1024, 4, LINE_SIZE_B));
  ASSERT_EQ(32, Address::n_sets(16384, 8, LINE_SIZE_B));

}

TEST(AddressTest, AddressOffsetBits) {
  for (int i = 1; i < 1024; i *= 2) {
    ASSERT_EQ(log2(i), Address::n_bits_offset(i));
  }
}

TEST(AddressTest, AddressTagBits) {
  for (unsigned int i = 0;
      i < sizeof(ADDRESS) * 8 - Address::n_bits_offset(LINE_SIZE_B); i++) {
    ASSERT_EQ(sizeof(ADDRESS) * 8 - Address::n_bits_offset(LINE_SIZE_B) - i,
        Address::n_bits_tag(i, Address::n_bits_offset(LINE_SIZE_B)));
  }
}

}
