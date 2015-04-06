LINKED_LIBS= libSource libExec \
			 libGCGVCallbacks \
			 libGCGVApp \
			 libCEF \
			 libGTK \
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

BUILD_TIME_TESTS=gcgv_jsTests 
MODE=CPP

USE_CEF=YES
USE_GTK=YES
USE_JSON=YES

TEST_LINK_CEF_RESOURCES=YES

#
# CEF does not seem to support valgrind (even the sample apps explode with too
# many errors).
# 
# Until we find a better solution, supress valgrind on this folder, so we can
# still memory test the rest of our code
NO_MEMTEST=YES

NO_TRAVIS=YES

include ../../makefile.include
