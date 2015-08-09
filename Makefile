MAKE_DIRS= DEV_TOOLS CoreLibs GCGVLibs Tests Binaries

# makefile tools library  for bulding multi-directory projects
#
# Luke Humphreys 2012
ALL_WARNINGS=YES
LINK_CEF_RESOURCES=YES

MODE=CPP

#
# We do the sym_link job before anything else to make sure libCef is available
# to link against!
#
dummy: makefile.include sym_link all

makefile.include: 
	$(MAKE) -C DEV_TOOLS/Config/Makefiles/


release: 
	rm -f libs.tar.gz
	rm -f libs.tar.gz.bz2
	cp gcgv libs/
	cp /lib64/libc.so.6  libs/
	cp /usr/lib/gcc/x86_64-pc-linux-gnu/4.9.2/libgcc_s.so.1 libs/
	cp /usr/lib/gcc/x86_64-pc-linux-gnu/4.9.2/libstdc++.so libs/
	cp /lib64/ld-linux-x86-64.so.2 libs/
	cp /usr/lib64/libboost_regex.a libs/

	tar -cvf libs.tar libs
	gzip libs.tar
	bzip2 libs.tar.gz
	scp libs.tar.gz.bz2 root@46.101.22.25:/var/www/html/Nostalgia

include makefile.include
