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

TEST_F(DirectMappedCacheSetTest, SetEvictFull) {
  set->Insert(*line0);
  ASSERT_EQ(line0, set->Evict());
}

TEST_F(DirectMappedCacheSetTest, SetEvictEmpty) {
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(DirectMappedCacheSetTest, SetEvictFullEmpty) {
  set->Insert(*line0);
  ASSERT_EQ(line0, set->Evict());
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(DirectMappedCacheSetTest, SetEvictEmptyFull) {
  ASSERT_EQ(NULL, set->Evict());
  set->Insert(*line0);
  ASSERT_EQ(line0, set->Evict());
}

TEST_F(DirectMappedCacheSetTest, SetContains) {
  ASSERT_FALSE(set->Contains(line0->address));
  set->Insert(*line0);
  ASSERT_TRUE(set->Contains(line0->address));
  set->Evict();
  ASSERT_FALSE(set->Contains(line0->address));
}

TEST_F(DirectMappedCacheSetTest, SetGetLine) {
  ASSERT_EQ(NULL, set->GetLine(line0->address));
  set->Insert(*line0);
  ASSERT_EQ(line0, set->GetLine(line0->address));
  set->Evict();
  ASSERT_EQ(NULL, set->GetLine(line0->address));
}

TEST_F(DirectMappedCacheSetTest, SetInsertDuplicate) {
  ASSERT_TRUE(set->Insert(*line0));
  ASSERT_TRUE(set->Insert(*line0));
}

TEST_F(DirectMappedCacheSetTest, CacheRemoveLine) {
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

}
