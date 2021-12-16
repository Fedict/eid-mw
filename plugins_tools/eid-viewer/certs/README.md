# About this directory

The files in this directory are the currently active root certificates
for the Belgian eID PKI.

The .0 and .1 files are symlinks to the hashed versions of these
certificates, as required by OpenSSL for the
[`X509_LOOKUP_add_dir()`](https://www.openssl.org/docs/manmaster/man3/X509_LOOKUP_add_dir.html)
call.

This directory is used as the trust root by the Linux and macOS eID
viewer. The Windows eID viewer uses a different method for validating
cards, and does not use these files.

# Note on test cards

The eID viewer does not ship with test card roots. This means that if
you insert a test card into a card reader, start the eID viewer, and
choose the "validate card" button, that validation will fail.

Given that these are not legal eID cards, this is expected; you don't
want anyone to be able to trick you into believing you have a valid eID
card when in reality the card is a specimen one. However, that does not
mean it is not possible to make the eID viewer validate test cards; it
just means you have a bit of configuration to do.

To allow the Linux and macOS viewers to validate test cards as valid,
two steps are necessary: first you need to copy the test card's root
certificate into this trust directory and rehash the directory; and
second, you need to disable the allowlist that limits the OCSP server
URLs that may appear on certificates.

## Installing the root certificate

To install the root certificate, first copy it off the card. You can do
that using the eID viewer: start the viewer, insert the card, go to the
"certificates" tab, right click on the root certificate, and choose the
"Export to PEM file..." menu option; then choose a file name in this
directory with the ".pem" extension.

Next, you need to rehash the directory, which can be done with the
`openssl rehash /path/to/dir` command. Please note that the version of
OpenSSL which is shipped with macOS does not come with a working
`rehash` command; you will have to install a different version of
OpenSSL separately (e.g., through homebrew).

On macOS, it is possible to install the root certificate directly into
the copy of this directory in the eID Viewer application bundle, which
can be found under `<eID Viewer>/Contents/Resources/certs`. On Linux,
the the viewer looks at `${pkgdatadir}/trustdir` (e.g.,
`/usr/share/eid-mw/trustdir`), and installing the root certificate
there is equally possible. Alternatively, one can also install the root
certificate to this directory in the source, and then rebuild & install
the viewer.

## Disabling the allowlist

The viewer will refuse to validate any eID certificate that has an OCSP
URL which does not match the ones found on production cards in the
field. In order to disable that allowlist, hidden configuration
settings need to be modified. Note that when doing so, the eID Viewer
will *always* log a message that this configuration has been modified.

How to do so is operating-system specific:

### Linux

- Start the `dconf-editor` program, installing it first if needed.
- Go to the be.belgium.eid.eid-viewer section in dconf-editor
- Switch off the `check-urls` option (i.e., ensure it is set to
  `false`, switching off the `Use default` option if needed).
  
### macOS

- Close the eID Viewer
- Create a file called `<eID Viewer>/Contents/Resources/extraopts.plist`
- Make sure it has the following contents:
        
        <?xml version="1.0" encoding="UTF-8"?>
        <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
        <plist version="1.0">
        <dict>
                <key>disableAllowlist</key>
                <true/>
        </dict>
        </plist>

- Start the viewer again; the allowlist should be disabled.
