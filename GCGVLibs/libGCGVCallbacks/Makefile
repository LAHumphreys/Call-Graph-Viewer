SOURCES=$(shell echo *.cpp)
EXPORT_INCLUDES=$(shell echo *.h)

LINKED_LIBS= libGCGVJS libExec libCallgrindData libProfData libExec libSource\
			 libWebSockets \
             libCommands \
             libJSON \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 

TARGET_LIB=libGCGVCallbacks

MODE=CPP
TAGS_FILE=profiler-gcgv_callbacks-c++.tags
USE_JSON=YES



include ../makefile.gcgv.include
