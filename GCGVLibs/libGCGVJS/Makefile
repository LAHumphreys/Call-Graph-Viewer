SOURCES=$(shell echo *.cpp)
EXPORT_INCLUDES=$(shell echo *.h)

LINKED_LIBS= libCEF libExec libCallgrindData libProfData libExec libSource\
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 

TARGET_LIB=libGCGVJS

MODE=CPP
CPP_TAGS_FILE=profiler-js-c++.tags
USE_CEF=YES
USE_GTK=YES



include ../makefile.gcgv.include
