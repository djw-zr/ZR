#
################################################################################
#
#   File: makefile
#
#   This file is part of ZR. Released under licence GPL-3.0-or-later.
#   You should have received a copy of the GNU General Public License
#   along with ZR.  If not, see <https://www.gnu.org/licenses/>.
#
#   See README.MAKEFILE
#
################################################################################
#
SHELL      = /bin/sh

#  Specify include and source directories relative to this directory
INC_DIR   := include
SRC_DIR   := src/core src/graphics src/input

#  Specify Compiler and Compiler Options
CC       = gcc
CFLAGS   = -m64 -march=native -Iinclude
#CFLAGS   = -O3 #               Level 3 compiler optimisation
#CFLAGS  += -g -Og #            For valgrind use -g and -Og optimisation
CFLAGS  += -Wall -pedantic
CFLAGS  += -Wno-unused-variable
CFLAGS  += $(foreach dir, $(INC_DIR), $(addprefix -I, $(dir)))
CFLAGS  += $(foreach dir, $(SRC_DIR), $(addprefix -I, $(dir)))

#  Specify pre-processor options
#CFLAGS  += -Dsketch_tracks_and_roads #  Plot outline of roads/tracks
#CFLAGS  += -Dgrid_lines #          Outline tiles
#CFLAGS  += -Dfull_topog #          Plot all topography
CFLAGS  += -Dkb_dev #              Keyboard with modified 'flying' keys
#CFLAGS  += -Dgeo_coord #           Use x=east, y=north, z=up for shapes
#CFLAGS  += -D_Display_Shapes #
#CFLAGS  += -D_Display_Wagons  #
#CFLAGS  += -D_Display_Textures #

#  Specify libraries
LIBGL   = -lglut -lGLU -lGL
#LIBGL  += -lportaudio #                Sound library
LIBX    = -lXmu -lXext -lX11 -lXi -lz -lm
LDFLAGS = $(LIBGL) $(LIBX)

#  Generate list of  prerequisits
HFILES := $(shell find $(INC_DIR) -name "*.h")
CFILES := $(shell find $(SRC_DIR) -name "*.c")

# Which suffixes to process
.SUFFIXES:
.SUFFIXES: .c .h .o

#  Targets that are not the name of a file
.PHONEY: clean cleaner depend echo

#  Makefile targets:

zr: zr.c $(HFILES) $(CFILES) makefile
	$(CC) zr.c -o $@ $(CFLAGS) $(LDFLAGS)

echo:
	@echo $(HFILES)
	@echo $(CFILES)

clean:
	rm -f *.o *~

cleaner:
	rm -f zr *.o *~

