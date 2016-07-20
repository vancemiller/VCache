/*
 * LargeMultilevelCacheTest.cpp
 *
 *  Created on: Jul 18, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/Cache.h"
#include "../src/MultilevelCache.h"

#include "gtest/gtest.h"

namespace {

class LargeMultilevelCacheTest: public ::testing::Test {
protected:
  MultilevelCache* cache;

  virtual void SetUp() {
    std::vector<uint64_t> capacities_B;
    std::vector<uint16_t> associativities;
    uint32_t line_size_B = 64;
    capacities_B.push_back(1024 * 1024);
    capacities_B.push_back(2048 * 1024);
    capacities_B.push_back(4096l * 1024l * 1024l);
    associativities.push_back(4);
    associativities.push_back(8);
    associativities.push_back(8);
    cache = new MultilevelCache(capacities_B, associativities, line_size_B);
  }

  virtual void TearDown() {
    delete cache;
  }
};

TEST_F(LargeMultilevelCacheTest, CacheAccess) {
  const int access_max = 1024 * 1024;
  const int size_max = 16;
  for (int i = 0; i < access_max; i++) {
    ADDRESS address = rand();
    size_t size = rand() % size_max;
    std::vector<CacheLine*> lines = cache->Access(address, size);
  }
  ASSERT_TRUE(true);
}

}
