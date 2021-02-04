README.MAKEFILE
===============

The makefile has a slightly unusual structure but this has been chosen:

  1.  To work with the multi-level source directory structure
      used by openrail.
  2.  To use only a single level of makefile
  3.  To remove the requirement for dependency files
  4.  Because at present, compilation of the whole program is fast

The main program 'zr.c' in the top level directory is the only C file in the makefile line calling the C compiler.  'zr.c' then loads all the include files (*.h) and other C files (*.c) through include statements.

New include (*.h) files should be placed under directory 'include' and their names should be added to the beginning of the file 'include/zr.h'.

New C (*.c) files should be added to one of the directories under 'src'.  Then statements to include the new files should be added at the end of zr.c.

If more sub-directories are added under the 'include' or 'src' directories, these should be added to the  INC_DIR or SRC_DIR lines in the makefile.

As the program grows and compilation times increase, there may come a time for a more traditional makefile in which each C file is compiled separately.

The makefile works correctly with the gmake (GNU) version of make.
