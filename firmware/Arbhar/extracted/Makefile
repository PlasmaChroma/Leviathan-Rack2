# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = arbhar.expernals

# input source file (class name == source file basename)
#class.sources = fftwObject~.c 
#class.sources = arbhar_shmem.c
#class.sources = midi_reader.c
#class.sources = midi_fifo.c
#class.sources = raw_play~.c
# class.sources = arbhar_gpio~.c
# class.sources = arbhar_play~.c
class.sources = arbhar_rec~.c
#class.sources = main_out_stereo~.c
#class.sources = arbhar_wtosc~.c
#class.sources = layer_control.c
#class.sources = arbhar_ShMemRec~.c
#class.sources = shmem.c
#class.sources = shell.c
#class.sources = terminal.c
#class.sources = mcp3208_ch.c
#class.sources = pitchShift~.c
#class.sources = arbhar_load.c
#class.sources = arbhar_control.c
#class.sources = arbhar_adcval.c
#class.sources = vs_delay~.c
#class.sources = shifter~.c
#class.sources = scanFollow~.c
#class.sources = process_mcp_ch~.c
#class.sources = stereo_thru~.c
#class.sources = stereo_in~.c
#class.sources = neamh~.c
#class.sources = tabosc4c~.c
#class.sources = tanh_approx~.c
#class.sources = lopass~.c

#enable on raspberry!!
ldlibs = -lbcm2835
#ldlibs = -lfftw3f -lm

# all extra files to be included in binary distribution of the library
datafiles =

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=/src/pd-lib-builder/
#PDLIBBUILDER_DIR=../pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
