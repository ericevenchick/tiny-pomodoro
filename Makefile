# AVR Makefile
# #####################################
#
# Part of the uCtools project
# uctools.github.com
#
#######################################
# user configuration:
#######################################
# TARGET: name of the output file
TARGET = main
# MCU: part number to build for
MCU = atmega328p
# SOURCES: list of input source sources
SOURCES = main.c
# OUTDIR: directory to use for output
OUTDIR = build
# PROGRAMMER: name of programmer
PROGRAMMER = arduino
# PORT: location of programmer
PORT = /dev/ttyACM0
# define flags
CFLAGS = -mmcu=$(MCU) -g -O0 -Wall -Wunused
ASFLAGS = -mmcu=$(MCU) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS = -mmcu=$(MCU) -Wl,-Map=$(OUTDIR)/$(TARGET).map
AVRDUDE_FLAGS = -p $(MCU) -c $(PROGRAMMER) -P $(PORT)
#######################################
# end of user configuration
#######################################
#
#######################################
# binaries
#######################################
CC      = avr-gcc
LD      = avr-ld
AR      = avr-ar
AS      = avr-gcc
GASP    = avr-gasp
NM      = avr-nm
OBJCOPY = avr-objcopy
RM      = rm -f
MKDIR	= mkdir -p
AVRDUDE = avrdude
#######################################

# file that includes all dependancies
DEPEND = $(SOURCES:.c=.d)

# list all object files
OBJECTS = $(addprefix $(OUTDIR)/,$(SOURCES:.c=.o))

# default: build all
all: $(OUTDIR)/$(TARGET).elf $(OUTDIR)/$(TARGET).hex $(OUTDIR)/$(TARGET).srec

$(OUTDIR)/%.srec: $(OUTDIR)/%.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

$(OUTDIR)/%.elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

$(OUTDIR)/%.hex: $(OUTDIR)/%.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(OUTDIR)/%.o: src/%.c | $(OUTDIR)
	$(CC) -c $(CFLAGS) -o $@ $<

%.lst: %.c
	$(CC) -c $(ASFLAGS) -Wa,-anlhd $< > $@

# create the output directory
$(OUTDIR):
	$(MKDIR) $(OUTDIR)

# download to mcu flash
flash: $(OUTDIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$<

# verify mcu flash
verify: $(OUTDIR)/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:v:$<

clean:
	-$(RM) $(OUTDIR)/*