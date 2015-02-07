SOURCES=$(shell echo *.cpp)

LINKED_LIBS= libExec libCallgrindData libProfData libExec libSource\
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 
EXECUTABLE=gcgv
MODE=CPP
CPP_TAGS_FILE=profiler-gprofile-c++.tags
EXTERNAL_LIBS=-lncurses
USE_CEF=YES
USE_GTK=YES



include ../../makefile.include
