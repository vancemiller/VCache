# VCache
A C++ multilevel cache simulator with an inclusive replacement policy.

Setup:
 1. Clone the repository.
 2. Install pin

  ```bash
  cd VCache
  mkdir pin
  cd pin
  wget http://software.intel.com/sites/landingpage/pintool/downloads/pin-3.0-76991-gcc-linux.tar.gz
  tar -xvf pin-3.0-76991-gcc-linux.tar.gz
  ```
  
 3. Compile the cache simulator with `make`. If you download a different version of pin, edit `makefile` to locate it.
 4. Read about the input knobs to the pintool with
 
  ```bash
  pin/pin-3.0-76991-gcc-linux/pin -t obj-intel64/VCachePintool.so -h -- echo
  ```
 5. Run your experiment. For example to collect data on the `ls` program with a 4-way set-associative 1024 B cache with 64 B cache lines that tracks load and store instructions use:
 
  ```bash
  pin/pin-3.0-76991-gcc-linux/pin -injection child -t obj-intel64/VCachePintool.so -b 64 -a 4 -c 1024 -l -s -- ls
  ```
  Or to define multiple levels of caching, add additional -a and -c parameters. For example 1024 B 4-way associative L1 and 2048 B 8-way associative L2:
 
  ```bash
  pin/pin-3.0-76991-gcc-linux/pin -injection child -t obj-intel64/VCachePintool.so -b 64 -a 4 -c 1024 -a 8 -c 2048 -l -s -- ls
  ```
