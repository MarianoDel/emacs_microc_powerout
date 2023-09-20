#
#       !!!! Do NOT edit this makefile with an editor which replace tabs by spaces !!!!
#
##############################################################################################
#
# On command line:
#
# make all = Create project
#
# make clean = Clean project files.
#
# To rebuild project do "make clean" and "make all".
#
# Included originally in the yagarto projects. Original Author : Michael Fischer
# Modified to suit our purposes by Hussam Al-Hertani
# Use at your own risk!!!!!
##############################################################################################
# Start of default section
#
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc -x assembler-with-cpp
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = cortex-m3

# List all default C defines here, like -D_DEBUG=1
# for STM32F103RCT6 micro
DDEFS = -DSTM32F10X_HD
# for STM32F051C8T6 micro
# DDEFS = -DSTM32F051
# for STM32F030K6T6 micro
# DDEFS = -DSTM32F030

# List all default ASM defines here, like -D_DEBUG=1
DADEFS =

# List all default directories to look for include files here
DINCDIR = ./src

# List the default directory to look for the libraries here
DLIBDIR =

# List all default libraries here
DLIBS =

#
# End of default section
##############################################################################################

##############################################################################################
# Start of user section
#

#
# Define project name here
PROJECT = MicroC_pout

# List C define here
CORELIBDIR = ./cmsis_core
LINKER = ./startup_src

# Sources Files
SRC  = ./src/main.c
SRC += ./src/gpio.c
SRC += ./src/it.c
SRC += ./src/adc.c
SRC += ./src/usart.c
SRC += ./src/tim.c
SRC += ./src/dma.c
SRC += ./src/hard.c
SRC += ./src/dac.c

# SRC += ./src/dsp.c
# SRC += ./src/comms_from_rasp.c
# SRC += ./src/treatment.c
# SRC += ./src/utils.c
SRC += ./src/test_functions.c

# SRC += ./src/comms_channels.c
# SRC += ./src/antennas.c
# SRC += ./src/signals.c
# SRC += ./src/errors.c

# SRC += ./src/tamper_funcs.c
# SRC += ./src/flash_program.c


## Core Support
SRC += ./startup_src/syscalls.c
SRC += ./startup_src/startup_clocks.c
SRC += ./cmsis_core/core_cm3.c

# List ASM source files here
ASRC = ./startup_src/startup_stm32f10x_hd.s

# List all user directories here
UINCDIR = $(BOOTDIR) \
          $(CORELIBDIR)


# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

# Define optimisation level here
# O1 optimiza size no significativo
# O2 size mas fuerte
# Os (size mas fuerte que O2)
# O3 el mas fuerte, seguro despues no corre
# O0 (no optimiza casi nada, mejor para debug)
OPT = -O0

#
# End of user defines
##############################################################################################
#
# Define linker script file here
#
LDSCRIPT = $(LINKER)/stm32_flash.ld
FULL_PRJ = $(PROJECT)_rom

INCDIR  = $(patsubst %,-I%,$(DINCDIR) $(UINCDIR))
LIBDIR  = $(patsubst %,-L%,$(DLIBDIR) $(ULIBDIR))

ADEFS   = $(DADEFS) $(UADEFS)

LIBS    = $(DLIBS) $(ULIBS)
MCFLAGS = -mcpu=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb  -Wa,-amhls=$(<:.s=.lst) $(ADEFS)

# SIN INFO DEL DEBUGGER + STRIP CODE
# CPFLAGS = $(MCFLAGS) $(OPT) -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst)

# CON INFO PARA DEBUGGER
#CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DEFS)

# CON INFO PARA DEBUGGER + STRIP CODE
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DDEFS)

# SIN DEAD CODE, hace el STRIP
LDFLAGS = $(MCFLAGS) -mthumb -lm --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
# CON DEAD CODE
#LDFLAGS = $(MCFLAGS) -mthumb --specs=nano.specs -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)
#LDFLAGS = $(MCFLAGS) -mthumb -T$(LDSCRIPT) -Wl,-Map=$(FULL_PRJ).map,--cref,--no-warn-mismatch $(LIBDIR)

#
# makefile rules
#

assemblersources = $(ASRC)
sources = $(SRC)
OBJS  = $(SRC:.c=.o) $(ASRC:.s=.o)

objects = $(sources:.c=.o)
assobjects = $(assemblersources:.s=.o)

all: $(objects) $(assobjects) $(FULL_PRJ).elf $(FULL_PRJ).bin
	arm-none-eabi-size $(FULL_PRJ).elf
	gtags -q

$(objects): %.o: %.c
	$(CC) -c $(CPFLAGS) -I. $(INCDIR) $< -o $@

$(assobjects): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

%elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN)  $< $@

flash:
	sudo openocd -f stm32f1_flash.cfg

gdb:
	sudo openocd -f stm32f1_gdb.cfg

reset:
	sudo openocd -f stm32f1_reset.cfg

clean:
	rm -f $(OBJS)
	rm -f $(FULL_PRJ).elf
	rm -f $(FULL_PRJ).map
	rm -f $(FULL_PRJ).hex
	rm -f $(FULL_PRJ).bin
	rm -f $(SRC:.c=.lst)
	rm -f $(ASRC:.s=.lst)
	rm -f *.o
	rm -f *.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f *.gcov


tests:
	# simple functions tests, copy functions to test into main
	gcc -c src/tests_ok.c
	gcc src/tests.c tests_ok.o
	./a.out


tests_treatment:
	# compile first modules in this test
	# first module objects to test
	gcc -c --coverage src/treatment.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/utils.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_mock_usart.c -I $(INCDIR)
	gcc --coverage src/tests_treatment.c treatment.o utils.o tests_ok.o tests_mock_usart.o -I $(INCDIR) $(DDEFS)
	./a.out
	# process coverage
	gcov treatment.c -m


tests_treat_all_chain:
	# compile first modules in this test
	# first module objects to test
	# gcc -c src/treatment.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/treatment.c -I. $(INCDIR) $(DDEFS) -DTESTING_SHOW_INFO
	gcc -c src/comms_from_rasp.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/utils.c -I. $(INCDIR)
	gcc -c src/antennas.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/errors.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS) -DTESTING_SHOW_INFO
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS) -DTESTING_SHOW_INFO_PI -DTESTING_SHOW_INFO_OPENLOOP
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)  -DTESTING_SHOW_INFO_OPENLOOP -DTESTING_SHOW_INFO_OPENLOOP_INDEX
	gcc -c src/signals.c -I. $(INCDIR) $(DDEFS) -DTESTING_SHOW_INFO -DTESTING_SHOW_INFO_OPENLOOP
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS) -DTESTING_SHOW_INFO -DTESTING_SHOW_INFO_INDEX_SP
	gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_mock_usart.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc src/tests_treat_all_chain.c treatment.o comms_from_rasp.o utils.o antennas.o errors.o signals.o dsp.o tests_ok.o tests_mock_usart.o tests_recursive_utils.o tests_vector_utils.o tests_know_antennas.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_comms:
	# compile first modules in this test
	# first module objects to test
	gcc -c --coverage src/comms.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/utils.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_mock_usart.c -I $(INCDIR)
	gcc --coverage src/tests_comms.c comms.o tests_ok.o tests_mock_usart.o -I $(INCDIR) $(DDEFS)
	./a.out
	# process coverage
	gcov comms.c -m


tests_signals:
	# compile first modules in this test
	# first module objects to test
	gcc -c --coverage src/signals.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc --coverage src/tests_signals.c signals.o dsp.o tests_ok.o tests_vector_utils.o tests_know_antennas.o -I $(INCDIR) $(DDEFS)
	./a.out
	# process coverage
	gcov signals.c -m


tests_signals_simul:
	# compile first modules in this test
	# first module objects to test
	gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc src/tests_signals_simul.c signals.o dsp.o tests_ok.o tests_know_antennas.o tests_vector_utils.o tests_recursive_utils.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_signals_simul2:
	# compile first modules in this test
	# first module objects to test
	gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc src/tests_signals_simul2.c signals.o dsp.o tests_ok.o tests_know_antennas.o tests_vector_utils.o tests_recursive_utils.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_signals_transform:
	# compile first modules in this test
	# first module objects to test
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc src/tests_signals_transform.c -lm tests_ok.o tests_know_antennas.o tests_vector_utils.o tests_recursive_utils.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_signals_transform2:
	# compile first modules in this test
	# first module objects to test
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc src/tests_signals_transform2.c -lm tests_ok.o tests_know_antennas.o tests_vector_utils.o tests_recursive_utils.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_signals_transform3:
	# compile first modules in this test
	# first module objects to test
	# gcc -c src/signals.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc -c src/tests_recursive_utils.c -I $(INCDIR)
	gcc src/tests_signals_transform3.c -lm tests_ok.o tests_know_antennas.o tests_vector_utils.o tests_recursive_utils.o -I $(INCDIR) $(DDEFS)
	./a.out


tests_antennas:
	# compile first modules in this test
	# first module objects to test
	gcc -c --coverage src/antennas.c -I. $(INCDIR) $(DDEFS)
	# gcc -c src/dsp.c -I. $(INCDIR) $(DDEFS)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_mock_usart.c -I $(INCDIR)
	gcc -c src/tests_know_antennas.c -I $(INCDIR)
	# gcc -c src/tests_vector_utils.c -I $(INCDIR)
	gcc --coverage src/tests_antennas.c antennas.o tests_ok.o tests_mock_usart.o tests_know_antennas.o -I $(INCDIR) $(DDEFS)
	./a.out
	# process coverage
	gcov antennas.c -m

tests_comms_rasp:
	# compile first modules in this test
	gcc -c src/comms_from_rasp.c -I. $(INCDIR) -DSTM32F10X_HD
	gcc -c src/treatment.c -I. $(INCDIR)
	gcc -c src/utils.c -I. $(INCDIR)
	# second auxiliary helper modules
	gcc -c src/tests_ok.c -I $(INCDIR)
	gcc -c src/tests_mock_usart.c -I $(INCDIR)
	gcc src/tests_comms_rasp.c comms_from_rasp.o treatment.o utils.o tests_ok.o tests_mock_usart.o -I $(INCDIR) $(DDEFS)
	./a.out



# *** EOF ***
