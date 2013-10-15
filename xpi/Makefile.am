ACLOCAL_AMFLAGS         = -I m4

SUBDIRS = src

export XPI_ID = belgiumeid@eid.belgium.be
export mozext
export TMPDIR ?= /tmp

xpipackage:
	$(MAKE) -C src xpipackage
