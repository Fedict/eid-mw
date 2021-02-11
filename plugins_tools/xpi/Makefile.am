ACLOCAL_AMFLAGS         = -I m4

SUBDIRS = src signed-build

export XPI_ID = belgiumeid@eid.belgium.be
export mozext = @mozext@
export TMPDIR ?= /tmp

if SIGNED
xpipackage:
	echo "E: make xpipackage doesn't work with the prebuilt signed package"
	exit 1
else
xpipackage:
	$(MAKE) -C src xpipackage
endif
