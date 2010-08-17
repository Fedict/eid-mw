ACLOCAL_AMFLAGS         = -I m4

lib_LTLIBRARIES           = libdiagengine.la
libdiagengine_la_SOURCES  = analysis.cpp  diagengine.cpp  hpprotecttoolsrule.cpp  middlewarefilesintegrityrule.cpp  winscarddlllocksrule.cpp ../diaglib/util.cpp
libdiagengine_la_CPPFLAGS = -I$(srcdir)/.. -I$(srcdir)/../../../eidmw/eidlib -I$(srcdir)/../../../eidmw/common -I$(srcdir)/../diaglib
libdiagengine_la_LIBADD   = ../diaglib/libdiag.la
libdiagengine_la_LDFLAGS  =  -L/opt/i686-w64-mingw32/lib -lmsvcr80
libdiagengine_la_CXXFLAGS = -municode

