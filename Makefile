MAKE_DIRS= DEV_TOOLS CoreLibs GCGVLibs Tests Binaries

# makefile tools library  for bulding multi-directory projects
#
# Luke Humphreys 2012
ALL_WARNINGS=YES
LINK_CEF_RESOURCES=YES

MODE=CPP

dummy: makefile.include all

makefile.include: 
	$(MAKE) -C DEV_TOOLS/Config/Makefiles/

include makefile.include
