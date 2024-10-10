################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Core/Internal/CoreAlgorithms.cpp \
../src/Core/Internal/LookupTables.cpp 

OBJS += \
./src/Core/Internal/CoreAlgorithms.o \
./src/Core/Internal/LookupTables.o 

CPP_DEPS += \
./src/Core/Internal/CoreAlgorithms.d \
./src/Core/Internal/LookupTables.d 


# Each subdirectory must supply rules for building sources it contributes
src/Core/Internal/%.o: ../src/Core/Internal/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


