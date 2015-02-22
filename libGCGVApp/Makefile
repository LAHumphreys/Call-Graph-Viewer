SOURCES=$(shell echo *.cpp)
EXPORT_INCLUDES=$(shell echo *.h)

LINKED_LIBS= libGCGVCallbacks libCEF libExec libCallgrindData libProfData libExec libSource\
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 
TARGET_LIB=libGCGVApp
MODE=CPP
CPP_TAGS_FILE=profiler-gprofile-c++.tags
USE_CEF=YES
USE_GTK=YES



include ../makefile.include
