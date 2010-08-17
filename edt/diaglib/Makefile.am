ACLOCAL_AMFLAGS         = -I m4
COMMON_LIBADD   			= ../common/libbeidcommon.la ../cardlayer/libbeidcardlayer.la
COMMON_CPPFLAGS  			= -I$(srcdir)/../../common/src -I$(srcdir)/../../cardlayer/src 

#################################################################################################################################################################################

if LINUX
# no eDT for Linux (yet?)
endif

#################################################################################################################################################################################

if OSX
#
lib_LTLIBRARIES      = libdiag.la
libdiag_la_SOURCES   = src/beidlib.cpp src/card.cpp src/device.cpp src/file.cpp src/hardware.cpp src/log.cpp src/md5.cpp src/md5sum.cpp src/metarule.cpp src/metaruleverdict.cpp src/middleware.cpp src/module.cpp src/ocsp.cpp src/pcsc.cpp src/pkcs.cpp src/process.cpp src/progress.cpp src/reader.cpp src/report.cpp src/repository.cpp src/services.cpp src/software.cpp src/system.cpp src/winregkey.cpp src/win/win_csp.cpp src/win/win_device.cpp src/win/win_file.cpp src/win/win_folder.cpp src/win/win_middleware.cpp src/win/win_process.cpp src/win/win_registry.cpp src/win/win_services.cpp src/win/win_software.cpp src/win/win_system.cpp src/win/win_tokend.cpp src/win/win_user.cpp ../secure_helper.cpp ../util.cpp
libdiag_la_CPPFLAGS  = $(COMMON_CPPFLAGS) -I/System/Library/Frameworks/PCSC.framework/Headers
libdiag_la_CXXFLAGS  = -framework PCSC
libdiag_la_LIBADD    = $(COMMON_LIBADD)
#
endif

###########################################################################################################################3

if WIN32
#
lib_LTLIBRARIES     = libdiag.la
libdiag_la_SOURCES   = src/beidlib.cpp src/card.cpp src/device.cpp src/file.cpp src/hardware.cpp src/log.cpp src/md5.cpp src/md5sum.cpp src/metarule.cpp src/metaruleverdict.cpp src/middleware.cpp src/module.cpp src/ocsp.cpp src/pcsc.cpp src/pkcs.cpp src/process.cpp src/progress.cpp src/reader.cpp src/report.cpp src/repository.cpp src/services.cpp src/software.cpp src/system.cpp src/winregkey.cpp src/win/win_csp.cpp src/win/win_device.cpp src/win/win_file.cpp src/win/win_folder.cpp src/win/win_middleware.cpp src/win/win_process.cpp src/win/win_registry.cpp src/win/win_services.cpp src/win/win_software.cpp src/win/win_system.cpp src/win/win_tokend.cpp src/win/win_user.cpp ../secure_helper.cpp ../util.cpp
libdiag_la_CPPFLAGS = $(COMMON_CPPFLAGS)
libdiag_la_LDFLAGS	= -lsetupapi -lcrypt32 -lpsapi -lversion -lwinscard -no-undefined -static-libstdc++ -static-libgcc -avoid-version
libdiag_la_CXXFLAGS = $(COMMON_CXXFLAGS) -municode -DMINGW_HAS_SECURE_API
libdiag_la_LIBADD   = $(COMMON_LIBADD)
#
endif
