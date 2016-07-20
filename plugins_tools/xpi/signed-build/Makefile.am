XPI_NAME=belgiumeid.xpi
EXTRA_DIST = belgiumeid-1.0.18.1-signed.1-signed.xpi eid_belgie-1.0.21-fx.xpi
extensionbase_DATA = $(XPI_ID).xpi $(XPI_ID)-ff47p.xpi

$(XPI_ID).xpi: belgiumeid-1.0.18.1-signed.1-signed.xpi
	cp $(srcdir)/belgiumeid-1.0.18.1-signed.1-signed.xpi $(XPI_ID).xpi

$(XPI_ID)-ff47p.xpi: eid_belgie-1.0.21-fx.xpi
	cp $(srcdir)/$< $(XPI_ID)-ff47p.xpi

extensionbasedir=$(mozext)/$(XPI_UUID)
