SOURCES=$(shell echo *.cpp)

LINKED_LIBS= libGCGVApp libGCGVCallbacks libCEF libGTK libExec libCallgrindData libProfData libExec libSource\
             libJSON \
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 

EXTERNAL_LIBS=/usr/lib64/libboost_regex.a -static-libgcc -static-libasan -static-libstdc++

EXECUTABLE=gcgv
MODE=CPP
CPP_TAGS_FILE=profiler-gprofile-c++.tags
USE_CEF=YES
USE_GTK=YES
USE_JSON=YES
STATIC=NO
USE_FPIC=NO



include ../../makefile.include
