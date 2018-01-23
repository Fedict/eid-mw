XPI_NAME=belgiumeid.xpi
EXTRA_DIST = eid_belgie-1.0.23.xpi belgium_eid-1.0.27.xpi
if SIGNED
extensionbase_DATA = $(XPI_ID).xpi
endif

$(XPI_ID).xpi: eid_belgie-1.0.23.xpi
	cp $< $(XPI_ID).xpi

extensionbasedir=$(mozext)/$(XPI_UUID)

DISTCLEANFILES = $(XPI_ID).xpi
