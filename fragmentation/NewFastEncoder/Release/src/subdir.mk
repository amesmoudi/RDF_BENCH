################################################################################
# Automatically-generated file. Do not edit!
################################################################################
# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/FastEncoder.cpp \
../src/TurtleParser.cpp \
../src/partitioner_store.cpp \
../src/sorter.cpp \
../src/fragmenter.cpp \
../src/ReferenceIndexer.cpp \
../src/DictionaryIndexer.cpp \
../src/FragmentReencoder.cpp \
../src/profiler.cpp \
../src/RocksDBHandler.cpp \
../src/utils.cpp 

OBJS += \
./src/FastEncoder.o \
./src/TurtleParser.o \
./src/partitioner_store.o \
../src/sorter.o \
../src/fragmenter.o \
../src/ReferenceIndexer.o \
../src/DictionaryIndexer.o \
../src/FragmentReencoder.o \
./src/profiler.o \
./src/RocksDBHandler.o \
./src/utils.o 

CPP_DEPS += \
./src/FastEncoder.d \
./src/TurtleParser.d \
./src/partitioner_store.d \
../src/sorter.d \
../src/fragmenter.d \
../src/ReferenceIndexer.d \
../src/DictionaryIndexer.d \
../src/FragmentReencoder.d \
./src/profiler.d \
./src/RocksDBHandler.d \
./src/utils.d 

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -g -O0 -I/usr/local/include -I/home/harbir/sparsehash/include -O3 -Wall -c -fmessage-length=0 \
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '
