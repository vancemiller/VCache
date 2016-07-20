/*
 * LineTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"

#include "gtest/gtest.h"

namespace {

class CacheLineTest: public ::testing::Test {
protected:
  static const uint32_t LINE_SIZE = 64;
  static const uint32_t LINE_ADDRESS = 0xdeadbeef; // an arbitrary 32-bit address
  static const uint32_t n_bytes_rw = 4;
protected:
  CacheLine *line;
  struct AddressBits *addressBits;
  virtual void SetUp() {
    line = new CacheLine(LINE_SIZE, LINE_ADDRESS);
  }

  virtual void TearDown() {
    delete line;
  }
};

TEST_F(CacheLineTest, AccessedBytes1) {
  const uint32_t access_one_byte = 1;
  for (uint32_t i = 0; i < LINE_SIZE; i++) {
    line->Access(LINE_ADDRESS + i, access_one_byte);
    ASSERT_TRUE(line->getAccessedBytes()[i]);
    for (uint32_t j = i + 1; j < LINE_SIZE; j++) {
      ASSERT_FALSE(line->getAccessedBytes()[j]);
    }
  }
}

TEST_F(CacheLineTest, AccessedBytes2) {
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Access(LINE_ADDRESS + i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes()[i + j]);
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_FALSE(line->getAccessedBytes()[j]);
    }
  }
}

TEST_F(CacheLineTest, AccessedBytes3) {
  line->Access(LINE_ADDRESS, LINE_SIZE);
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Access(LINE_ADDRESS + i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes()[i + j]);
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_TRUE(line->getAccessedBytes()[j]);
    }
  }
}

TEST_F(CacheLineTest, LargeAccessTest) {
  const int access_count = 1024 * 1024;
  for (int i = 0; i < access_count; i++) {
    line->Access(LINE_ADDRESS, LINE_SIZE);
  }
}

TEST_F(CacheLineTest, CreateLines) {
  const int create_max = 1024 * 1024;
  for (int i = 0; i < create_max; i++) {
    volatile CacheLine line(LINE_SIZE, LINE_ADDRESS + i);
  }
}
}
