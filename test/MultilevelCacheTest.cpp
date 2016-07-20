/*
 * MultilevelCacheTest.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "../src/CacheLine.h"
#include "../src/Cache.h"
#include "../src/MultilevelCache.h"

#include "gtest/gtest.h"

namespace {

class MultilevelCacheTest: public ::testing::Test {
protected:
  MultilevelCache* cache;

  virtual void SetUp() {
    std::vector<uint64_t> capacities_B;
    std::vector<uint16_t> associativities;
    uint16_t line_size_B = 1;
    capacities_B.push_back(1);
    capacities_B.push_back(2);
    capacities_B.push_back(4);
    associativities.push_back(1);
    associativities.push_back(2);
    associativities.push_back(2);
    cache = new MultilevelCache(capacities_B, associativities, line_size_B);
  }

  virtual void TearDown() {
    delete cache;
  }
};

TEST_F(MultilevelCacheTest, CacheAccess) {
  cache->Access(0, 1);
  cache->Access(4, 1);
  cache->Access(8, 1);
  cache->Access(12, 1);
  cache->Access(16, 1);
  cache->Access(20, 1);
  cache->Access(24, 1);
  cache->Access(28, 1);
  cache->Access(32, 1);
  cache->Access(36, 1);
  cache->Access(40, 1);
  ASSERT_TRUE(true);
}

}
