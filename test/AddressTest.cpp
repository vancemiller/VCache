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
  ASSERT_EQ(2, Address::GetSetCount(128, 1, LINE_SIZE_B));
  ASSERT_EQ(2, Address::GetSetCount(256, 2, LINE_SIZE_B));
  ASSERT_EQ(1, Address::GetSetCount(256, 4, LINE_SIZE_B));
  ASSERT_EQ(4, Address::GetSetCount(1024, 4, LINE_SIZE_B));
  ASSERT_EQ(32, Address::GetSetCount(16384, 8, LINE_SIZE_B));

}

TEST(AddressTest, AddressOffsetBits) {
  for (int i = 1; i < 1024; i *= 2) {
    ASSERT_EQ(log2(i), Address::GetOffsetBitCount(i));
  }
}

TEST(AddressTest, AddressTagBits) {
  for (unsigned int i = 0;
      i < sizeof(ADDRESS) * BITS_IN_BYTE - Address::GetOffsetBitCount(LINE_SIZE_B); i++) {
    ASSERT_EQ(sizeof(ADDRESS) * BITS_IN_BYTE - Address::GetOffsetBitCount(LINE_SIZE_B) - i,
        Address::GetTagBitCount(i, Address::GetOffsetBitCount(LINE_SIZE_B)));
  }
}

}
