ACLOCAL_AMFLAGS         		= -I m4
AM_CPPFLAGS       				= -I../../pkcs11/src/include/rsaref220/

bin_PROGRAMS   					= openclosesession finalizeinitialize sign

openclosesession_SOURCES		= src/open-close-session.c
finalizeinitialize_SOURCES		= src/finalize-initialize.c
sign_SOURCES					= src/sign.c

if LINUX
#
AM_LDFLAGS						= -avoid-version -ldl -pthread
#
endif

if OSX
#
AM_LDFLAGS						= -avoid-version -ldl -pthread -framework PCSC
AM_CXXFLAGS    					= -framework PCSC
#
endif

if WIN32
#
AM_LDFLAGS						= -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version -static
AM_CXXFLAGS						= -municode -no-undefined -DMINGW_HAS_SECURE_API
#
endif
