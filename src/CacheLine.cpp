/*
 * CacheLine.cpp
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#include "CacheLine.h"


CacheLine::CacheLine(const uint8_t line_size, const ADDRESS address) :
    dirty(false), address(address) {
  accessed_bytes.resize(line_size, false);
}

CacheLine::~CacheLine() {
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

void CacheLine::AccessBytes(const ADDRESS address, const uint8_t size) {
  for (uint32_t i = address - this->address; i < address - this->address + size;
      i++) {
    accessed_bytes.at(i) = true;
  }
}
