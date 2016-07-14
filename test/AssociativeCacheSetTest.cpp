/*
 * AssociativeCacheSetTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/CacheSet.h"

#include "gtest/gtest.h"

namespace {

class AssociativeCacheSetTest: public ::testing::Test {
protected:
  static const uint32_t ASSOCIATIVE_CACHE_ASSOCIATIVITY = 2;
  static const uint32_t ASSOCIATIVE_CACHE_CAPACITY_B = 4;
  static const uint32_t LINE_SIZE_B = 1;
  Cache* cache;
  CacheSet* set;
  CacheLine* line1, *line2, *line3;
  virtual void SetUp() {
    cache = Cache::Create(ASSOCIATIVE_CACHE_CAPACITY_B,
        ASSOCIATIVE_CACHE_ASSOCIATIVITY, LINE_SIZE_B);

    set = new CacheSet(cache);
    line1 = new CacheLine(LINE_SIZE_B, 0);
    line2 = new CacheLine(LINE_SIZE_B, 2);
    line3 = new CacheLine(LINE_SIZE_B, 4);
  }

  virtual void TearDown() {
    delete cache;
    delete set;
    delete line1;
    delete line2;
    delete line3;
  }
};

TEST_F(AssociativeCacheSetTest, SetInsert) {
  ASSERT_TRUE(set->Insert(*line1));
}

TEST_F(AssociativeCacheSetTest, SetInsertTwo) {
  ASSERT_TRUE(set->Insert(*line1));
  ASSERT_TRUE(set->Insert(*line2));
}

TEST_F(AssociativeCacheSetTest, SetInsertFull) {
  ASSERT_TRUE(set->Insert(*line1));
  ASSERT_TRUE(set->Insert(*line2));
  ASSERT_FALSE(set->Insert(*line3));
}

TEST_F(AssociativeCacheSetTest, SetEvictFull) {
  set->Insert(*line1);
  set->Insert(*line2);
  ASSERT_EQ(line1, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetInsertAfterEvictFull) {
  set->Insert(*line1);
  set->Insert(*line2);
  ASSERT_EQ(line1, set->Evict());
  set->Insert(*line3);
  ASSERT_EQ(line2, set->Evict());
  set->Insert(*line1);
  ASSERT_EQ(line3, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetEvictEmpty) {
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetEvictNotFull1) {
  set->Insert(*line1);
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetEvictNotFull2) {
  set->Insert(*line2);
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetEvictFullEmpty) {
  set->Insert(*line2);
  set->Insert(*line1);
  ASSERT_EQ(line2, set->Evict());
  ASSERT_EQ(NULL, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetEvictEmptyFull) {
  ASSERT_EQ(NULL, set->Evict());
  set->Insert(*line1);
  ASSERT_EQ(NULL, set->Evict());
  set->Insert(*line2);
  ASSERT_EQ(line1, set->Evict());
}

TEST_F(AssociativeCacheSetTest, SetContains) {
  ASSERT_FALSE(set->Contains(line1->address));
  ASSERT_FALSE(set->Contains(line2->address));
  set->Insert(*line1);
  ASSERT_TRUE(set->Contains(line1->address));
  ASSERT_FALSE(set->Contains(line2->address));
  set->Insert(*line2);
  ASSERT_TRUE(set->Contains(line1->address));
  ASSERT_TRUE(set->Contains(line2->address));
  set->Evict();
  ASSERT_FALSE(set->Contains(line1->address));
  ASSERT_TRUE(set->Contains(line2->address));
}

TEST_F(AssociativeCacheSetTest, SetGetLine) {
  ASSERT_EQ(NULL, set->GetLine(line1->address));
  ASSERT_EQ(NULL, set->GetLine(line2->address));
  set->Insert(*line1);
  ASSERT_EQ(line1, set->GetLine(line1->address));
  ASSERT_EQ(NULL, set->GetLine(line2->address));
  set->Insert(*line2);
  ASSERT_EQ(line1, set->GetLine(line1->address));
  ASSERT_EQ(line2, set->GetLine(line2->address));
  set->Evict();
  ASSERT_EQ(NULL, set->GetLine(line1->address));
  ASSERT_EQ(line2, set->GetLine(line2->address));
}

TEST_F(AssociativeCacheSetTest, SetInsertDuplicates) {
  ASSERT_TRUE(set->Insert(*line1));
  ASSERT_TRUE(set->Insert(*line1));
  ASSERT_TRUE(set->Insert(*line2));
  ASSERT_TRUE(set->Insert(*line1));
  ASSERT_EQ(line1, set->GetLine(line1->address));
  ASSERT_EQ(line2, set->GetLine(line2->address));
  set->Evict();
  ASSERT_EQ(line1, set->GetLine(line1->address));
  ASSERT_EQ(NULL, set->GetLine(line2->address));
}

TEST_F(AssociativeCacheSetTest, SetRemoveLine) {
  set->Insert(*line1);
  ASSERT_TRUE(set->Contains(line1->address));
  set->RemoveLine(line1->address);
  ASSERT_FALSE(set->Contains(line1->address));
  set->Insert(*line2);
  ASSERT_TRUE(set->Contains(line2->address));
  set->RemoveLine(line2->address);
  ASSERT_FALSE(set->Contains(line2->address));
  set->Insert(*line1);
  set->Insert(*line2);
  ASSERT_TRUE(set->Contains(line1->address));
  ASSERT_TRUE(set->Contains(line2->address));
  set->RemoveLine(line1->address);
  ASSERT_FALSE(set->Contains(line1->address));
  ASSERT_TRUE(set->Contains(line2->address));
  set->RemoveLine(line2->address);
  ASSERT_FALSE(set->Contains(line1->address));
  ASSERT_FALSE(set->Contains(line2->address));
}
}
