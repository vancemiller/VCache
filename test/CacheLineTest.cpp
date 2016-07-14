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
  static const uint32_t line_offset = 0;
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

TEST_F(CacheLineTest, LineIsClean1) {
  ASSERT_FALSE(line->isDirty());
}

TEST_F(CacheLineTest, LineIsClean2) {
  line->Read(line_offset, n_bytes_rw);
  ASSERT_FALSE(line->isDirty());
}

TEST_F(CacheLineTest, LineIsDirty1) {
  line->Write(line_offset, n_bytes_rw);
  ASSERT_TRUE(line->isDirty());
}

TEST_F(CacheLineTest, LineIsDirty2) {
  line->Write(line_offset, n_bytes_rw);
  line->Read(line_offset, n_bytes_rw);
  ASSERT_TRUE(line->isDirty());
}

TEST_F(CacheLineTest, AccessedBytes1) {
  const uint32_t read_one_byte = 1;
  for (uint32_t i = 0; i < LINE_SIZE; i++) {
    line->Read(i, read_one_byte);
    ASSERT_TRUE(line->getAccessedBytes().at(i));
    for (uint32_t j = i + 1; j < LINE_SIZE; j++) {
      ASSERT_FALSE(line->getAccessedBytes().at(j));
    }
  }
}

TEST_F(CacheLineTest, AccessedBytes2) {
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Read(i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(i + j));
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_FALSE(line->getAccessedBytes().at(j));
    }
  }
}

TEST_F(CacheLineTest, AccessedBytes3) {
  line->Read(0, LINE_SIZE);
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Read(i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(i + j));
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(j));
    }
  }
}

/**
 * Same as test 2 but with Write.
 */
TEST_F(CacheLineTest, AccessedBytes4) {
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Write(i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(i + j));
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_FALSE(line->getAccessedBytes().at(j));
    }
  }
}

/**
 * Same as test 3 but with Write.
 */
TEST_F(CacheLineTest, AccessedBytes5) {
  line->Write(0, LINE_SIZE);
  for (uint32_t i = 0; i < LINE_SIZE; i += n_bytes_rw) {
    line->Write(i, n_bytes_rw);
    for (uint32_t j = 0; j < n_bytes_rw; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(i + j));
    }
    for (uint32_t j = i + n_bytes_rw; j < LINE_SIZE; j++) {
      ASSERT_TRUE(line->getAccessedBytes().at(j));
    }
  }
}
}
