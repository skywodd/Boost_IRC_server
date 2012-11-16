################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Channel_info.cpp \
../Channels_manager.cpp \
../Connection.cpp \
../Prefix_wrapper.cpp \
../Reply_generator.cpp \
../Request_handler.cpp \
../Request_parser.cpp \
../Sanity_check.cpp \
../Server.cpp \
../User_info.cpp \
../Users_manager.cpp \
../main.cpp 

OBJS += \
./Channel_info.o \
./Channels_manager.o \
./Connection.o \
./Prefix_wrapper.o \
./Reply_generator.o \
./Request_handler.o \
./Request_parser.o \
./Sanity_check.o \
./Server.o \
./User_info.o \
./Users_manager.o \
./main.o 

CPP_DEPS += \
./Channel_info.d \
./Channels_manager.d \
./Connection.d \
./Prefix_wrapper.d \
./Reply_generator.d \
./Request_handler.d \
./Request_parser.d \
./Sanity_check.d \
./Server.d \
./User_info.d \
./Users_manager.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ $(BUILD_OPS) $(BOOST_INCLUDE_PATH) -c -fmessage-length=0 -ffunction-sections -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


