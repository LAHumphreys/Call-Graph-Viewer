MAKE_DIRS= DEV_TOOLS CoreLibs GCGVLibs Tests Binaries

# makefile tools library  for bulding multi-directory projects
#
# Luke Humphreys 2012
ALL_WARNINGS=YES

MODE=CPP

#
# We do the sym_link job before anything else to make sure libCef is available
# to link against!
#
dummy: makefile.include all

makefile.include: 
	$(MAKE) -C DEV_TOOLS/Config/Makefiles/

include makefile.include
