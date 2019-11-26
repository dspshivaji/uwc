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
../src/BoostLogger.cpp \
../src/Main.cpp \
../src/ModbusStackInterface.cpp \
../src/NetworkInfo.cpp \
../src/PeriodicRead.cpp \
../src/PublishJson.cpp \
../src/ZmqHandler.cpp 

OBJS += \
./src/BoostLogger.o \
./src/Main.o \
./src/ModbusStackInterface.o \
./src/NetworkInfo.o \
./src/PeriodicRead.o \
./src/PublishJson.o \
./src/ZmqHandler.o 

CPP_DEPS += \
./src/BoostLogger.d \
./src/Main.d \
./src/ModbusStackInterface.d \
./src/NetworkInfo.d \
./src/PeriodicRead.d \
./src/PublishJson.d \
./src/ZmqHandler.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -lrt -std=c++11 -fpermissive -DBOOST_LOG_DYN_LINK=1 -I../$(PROJECT_DIR)/include -I../$(PROJECT_DIR)/../bin/yaml-cpp/include -I../$(PROJECT_DIR)/include/utils -I../$(PROJECT_DIR)/../bin/boost/include -I../$(PROJECT_DIR)/../bin/safestring/include -I../$(PROJECT_DIR)/../bin/ssl/include -O0 -g3 -Wall -c -fmessage-length=0 -fPIE -O2 -D_FORTIFY_SOURCE=2 -static -fvisibility=hidden -fvisibility-inlines-hidden -Wformat -Wformat-security -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


