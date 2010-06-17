AM_CXXFLAGS								= -DLTC_NO_ASM

bin_PROGRAMS = certprop

certprop_SOURCES = src/main.cpp src/certprop.cpp
certprop_LDFLAGS = -lwinscard -lcrypt32 -lcardlayer -lcommon -ladvapi32 -no-undefined
certprop_LDADD = ../common/libcommon.la ../cardlayer/libcardlayer.la
