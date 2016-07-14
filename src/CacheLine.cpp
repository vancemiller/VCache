/*
 * CacheLine.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#include "CacheLine.h"

#include <iostream>

CacheLine::CacheLine(const uint8_t line_size, const ADDRESS address) :
    dirty(false), address(address) {
//  std::cerr << "built cache line " << address << " with size " << unsigned(line_size) << std::endl;
  accessed_bytes.resize(line_size, false);
}

CacheLine::~CacheLine() {
  std::cerr << "Line evicted from cache hierarchy " << *this << std::endl;
}

void CacheLine::Read(const LINE_OFFSET address, const uint8_t size) {
  AccessBytes(address, size);
}

void CacheLine::Write(const LINE_OFFSET address, const uint8_t size) {
  AccessBytes(address, size);
  dirty = true;
}

bool CacheLine::isDirty() const {
  return dirty;
}

const std::vector<bool>& CacheLine::getAccessedBytes() const {
  return accessed_bytes;
}

void CacheLine::AccessBytes(const LINE_OFFSET address, const uint8_t size) {
  std::cerr << "|--Accessed bytes " << address << "-"
      << address + size << std::endl;
  for (uint32_t i = address; i < address + size;
      i++) {
    accessed_bytes.at(i) = true;
  }
  std::cerr << "|--" << *this << std::endl;
}
