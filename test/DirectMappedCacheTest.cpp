/*
 * DirectMappedCacheTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/Cache.h"

#include "gtest/gtest.h"

namespace {

class DirectMappedCacheTest: public ::testing::Test {
protected:
  static const uint32_t CACHE_ASSOCIATIVITY = 1;
  static const uint32_t CACHE_CAPACITY_B = 2;
  static const uint32_t LINE_SIZE = 1;
  Cache* cache;
  CacheLine* line0, *line1;
  virtual void SetUp() {
    cache = Cache::Create(CACHE_CAPACITY_B, CACHE_ASSOCIATIVITY, LINE_SIZE);

    line0 = new CacheLine(LINE_SIZE, 0);
    line1 = new CacheLine(LINE_SIZE, 1);
  }

  virtual void TearDown() {
    delete cache;
    delete line0;
    delete line1;
  }
};

TEST_F(DirectMappedCacheTest, CacheInsert) {
  ASSERT_TRUE(cache->Insert(*line0));
}

TEST_F(DirectMappedCacheTest, CacheInsert2) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
}

TEST_F(DirectMappedCacheTest, CacheInsertDuplicates) {
  ASSERT_NE(line0->address, line1->address);
  ASSERT_NE(cache->GetSetIndex(line0->address),
      cache->GetSetIndex(line1->address));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_TRUE(cache->Insert(*line1));
}

TEST_F(DirectMappedCacheTest, CacheInsertAfterEvict) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_EQ(line0, cache->Evict(0));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_EQ(line1, cache->Evict(1));
  ASSERT_TRUE(cache->Insert(*line1));
}

TEST_F(DirectMappedCacheTest, CacheEvictNotMapped) {
  ASSERT_EQ(NULL, cache->Evict(0));
}

TEST_F(DirectMappedCacheTest, CacheContains) {
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_FALSE(cache->Contains(line1->address));
  cache->Insert(*line0);
  ASSERT_TRUE(cache->Contains(line0->address));
  ASSERT_FALSE(cache->Contains(line1->address));
  cache->Evict(line0->address);
  cache->Insert(*line1);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_TRUE(cache->Contains(line1->address));
  cache->Evict(line0->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_TRUE(cache->Contains(line1->address));
  cache->Evict(line1->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_FALSE(cache->Contains(line1->address));
}

TEST_F(DirectMappedCacheTest, CacheGetLine) {
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(NULL, cache->GetLine(line1->address));
  cache->Insert(*line0);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(NULL, cache->GetLine(line1->address));
  cache->Insert(*line1);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  cache->Evict(line0->address);
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  cache->Evict(line1->address);
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(NULL, cache->GetLine(line1->address));
}

TEST_F(DirectMappedCacheTest, CacheRemoveLine) {
  cache->Insert(*line0);
  ASSERT_TRUE(cache->Contains(line0->address));
  cache->RemoveLine(line0->address);
  ASSERT_FALSE(cache->Contains(line0->address));
}
}
