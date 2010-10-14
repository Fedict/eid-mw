ACLOCAL_AMFLAGS   = -I m4

bin_PROGRAMS      = certprop

certprop_CPPFLAGS = -I$(srcdir)/../common/src
certprop_SOURCES  = src/main.cpp src/certprop.cpp
certprop_LDFLAGS  = -lcrypt32 -ladvapi32 -lwinscard -no-undefined -L.
certprop_LDADD    = ../common/libbeidcommon.la ../cardlayer/libbeidcardlayer.la
certprop_CXXFLAGS = -DLTC_NO_ASM -DUNICODE
