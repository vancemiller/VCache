/*
 * DirectMappedCacheSetTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/CacheSet.h"

#include "gtest/gtest.h"

namespace {

class DirectMappedCacheSetTest: public ::testing::Test {
protected:
  static const uint32_t LINE_SIZE_B = 1;
  Cache* cache;
  CacheSet* set;
  CacheLine* line0, *line1;
  virtual void SetUp() {
    cache = Cache::Create(1, 1, LINE_SIZE_B);

    set = new CacheSet(cache);
    line0 = new CacheLine(LINE_SIZE_B, 0);
    line1 = new CacheLine(LINE_SIZE_B, 1);
  }

  virtual void TearDown() {
    delete cache;
    delete set;
    delete line0;
    delete line1;
  }
};

TEST_F(DirectMappedCacheSetTest, SetInsert) {
  ASSERT_TRUE(set->Insert(*line0));
}

TEST_F(DirectMappedCacheSetTest, SetInsertFull) {
  ASSERT_TRUE(set->Insert(*line0));
  ASSERT_FALSE(set->Insert(*line1));
}

TEST_F(DirectMappedCacheSetTest, SetEvictLRUFull) {
  set->Insert(*line0);
  ASSERT_EQ(line0, set->EvictLRU());
}

TEST_F(DirectMappedCacheSetTest, SetEvictLRUEmpty) {
  ASSERT_EQ(NULL, set->EvictLRU());
}

TEST_F(DirectMappedCacheSetTest, SetEvictLRUFullEmpty) {
  set->Insert(*line0);
  ASSERT_EQ(line0, set->EvictLRU());
  ASSERT_EQ(NULL, set->EvictLRU());
}

TEST_F(DirectMappedCacheSetTest, SetEvictLRUEmptyFull) {
  ASSERT_EQ(NULL, set->EvictLRU());
  set->Insert(*line0);
  ASSERT_EQ(line0, set->EvictLRU());
}

TEST_F(DirectMappedCacheSetTest, SetContains) {
  ASSERT_FALSE(set->Contains(line0->address));
  set->Insert(*line0);
  ASSERT_TRUE(set->Contains(line0->address));
  set->EvictLRU();
  ASSERT_FALSE(set->Contains(line0->address));
}

TEST_F(DirectMappedCacheSetTest, SetAccessLine) {
  ASSERT_EQ(NULL, set->AccessLine(line0->address, LINE_SIZE_B));
  set->Insert(*line0);
  ASSERT_EQ(line0, set->AccessLine(line0->address, LINE_SIZE_B));
  set->EvictLRU();
  ASSERT_EQ(NULL, set->AccessLine(line0->address, LINE_SIZE_B));
}

TEST_F(DirectMappedCacheSetTest, SetInsertDuplicate) {
  ASSERT_TRUE(set->Insert(*line0));
  ASSERT_TRUE(set->Insert(*line0));
}

TEST_F(DirectMappedCacheSetTest, SetRemoveLine) {
  ASSERT_FALSE(set->Contains(line0->address));
  set->Insert(*line0);
  ASSERT_TRUE(set->Contains(line0->address));
  set->RemoveLine(line0->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  set->Insert(*line1);
  ASSERT_TRUE(set->Contains(line1->address));
  ASSERT_FALSE(cache->Contains(line0->address));
  set->RemoveLine(line0->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_TRUE(set->Contains(line1->address));
}

TEST_F(DirectMappedCacheSetTest, SetLargeInsert) {
  const int access_max = 1024 * 1024;
  for (int i = 0; i < access_max; i++) {
    CacheLine *line = new CacheLine(rand(), LINE_SIZE_B);
    if (!set->Insert(*line)) {
      CacheLine* evicted = set->EvictLRU();
      delete evicted;
      set->Insert(*line);
    }
  }
}

}
