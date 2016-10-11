ACLOCAL_AMFLAGS         = -I m4

SUBDIRS = src signed-build

export XPI_ID = belgiumeid@eid.belgium.be
export mozext
export TMPDIR ?= /tmp

if SIGNED
xpipackage:
	$(MAKE) -C signed-build xpipackage
else
xpipackage:
	$(MAKE) -C src xpipackage
endif
