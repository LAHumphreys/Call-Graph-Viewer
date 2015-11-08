SOURCES=$(shell echo *.cpp)

LINKED_LIBS= libGCGVCallbacks libWebSockets libExec libCallgrindData libProfData libExec libSource\
             libJSON \
             libCommands \
			 libMeta  \
			 libString \
             libScreen \
             libUtils  \
			 libIOInterface 

EXECUTABLE=gcgv
MODE=CPP
CPP_TAGS_FILE=profiler-gprofile-c++.tags
USE_JSON=YES
USE_WEB_SOCKET_CPP=YES
STATIC=NO
USE_FPIC=NO
BOOST_MODULES+=regex



include ../../makefile.include
