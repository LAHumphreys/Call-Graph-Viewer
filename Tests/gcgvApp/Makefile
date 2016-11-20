LINKED_LIBS= libSource libExec \
			 libGCGVCallbacks \
			 libThreadComms \
			 libWebSockets \
			 libJSON \
			 libCommands \
			 libMeta \
			 libScreen \
			 libCallgrindData \
			 libProfData\
			 libUtils  \
			 libString \
			 libIOInterface \
			 libTest

CPP_TAGS_FILE=cgv_tests_cef-c++.tags

BUILD_TIME_TESTS=reqGraph fileReq reqLoad details flat navigate reqFind reqGraph
MODE=CPP

USE_JSON=YES
USE_WEB_SOCKET_CPP=YES
SUPPRESSED_WARNINGS+="unused-function"
BOOST_MODULES+=regex

include ../../makefile.include
