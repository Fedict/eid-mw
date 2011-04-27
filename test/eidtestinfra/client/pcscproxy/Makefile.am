ACLOCAL_AMFLAGS         	= -I m4
lib_LTLIBRARIES				= libpcscproxy.la

if LINUX
#
libpcscproxy_la_SOURCES		= src/apdurequest.cpp src/carddata.cpp src/control.cpp src/filestructure.cpp src/hexutils.cpp src/proxy_pcsclite.cpp src/softcard.cpp src/softreader.cpp src/softreadermanager.cpp src/transaction.cpp src/winscardimpl.cpp
libpcscproxy_la_CPPFLAGS 	= -I$(srcdir)/../etilib/src -Wall -O3 -Wno-deprecated-declarations
libpcscproxy_la_LDFLAGS		= -avoid-version -ldl -pthread 
libpcscproxy_la_LIBADD		= ../etilib/libeti.la
#
endif

#if WIN32
#AM_LDFLAGS					= -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version -static
#AM_CXXFLAGS				= -municode -no-undefined -DMINGW_HAS_SECURE_API
#endif

#ifdef OSX
#LFLAGS = -fPIC -headerpad_max_install_names -single_module -dynamiclib -compatibility_version 1.0.0
#
#endif
