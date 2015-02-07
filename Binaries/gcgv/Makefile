#
# Naive makefile to build a trivial CEF example app
#
# (C) Luke Humphreys 2014
#
 
ifndef CEF_DIR
    $(error "CEF_DIR must be set to the root location of your 'chromium embded framework' build")
endif

#
# Configure the GTK Library
#
GTK_INCLUDE_FLAGS=$(shell pkg-config --cflags gtk+-2.0) 
GTK_LIBS=$(shell pkg-config --libs-only-l gmodule-2.0 gtk+-2.0 gthread-2.0 gtk+-unix-print-2.0)

#
# Configure the CEF Library
#
CEF_LIBS=$(CEF_DIR)/out/Debug
CEF_RESOURCES=$(CEF_DIR)/Resources

all: viewer

#
# Build the testApp, but make sure requirements are linked in first
#
viewer: 
	g++ -std=c++11 -Werror -Wall -o viewer $(GTK_INCLUDE_FLAGS) -I$(CEF_DIR) *.cpp -L$(CEF_LIBS) $(CEF_LIBS)/obj.target/libcef_dll_wrapper.a -lcef $(GTK_LIBS) -ludev -lX11


#
# Link in the dev-sandbox
#
chrome-sandbox:
	ln -s $(CEF_LIBS)/chrome-sandbox

#
# Dummy target for linked in resources
#
.LINK_RESOURCES: locales cef_100_percent.pak cef_200_percent.pak cef.pak devtools_resources.pak icudtl.dat

#
# testApp will fail unless it has access to the relevant locale data
#
locales:
	ln -s $(CEF_RESOURCES)/locales

#
# Link in other resources used by the app
#
%.pak:
	ln -s $(CEF_RESOURCES)/$@
%.dat:
	ln -s $(CEF_RESOURCES)/$@

#
# Remove the binary, and all linked in resources
#
clean:
	rm -f devtools_resources.pak
	rm -f cef_100_percent.pak
	rm -f cef_200_percent.pak
	rm -f cef.pak
	rm -f icudtl.dat
	rm -f locales
	rm -f testApp
	rm -f chrome-sandbox
