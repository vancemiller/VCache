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

TEST_F(AssociativeCacheTest, CacheInsertAfterEvictLRU) {
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line1));
  ASSERT_EQ(line0, cache->EvictLRU(0));
  ASSERT_TRUE(cache->Insert(*line0));
  ASSERT_TRUE(cache->Insert(*line2));
  ASSERT_TRUE(cache->Insert(*line3));
  ASSERT_EQ(line2, cache->EvictLRU(1));
  ASSERT_TRUE(cache->Insert(*line2));
}

TEST_F(AssociativeCacheTest, CacheEvictLRUNotMapped) {
  ASSERT_EQ(NULL, cache->EvictLRU(0));
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
  cache->EvictLRU(line1->address);
  cache->EvictLRU(line3->address);
  ASSERT_FALSE(cache->Contains(line0->address));
  ASSERT_TRUE(cache->Contains(line1->address));
  ASSERT_FALSE(cache->Contains(line2->address));
  ASSERT_TRUE(cache->Contains(line3->address));
}

TEST_F(AssociativeCacheTest, CacheAccessLine) {
  ASSERT_EQ(NULL, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line3->address, LINE_SIZE));
  cache->Insert(*line0);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line3->address, LINE_SIZE));
  cache->Insert(*line1);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line3->address, LINE_SIZE));
  cache->Insert(*line2);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(line2, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line3->address, LINE_SIZE));
  cache->Insert(*line3);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(line2, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(line3, cache->AccessLine(line3->address, LINE_SIZE));
  cache->EvictLRU(line3->address);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(line3, cache->AccessLine(line3->address, LINE_SIZE));
  cache->EvictLRU(line2->address);
  ASSERT_EQ(line0, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(line3, cache->AccessLine(line3->address, LINE_SIZE));
  cache->EvictLRU(line1->address);
  ASSERT_EQ(NULL, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(line3, cache->AccessLine(line3->address, LINE_SIZE));
  cache->EvictLRU(line0->address);
  ASSERT_EQ(NULL, cache->AccessLine(line0->address, LINE_SIZE));
  ASSERT_EQ(line1, cache->AccessLine(line1->address, LINE_SIZE));
  ASSERT_EQ(NULL, cache->AccessLine(line2->address, LINE_SIZE));
  ASSERT_EQ(line3, cache->AccessLine(line3->address, LINE_SIZE));
}

TEST(LargeAssociativeCacheTest, BigTest) {
  int ins_count = 0;
  int match_count = 0;
  int remove_count = 0;
  static const uint8_t LINE_SIZE_B = 64;
  // Cache with 64 byte lines,
  // 8-way associativity
  // 16 sets
  const int associativity = 4;
  const int sets = 64;
  const int access_max = 4096 * associativity;
  Cache* c = Cache::Create(associativity * sets * LINE_SIZE_B, associativity,
      LINE_SIZE_B);

  std::map<ADDRESS, CacheLine*> lines;

  for (int set = 0; set < sets; set++) {
    for (int access = 0; access < access_max; access++) {

      uint32_t tag = rand() << (c->n_bits_offset + c->n_bits_set);
      uint32_t set_addr = set << c->n_bits_offset;
      uint32_t line_offset = rand() & ((1 << c->n_bits_offset) - 1);
      uint8_t n_bytes = rand() % LINE_SIZE_B;

      ADDRESS line_start = tag | set_addr;
      ADDRESS address = tag | set_addr | line_offset;

      if (!c->Contains(address)) {
        CacheLine* line = new CacheLine(LINE_SIZE_B, line_start);
        if (!c->Insert(*line)) {
          CacheLine* evicted = c->EvictLRU(address);
          ASSERT_FALSE(evicted == NULL);
          ASSERT_TRUE(c->Insert(*line));
          lines.erase(line_start);
          delete evicted;
          remove_count++;
        }

        lines[line_start] = line;
        ins_count++;
      } else {
        CacheLine* requested = c->AccessLine(address, n_bytes);
        ASSERT_EQ(lines[line_start], requested);
        match_count++;
      }
    }
  }
  ASSERT_EQ(access_max * sets, ins_count + match_count);

  delete c;
}

}
