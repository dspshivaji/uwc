####################################################################################
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation. Title to the
# Material remains with Intel Corporation.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or delivery of
# the Materials, either expressly, by implication, inducement, estoppel or otherwise.
####################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Test/src/Main_ut.cpp \
../Test/src/ModbusOnDemandHandler_ut.cpp \
../Test/src/ModbusStackInterface_ut.cpp \
../Test/src/PeriodicRead_ut.cpp \
../Test/src/PublishJson_ut.cpp \
../Test/src/YamlUtil_ut.cpp 

OBJS += \
./Test/src/Main_ut.o \
./Test/src/ModbusOnDemandHandler_ut.o \
./Test/src/ModbusStackInterface_ut.o \
./Test/src/PeriodicRead_ut.o \
./Test/src/PublishJson_ut.o \
./Test/src/YamlUtil_ut.o 

CPP_DEPS += \
./Test/src/Main_ut.d \
./Test/src/ModbusOnDemandHandler_ut.d \
./Test/src/ModbusStackInterface_ut.d \
./Test/src/PeriodicRead_ut.d \
./Test/src/PublishJson_ut.d \
./Test/src/YamlUtil_ut.d 


# Each subdirectory must supply rules for building sources it contributes
Test/src/%.o: ../Test/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -lrt -std=c++11 -fpermissive -DUNIT_TEST -DMODBUS_STACK_TCPIP_ENABLED -DINSTRUMENTATION_LOG -I../$(PROJECT_DIR)/include -I/usr/local/include -I../$(PROJECT_DIR)/include/utils -I../$(PROJECT_DIR)/../bin/yaml-cpp/include -I../$(PROJECT_DIR)/../bin/safestring/include -O0 -g3 -ftest-coverage -fprofile-arcs -Wall -c -fmessage-length=0 -fPIE -O2 -D_FORTIFY_SOURCE=2 -static -fvisibility=hidden -fvisibility-inlines-hidden -Wformat -Wformat-security -fstack-protector-strong -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


