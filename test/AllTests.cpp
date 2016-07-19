/*
 * AllTests.cpp
 *
 *  Created on: Jul 1, 2016
 *      Author: vance
 */

#include "gtest/gtest.h"

#include "AddressTest.cpp"
#include "AssociativeCacheSetTest.cpp"
#include "AssociativeCacheTest.cpp"
#include "CacheLineTest.cpp"
#include "DirectMappedCacheSetTest.cpp"
#include "DirectMappedCacheTest.cpp"
#include "LargeMultilevelCacheTest.cpp"
#include "MultilevelCacheTest.cpp"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
