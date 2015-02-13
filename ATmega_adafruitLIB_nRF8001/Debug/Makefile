################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../utility \
../utility/hal \
../utility/uart


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Adafruit_BLE_UART.cpp \
../ATmega_adafruitLIB_nRF8001.cpp \
../SPI.cpp \
../utility/acilib.cpp \
../utility/aci_setup.cpp \
../utility/hal_aci_tl.cpp \
../utility/lib_aci.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Adafruit_BLE_UART.o \
ATmega_adafruitLIB_nRF8001.o \
SPI.o \
utility/acilib.o \
utility/aci_setup.o \
utility/hal_aci_tl.o \
utility/lib_aci.o

OBJS_AS_ARGS +=  \
Adafruit_BLE_UART.o \
ATmega_adafruitLIB_nRF8001.o \
SPI.o \
utility/acilib.o \
utility/aci_setup.o \
utility/hal_aci_tl.o \
utility/lib_aci.o

C_DEPS +=  \
Adafruit_BLE_UART.d \
ATmega_adafruitLIB_nRF8001.d \
SPI.d \
utility/acilib.d \
utility/aci_setup.d \
utility/hal_aci_tl.d \
utility/lib_aci.d

C_DEPS_AS_ARGS +=  \
Adafruit_BLE_UART.d \
ATmega_adafruitLIB_nRF8001.d \
SPI.d \
utility/acilib.d \
utility/aci_setup.d \
utility/hal_aci_tl.d \
utility/lib_aci.d

OUTPUT_FILE_PATH +=ATmega_adafruitLIB_nRF8001.elf

OUTPUT_FILE_PATH_AS_ARGS +=ATmega_adafruitLIB_nRF8001.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler















./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

utility/%.o: ../utility/%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega328p -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="ATmega_adafruitLIB_nRF8001.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega328p  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "ATmega_adafruitLIB_nRF8001.elf" "ATmega_adafruitLIB_nRF8001.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "ATmega_adafruitLIB_nRF8001.elf" "ATmega_adafruitLIB_nRF8001.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "ATmega_adafruitLIB_nRF8001.elf" > "ATmega_adafruitLIB_nRF8001.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "ATmega_adafruitLIB_nRF8001.elf" "ATmega_adafruitLIB_nRF8001.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "ATmega_adafruitLIB_nRF8001.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "ATmega_adafruitLIB_nRF8001.elf" "ATmega_adafruitLIB_nRF8001.a" "ATmega_adafruitLIB_nRF8001.hex" "ATmega_adafruitLIB_nRF8001.lss" "ATmega_adafruitLIB_nRF8001.eep" "ATmega_adafruitLIB_nRF8001.map" "ATmega_adafruitLIB_nRF8001.srec" "ATmega_adafruitLIB_nRF8001.usersignatures"
	