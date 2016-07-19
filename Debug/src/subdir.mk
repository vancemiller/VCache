################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Cache.cpp \
../src/CacheLine.cpp \
../src/CacheSet.cpp \
../src/MultilevelCache.cpp 

OBJS += \
./src/Cache.o \
./src/CacheLine.o \
./src/CacheSet.o \
./src/MultilevelCache.o 

CPP_DEPS += \
./src/Cache.d \
./src/CacheLine.d \
./src/CacheSet.d \
./src/MultilevelCache.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


