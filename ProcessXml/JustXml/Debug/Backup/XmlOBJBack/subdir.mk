################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Backup/XmlOBJBack/RapidXmlObject.cpp 

OBJS += \
./Backup/XmlOBJBack/RapidXmlObject.o 

CPP_DEPS += \
./Backup/XmlOBJBack/RapidXmlObject.d 


# Each subdirectory must supply rules for building sources it contributes
Backup/XmlOBJBack/%.o: ../Backup/XmlOBJBack/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cygwin C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


