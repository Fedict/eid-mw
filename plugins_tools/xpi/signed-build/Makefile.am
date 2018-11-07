XPI_NAME=belgiumeid.xpi
EXTRA_DIST = eid_belgie-1.0.23.xpi belgium_eid-1.0.29.xpi
if SIGNED
extensionbase_DATA = $(XPI_ID).xpi
endif

if WEBEXT
extension_filename=belgium_eid-1.0.29.xpi
else
extension_filename=eid_belgie-1.0.23.xpi
endif

$(XPI_ID).xpi: $(extension_filename)
	cp $< $(XPI_ID).xpi

extensionbasedir=$(mozext)/$(XPI_UUID)

DISTCLEANFILES = $(XPI_ID).xpi
