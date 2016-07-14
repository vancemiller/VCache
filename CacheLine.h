/*
 * CacheLine.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef CACHELINE_H_
#define CACHELINE_H_

#include <vector>
#include <iostream>
#include "Address.h"

class CacheLine {
private:
  std::vector<bool> accessed_bytes;
  bool dirty;

private:
  void AccessBytes(const ADDRESS address, const uint8_t size);

public:
  const ADDRESS address;

public:
  CacheLine(uint8_t line_size, ADDRESS address);
  virtual ~CacheLine();

  void Read(const ADDRESS address, const uint8_t size);
  void Write(const ADDRESS address, const uint8_t size);
  bool isDirty() const;
  const std::vector<bool>& getAccessedBytes() const;

  friend std::ostream& operator<<(std::ostream& stream,
      const CacheLine& line) {
    stream << "Address: " << std::hex << line.address << std::dec;
    stream << ", Utilization: ";
    for (std::vector<bool>::const_iterator it = line.accessed_bytes.begin(); it < line.accessed_bytes.end(); it++) {
      if ((*it)) {
        stream << "1";
      } else {
        stream << "0";
      }
    }
    return stream;
  }
};

#endif /* CACHELINE_H_ */
