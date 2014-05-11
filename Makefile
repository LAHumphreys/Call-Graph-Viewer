MAKE_DIRS= DEV_TOOLS libProfData libCallgrindData libExec Tests Binaries

# makefile tools library  for bulding multi-directory projects
#
# Luke Humphreys 2012
ALL_WARNINGS=YES

MODE=CPP

dummy: makefile.include all

makefile.include: 
	$(MAKE) -C DEV_TOOLS/Config/Makefiles/

include makefile.include
