XPI_NAME=belgiumeid.xpi
EXTRA_DIST = belgiumeid-1.0.18.1-signed.1-signed.xpi eid_belgie-1.0.21-fx.xpi eid_belgie-1.0.22.xpi
if SIGNED
extensionbase_DATA = $(XPI_ID).xpi
endif

$(XPI_ID).xpi: eid_belgie-1.0.22.xpi
	cp $< $(XPI_ID).xpi

extensionbasedir=$(mozext)/$(XPI_UUID)

DISTCLEANFILES = $(XPI_ID).xpi
