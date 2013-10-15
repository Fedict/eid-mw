ACLOCAL_AMFLAGS     = -I m4
COMMON_SOURCES 		= src/mw_util.cpp src/libtomcrypt/md5.c src/libtomcrypt/rmd160.c src/libtomcrypt/sha1.c src/libtomcrypt/sha256.c src/libtomcrypt/sha384.c src/libtomcrypt/sha512.c src/socket/socketclient.cpp src/socket/socket.cpp src/socket/socketserver.cpp src/bytearray.cpp src/bytearrayreader.cpp src/configcommon.cpp src/datafile.cpp src/dynamiclib.cpp src/hash.cpp src/logbase.cpp src/log.cpp src/mutex.cpp src/mwexception.cpp src/thread.cpp src/tlvbuffer.cpp src/tlv.cpp src/util.cpp 
noinst_HEADERS		= src/beidversions.h src/bytearrayreader.h src/thread.h src/prefix.h src/socket/socketserver.h src/socket/socketclient.h src/socket/socket.h src/bytearray.h src/mutex.h src/dynamiclib.h src/tlv.h src/export.h src/tlvbuffer.h src/hash.h src/configuration.h src/util.h src/mwexception.h src/datafile.h src/mw_util.h src/log.h src/svn_revision.h src/tlvtagdefinitions.h src/eiderrors.h src/libtomcrypt/tomcrypt_hash.h src/libtomcrypt/tomcrypt_cfg.h src/libtomcrypt/tomcrypt_argchk.h src/libtomcrypt/tomcrypt_macros.h src/threaddefines.h src/logbase.h config.h
AM_CPPFLAGS 		= -DLTC_NO_ASM
lib_LTLIBRARIES		= libbeidcommon.la
libbeidcommon_la_SOURCES  	= $(COMMON_SOURCES) src/configuration.cpp
libbeidcommon_la_CXXFLAGS   = -DUSING_DL_OPEN $(COMMON_CXXFLAGS)
