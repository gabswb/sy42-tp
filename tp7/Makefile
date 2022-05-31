
#name of the target
PROJECT = HAL_STM32F401

#----------------------------------------------------------------------
# TOOL DEFINITIONS
#----------------------------------------------------------------------
PREFIX=arm-none-eabi-#arm-atollic-eabi-
AS      = $(PREFIX)as
CC      = $(PREFIX)gcc
CPP     = $(PREFIX)g++
LD      = $(PREFIX)gcc
OBJCOPY = $(PREFIX)objcopy
OBJDUMP = $(PREFIX)objdump
SIZE 	= $(PREFIX)size

#----------------------------------------------------------------------
# sources files
#----------------------------------------------------------------------
# main files
SRC_DIR = ./Core
CSRCS = $(SRC_DIR)/Src/main.c $(SRC_DIR)/Src/app_tasks.c \
		$(SRC_DIR)/Src/syscalls.c $(SRC_DIR)/Src/sysmem.c $(SRC_DIR)/Src/trace.c
CSRCS += $(SRC_DIR)/Src/system_stm32f4xx.c $(SRC_DIR)/Src/stm32f4xx_it.c 
CSRCS += $(SRC_DIR)/Src/stm32f4xx_hal_msp.c 
CSRCS += $(SRC_DIR)/Src/stm32f4xx_hal_timebase_tim.c
CSRCS += $(SRC_DIR)/Src/gpio.c 
CSRCS += $(SRC_DIR)/Src/tim.c 
CSRCS += $(SRC_DIR)/Src/usart.c 
CSRCS += $(SRC_DIR)/Src/i2c.c 
CSRCS += $(SRC_DIR)/Src/spi.c 

# add startup file to build
ASRCS = $(SRC_DIR)/Startup/startup_stm32f401retx.s 

##############   HAL    ###############
STM_LIBRARY = .
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
CSRCS_LIB += $(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c


CSRCS += $(CSRCS_LIB)
##############   FreeRTOS    ###############
#FreeRTOS sources directory
FREERTOS=./FreeRTOS

# add sources files for freeRTOS (depend on FreeRTOSConfig.h)
# others sources files can be added if you want to use 
# some OS services (timer, event, ..)
CSRCS += $(FREERTOS)/Source/queue.c
CSRCS += $(FREERTOS)/Source/tasks.c
CSRCS += $(FREERTOS)/Source/list.c
CSRCS += $(FREERTOS)/Source/portable/GCC/ARM_CM4F/port.c
#Choose a heap management sources :
CSRCS += $(FREERTOS)/Source/portable/MemMang/heap_4.c

#----------------------------------------------------------------------
# headers files
#----------------------------------------------------------------------
#PATH to STM32Cube library (here integrated in the project for headers)
STM_LIBRARY = .

INCLUDE_PATHS = -I$(SRC_DIR)/Inc 

#CMSIs library 
INCLUDE_PATHS += -I$(STM_LIBRARY)/Drivers/CMSIS/Include \
					-I$(STM_LIBRARY)/Drivers/CMSIS/Device/ST/STM32F4xx/Include

##############   HAL    ###############
INCLUDE_PATHS += -I$(STM_LIBRARY)/Drivers/STM32F4xx_HAL_Driver/Inc

##############   FreeRTOS    ###############
#add FreeRTOS Headers in the libraries search for gcc
INCLUDE_PATHS += -I$(FREERTOS)/Source/portable/GCC/ARM_CM4F 
INCLUDE_PATHS += -I$(FREERTOS)/Source/include 

#----------------------------------------------------------------------
#link script
#----------------------------------------------------------------------

LINKER_SCRIPT = ./STM32F401RETX_FLASH.ld

#----------------------------------------------------------------------
# COMPILER AND ASSEMBLER OPTIONS
#----------------------------------------------------------------------
#compiler options
CPU = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -specs=nano.specs
CC_FLAGS  = $(CPU) -c -ffunction-sections -fdata-sections -Wall -fstack-usage
AS_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16


#debug flags 
  CC_FLAGS +=  -O0 -g
  AS_FLAGS += -gdwarf-2
  CC_SYMBOLS = -DDEBUG #-DUSE_FULL_ASSERT
#no debug
# CC_FLAGS +=  -Os


#define the target version (needed in some include file) :
CC_SYMBOLS += -DSTM32F401xE 


##############   HAL    ###############
CC_SYMBOLS += -DUSE_HAL_DRIVER

#----------------------------------------------------------------------
# linker options and library to link
#----------------------------------------------------------------------

LD_FLAGS = $(CPU) -Wl,--gc-sections  -Wl,-Map=$(PROJECT).map,--cref 
LD_SYS_LIBS = -lm -lc -lgcc -lnosys 
LD_SYS_LIBS += -lstdc++ -lsupc++ 


#first target to avoid writting 'make all' each time
$(PROJECT):$(PROJECT).elf
#----------------------------------------------------------------------
# BUILD OBJECTS
#----------------------------------------------------------------------

# list all object files
OBJECTS = $(CSRCS:.c=.o) $(ASRCS:.s=.o) $(CPPSRCS:.cpp=.o) 
OBJECTS_LIB = $(CSRCS_LIB:.c=.o) 
dep = $(OBJECTS:.o=.d)  # one dependency file for each source
su=$(OBJECTS:.o=.su)

#compile all object files
%.o:%.c
	$(CC) $(CC_FLAGS) $(CC_SYMBOLS) $(INCLUDE_PATHS) -o $@ $<
%.o:%.s
	$(AS) $(AS_FLAGS) -o $@ $<
%.o:%.cpp
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98  $(INCLUDE_PATHS) -o $@ $< 

#dependencie
-include $(dep)   # include all dep files in the makefile
#dependencies files .d
%.d:%.c
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) $(CC_SYMBOLS) $< -MM -MT $(@:.d=.o) >$@

#----------------------------------------------------------------------
# BUILD PROJECT (link)
#----------------------------------------------------------------------

$(PROJECT).elf: $(OBJECTS) $(OBJECTS_LIB)
	$(LD) -o $@ $^ $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LD_SYS_LIBS) $(LIBS) $(LIBRARY_PATHS)
	$(SIZE) $@

# other binary files il needeed

$(PROJECT).bin: $(PROJECT).elf
	@$(OBJCOPY) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROJECT).lst

size:
	$(SIZE) $(PROJECT).elf


#inclusion du contenu des fichiers .d
#-include $(CDEP)

all: $(PROJECT).bin $(PROJECT).hex 

.PHONY: clean
clean:
	rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map 
	rm -f $(PROJECT).lst $(OBJECTS) $(OBJECTS_LIB) $(dep) $(su)
.PHONY: cleandep
cleandep:
	rm -f $(dep)



