ACLOCAL_AMFLAGS	= -I m4
lib_LTLIBRARIES	= libeti.la

if LINUX
#
libeti_la_LDFLAGS	= -avoid-version -ldl -pthread
libeti_la_SOURCES	= src/controlfileaccess.cpp src/logger.cpp src/mutex.cpp src/util.cpp src/virtualcardaccess.cpp src/xml.cpp
libeti_la_CPPFLAGS 	= -Wall -O3 -Wno-deprecated-declarations
#
endif

#if WIN32
#
#AM_LDFLAGS		= -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version -static
#AM_CXXFLAGS	= -municode -no-undefined -DMINGW_HAS_SECURE_API
#
#endif



