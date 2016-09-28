/*
 * CacheLine.h
 *
 *  Created on: Jun 27, 2016
 *      Author: vance
 */

#ifndef CACHELINE_H_
#define CACHELINE_H_

#include <boost/dynamic_bitset.hpp>
#include <iostream>

#include "Address.h"

class CacheLine {
private:
  // A vector<bool> is a bit vector and will use accessed_bytes bits of storage.
  boost::dynamic_bitset<> *accessed_bytes;

public:
  const ADDRESS address;

private:
  /**
   * Marks bytes as accessed in the accessed_bytes vector.
   *
   * @param address the start of the access.
   * @param size the number of bytes accessed.
   */
  void AccessBytes(const ADDRESS address, const uint8_t size);

public:
  /**
   * Constructs a CacheLine.
   *
   * @param line_size the number of bytes in the line.
   * @param line_start_address the address of the first byte in the line.
   */
  CacheLine(uint8_t line_size, ADDRESS line_start_address);

  /**
   * CacheLine destructor.
   */
  virtual ~CacheLine();

  /**
   * Accesses the CacheLine. Sets accessed_bytes according to address and size.
   */
  void Access(const ADDRESS address, const uint8_t size);

  /**
   * Returns a bitset of the accessed bytes in the cache line.
   * The bit at position i is set iff byte i was accessed.
   */
  const boost::dynamic_bitset<>& getAccessedBytes() const;

  friend std::ostream& operator<<(std::ostream& stream, const CacheLine& line) {
    stream << "Address: " << std::hex << line.address << std::dec;
    stream << ", Utilization: ";
    for (size_t i = 0; i < line.accessed_bytes->size(); i++) {
      stream << ((*(line.accessed_bytes))[i] ? "1" : "0");
    }
    return stream;
  }
};

#endif /* CACHELINE_H_ */
