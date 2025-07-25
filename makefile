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
#   This version of the makefile is designed for use with Linux
#
#   For valgrind and gdb compile with "-g -Og", then run:
#   valgrind -v -v --leak-check=yes --track-origins=yes -s --main-stacksize=200000000 ./zr -p
#   gdb ./zr
#   (gdb) run
################################################################################
#
#SHELL      = /bin/sh

# Define interface: SDL2 or (default) GLUT
# This does not work if # follows SDL2
#IFACE=SDL2
SOUND=OPENAL
#SOUND=

#  Specify include and source directories relative to this directory
INC_DIR   := include
SRC_DIR   := src/core src/graphics src/input src/dynamics src/layout src/setup

#  Specify Compiler and Compiler Options
CC       = gcc
LD       = gcc
CFLAGS   = -m64 -march=native -mcmodel=medium
#CFLAGS  += -O3 #         Level 3 compiler optimisation
CFLAGS  += -g -Og #    For valgrind use -g and -Og optimisation
CFLAGS  += -Wall -pedantic
CFLAGS  += -Wextra
CFLAGS  += -Wno-unused-variable
CFLAGS  += -I/usr/include/freetype2
CFLAGS  += $(foreach dir, $(INC_DIR), $(addprefix -I, $(dir)))
CFLAGS  += $(foreach dir, $(SRC_DIR), $(addprefix -I, $(dir)))

#  Specify pre-processor options
#CFLAGS += -DBasic_Test # Set default test values for route, consists etc.
#CFLAGS  += -Dsketch_tracks_and_roads #  Plot outline of roads/tracks
#CFLAGS  += -Dgrid_lines #          Outline tiles
#CFLAGS  += -Dfull_topog #          Plot all topography
#CFLAGS  += -Dgeo_coord #           Use x=east, y=north, z=up for shapes
#CFLAGS  += -D_Display_Shapes #
#CFLAGS  += -D_Display_Wagons  #
#CFLAGS  += -D_Display_Textures #
#CFLAGS  += -Dno_vertex_arrays
#CFLAGS  += -Dtexture_short
#CFLAGS  += -Dnormal_byte
#CFLAGS += -Dculling_on_for_wagons  #  Some wagons need back face culling off
#CFLAGS += -D_NoMultiSample #  MultiSampling turned off
#CFLAGS += -Dzr_freetype #     -Dfreetype would rename directory
#CFLAGS += -DMinGW #           Use system_alt.c routines
#CFLAGS += -DOPENAL #          Setup OpenAL system (in development)
#CFLAGS += -DROUTE_USA1 #      Northeast Corridor
#CFLAGS += -DROUTE_USA2 #      Marias Pass
#CFLAGS += -DROUTE_EUROPE1 #   Settle line
#CFLAGS += -DROUTE_EUROPE2 #   Innsbruck - St Anton
#CFLAGS += -DROUTE_JAPAN1 #    Tokyo - Hakone
#CFLAGS += -DROUTE_JAPAN2 #    Hisatsu Line             Segmentation
#CFLAGS += -DROUTE_TUTORIAL
#CFLAGS += -DROUTE_AU_NSW_SW_SS
#CFLAGS += -DROUTE_NEW_FOREST
CFLAGS += -DNO_ROUTE_DEF
#CFLAGS += -DSHADERS
#CFLAGS += -DDISPLAY_5_SECONDS # Used for remote tests over a slow network
#
#   Option to circumvent linker error with OpenAL functions.  See:
# https://maskray.me/blog/2021-01-09-copy-relocations-canonical-plt-entries-and-protected

CFLAGS += -fpie -fopenmp

#  Movie Options  FFMPEG or LIBPNG or PPM
#MOVIE = FFMPEG
#

#  Conditionals

ifeq ($(IFACE),SDL2)
  CFLAGS  += -DSDL2 -DMinGW
  LIBGL   =  -lSDL2_ttf -lSDL2
else
  LIBGL   =  -lglut
endif
ifeq ($(SOUND),OPENAL)
  CFLAGS  += -DOPENAL
#  LIBGL   += -lopenal -lalut
  LIBGL   += -lopenal
endif
ifeq ($(MOVIE),FFMPEG)
  CFLAGS += -DFFMPEG
  LIBGL += -lavcodec -lavutil -lswscale
endif
ifeq ($(MOVIE),LIBPNG)
  CFLAGS += -DLIBPNG
  LIBGL += -lpng
endif

LIBGL   += -lfreetype

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
.PHONEY: clean cleaner paths echo

#  Makefile targets:

zr: zr.c $(HFILES) $(CFILES) makefile load_sigscr_file.o y.tab.o lex.yy.o
	$(CC) zr.c -c $(CFLAGS)
	$(LD) zr.o load_sigscr_file.o y.tab.o lex.yy.o -o $@ $(CFLAGS) $(LDFLAGS)

load_sigscr_file.o: src/input/load_sigscr_file.c makefile
	$(CC) src/input/load_sigscr_file.c -c $(CFLAGS)

y.tab.o: src/input/y.tab.c makefile
	$(CC) src/input/y.tab.c  -c $(CFLAGS)

lex.yy.o: src/input/lex.yy.c makefile
	$(CC) src/input/lex.yy.c -c $(CFLAGS)

#.c.o:   makefile
#	$(CC) -c $*.c $(CFLAGS)

paths:
	$(CC) zr.c -o $@ $(CFLAGS) $(LDFLAGS) -E -v

echo:
	@echo $(HFILES)
	@echo $(CFILES)
	@echo ::$(IFACE)::
	@echo $(CFLAGS)
	@echo $(LIBGL)

clean:
	rm -f *.o *~

cleaner:
	rm -f zr *.o *~

