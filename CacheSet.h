/*
 * CacheSet.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef CACHESET_H_
#define CACHESET_H_

#include <map>
#include <list>

#include "Address.h"
#include "CacheLine.h"
#include "Cache.h"

// Forward declaration
class Cache;

class CacheSet {
private:
  const Cache* const cache;
  std::map<TAG, CacheLine*> lines_map;
  std::list<CacheLine*> lines_list;

public:
  CacheSet(const Cache* const cache);
  virtual ~CacheSet();

  bool Insert(CacheLine& line);
  CacheLine* const Evict();
  bool Contains(const ADDRESS address) const;
  CacheLine* const GetLine(const ADDRESS address) const;
  void RemoveLine(const ADDRESS address);
};

#endif /* CACHESET_H_ */
