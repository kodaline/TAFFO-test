#BOARD           := -mcpu=arm7tdmi -mfloat-abi=soft                             #lpc
#BOARD           := -mcpu=cortex-m0 -mthumb -mfloat-abi=soft                    #f072
#BOARD           := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft                    #f207
BOARD           := -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16   #f469

MIOSIX_EABI     := /opt/arm-miosix-eabi/arm-miosix-eabi
MIOSIX_CFLAGS   := -target arm-none-linux-eabi --sysroot=$(MIOSIX_EABI) -I$(MIOSIX_EABI)/include/ -I$(MIOSIX_EABI)/include/c++/4.7.3/ -I$(MIOSIX_EABI)/include/c++/4.7.3/arm-miosix-eabi/ -D_MIOSIX -D_MIOSIX_GCC_PATCH_VERSION=1 $(BOARD) -ffunction-sections -Wall

