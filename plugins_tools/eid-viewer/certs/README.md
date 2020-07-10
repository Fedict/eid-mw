The files in this directory are the currently active root certificates
for the Belgian eID PKI.

The .0 and .1 files are symlinks to the hashed versions of these
certificates, as required by OpenSSL for the
[`X509_LOOKUP_add_dir()`](https://www.openssl.org/docs/manmaster/man3/X509_LOOKUP_add_dir.html)
call.
