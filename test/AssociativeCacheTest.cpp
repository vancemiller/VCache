/*
 * AssociativeCacheTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/Cache.h"

#include "gtest/gtest.h"

namespace {

class AssociativeCacheTest: public ::testing::Test {
protected:
  static const uint32_t CACHE_ASSOCIATIVITY = 2;
  static const uint32_t CACHE_CAPACITY_B = 4;
  static const uint32_t LINE_SIZE = 1;
  Cache* cache;
  CacheLine* line0, *line1, *line2, *line3;
  virtual void SetUp() {
    cache = Cache::Create(CACHE_CAPACITY_B, CACHE_ASSOCIATIVITY, LINE_SIZE);

    line0 = new CacheLine(LINE_SIZE, 0);
    line1 = new CacheLine(LINE_SIZE, 2);
    line2 = new CacheLine(LINE_SIZE, 1);
    line3 = new CacheLine(LINE_SIZE, 3);
  }

  virtual void TearDown() {
    delete cache;
    delete line0;
    delete line1;
  }
};

TEST_F(AssociativeCacheTest, CacheInsert) {
  ASSERT_TRUE(cache->Insert(*line0));
}

TEST_F(AssociativeCacheTest, CacheInsert2) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
}

TEST_F(AssociativeCacheTest, CacheInsert4) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_TRUE(cache->Insert(*line2));
  ASSERT_TRUE(cache->Insert(*line3));
}

TEST_F(AssociativeCacheTest, CacheInsertDuplicates) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_TRUE(cache->Insert(*line2));
  ASSERT_TRUE(cache->Insert(*line3));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_TRUE(cache->Insert(*line2));
  ASSERT_TRUE(cache->Insert(*line3));
}

TEST_F(AssociativeCacheTest, CacheInsertAfterEvict) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_EQ(line0, cache->Evict(0));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line2));
  ASSERT_TRUE(cache->Insert(*line3));
  ASSERT_EQ(line2, cache->Evict(1));
  ASSERT_TRUE(cache->Insert(*line2));
}

TEST_F(AssociativeCacheTest, CacheEvictNotMapped) {
  ASSERT_EQ(NULL, cache->Evict(0));
}

TEST_F(AssociativeCacheTest, CacheContains) {
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_FALSE(cache->Contains(line1->address));
  ASSERT_FALSE(cache->Contains(line2->address));
  ASSERT_FALSE(cache->Contains(line3->address));
  cache->Insert(*line0);
  cache->Insert(*line1);
  cache->Insert(*line2);
  cache->Insert(*line3);
  ASSERT_TRUE(cache->Contains(line0->address));
  ASSERT_TRUE(cache->Contains(line1->address));
  ASSERT_TRUE(cache->Contains(line2->address));
  ASSERT_TRUE(cache->Contains(line3->address));
  cache->Evict(line1->address);
  cache->Evict(line3->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_TRUE(cache->Contains(line1->address));
  ASSERT_FALSE(cache->Contains(line2->address));
  ASSERT_TRUE(cache->Contains(line3->address));
}

TEST_F(AssociativeCacheTest, CacheGetLine) {
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(NULL, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(NULL, cache->GetLine(line3->address));
  cache->Insert(*line0);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(NULL, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(NULL, cache->GetLine(line3->address));
  cache->Insert(*line1);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(NULL, cache->GetLine(line3->address));
  cache->Insert(*line2);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(line2, cache->GetLine(line2->address));
  ASSERT_EQ(NULL, cache->GetLine(line3->address));
  cache->Insert(*line3);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(line2, cache->GetLine(line2->address));
  ASSERT_EQ(line3, cache->GetLine(line3->address));
  cache->Evict(line3->address);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(line3, cache->GetLine(line3->address));
  cache->Evict(line2->address);
  ASSERT_EQ(line0, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(line3, cache->GetLine(line3->address));
  cache->Evict(line1->address);
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(line3, cache->GetLine(line3->address));
  cache->Evict(line0->address);
  ASSERT_EQ(NULL, cache->GetLine(line0->address));
  ASSERT_EQ(line1, cache->GetLine(line1->address));
  ASSERT_EQ(NULL, cache->GetLine(line2->address));
  ASSERT_EQ(line3, cache->GetLine(line3->address));
}

}
