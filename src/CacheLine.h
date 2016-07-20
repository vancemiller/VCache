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
  bool dirty;

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
   * Read the CacheLine. Sets accessed_bytes according to address and size.
   */
  void Read(const ADDRESS address, const uint8_t size);

  /**
   * Write the CacheLine. Sets accessed_bytes according to address and size and
   * sets the dirty bit.
   */
  void Write(const ADDRESS address, const uint8_t size);

  /**
   * Returns true iff the line has been written to.
   */
  bool isDirty() const;

  /**
   * Returns a vector<bool> of the accessed bytes in the cache line.
   * The bit at position i is set iff byte i was accessed.
   */
  const boost::dynamic_bitset<>& getAccessedBytes() const;

  friend std::ostream& operator<<(std::ostream& stream, const CacheLine& line) {
    stream << "Address: " << std::hex << line.address << std::dec;
    stream << ", Utilization: " << line.accessed_bytes;
    return stream;
  }
};

#endif /* CACHELINE_H_ */
