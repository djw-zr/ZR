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
#   For valgrind compile with "-g -Og"
#   valgrind -v -v --leak-check=yes --track-origins=yes ./zr -p
#
################################################################################
#
SHELL      = /bin/sh

# Define interface: SDL2 or (default) GLUT
#IFACE = SDL2  # Optimisation -O1, -O2 and -O3 occasional cause freeze at shutdown.

#  Specify include and source directories relative to this directory
INC_DIR   := include
SRC_DIR   := src/core src/graphics src/input src/dynamics src/layout

#  Specify Compiler and Compiler Options
CC       = gcc
CFLAGS   = -m64 -march=native -Iinclude -O3
#CFLAGS   = -O3 #               Level 3 compiler optimisation
#CFLAGS  += -g3 -Og #    For valgrind use -g and -Og optimisation
CFLAGS  += -Wall -pedantic
CFLAGS  += -Wno-unused-variable
CFLAGS  += -I/usr/include/freetype2
CFLAGS  += $(foreach dir, $(INC_DIR), $(addprefix -I, $(dir)))
CFLAGS  += $(foreach dir, $(SRC_DIR), $(addprefix -I, $(dir)))

#  Specify pre-processor options
#CFLAGS  += -Dsketch_tracks_and_roads #  Plot outline of roads/tracks
CFLAGS  += -Dgrid_lines #          Outline tiles
#CFLAGS  += -Dfull_topog #          Plot all topography
CFLAGS  += -Dkb_dev #              Keyboard with modified 'flying' keys
#CFLAGS  += -Dgeo_coord #           Use x=east, y=north, z=up for shapes
#CFLAGS  += -D_Display_Shapes #
#CFLAGS  += -D_Display_Wagons  #
#CFLAGS  += -D_Display_Textures #
CFLAGS  += -Duse_vertex_arrays
#CFLAGS  += -Dtexture_short
#CFLAGS  += -Dnormal_byte
CFLAGS += -Dculling_off_for_wagons
CFLAGS += -Dzr_freetype  #  -Dfreetype would rename directory
#CFLAGS += -DMinGW        #   Use system_alt.c routines

#  Conditionals
ifeq ($(IFACE),SDL2)
  CFLAGS  += -DSDL2
#  LIBGL   =  -lSDL2_ttf -lSDL2main -lSDL2
  LIBGL   =  -lSDL2_ttf -lSDL2
else
  LIBGL   =  -lglut -lfreetype
endif

#  Specify libraries
LIBGL   += -lGLU -lGL
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

