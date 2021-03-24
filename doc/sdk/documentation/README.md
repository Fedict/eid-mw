# About

The files in this directory document how to use the Belgian eID software
to develop your own application.

They consist of two sets of documents: the PKCS#11 documentation (taken,
unmodified, from [the OASIS
website](https://docs.oasis-open.org/pkcs11/)), and the files specific to
the Belgian eID Card.

The files are:

* [pkcs11-ug-v2.40.pdf](pkcs11-ug-v2.40.pdf): The PKCS#11 "usage guide",
  which introduces the concepts and methods in PKCS#11. Start here.
* [pkcs11-base-v2.40.pdf](pkcs11-base-v2.40.pdf): The PKCS#11 base
  standard.
* [pkcs11-curr-v2.40.pdf](pkcs11-curr-v2.40.pdf): Currently-relevant
  PKCS#11 mechanisms. Note that the Belgian eID software does not
  implement all mechanisms in this document; only those that are
  relevant for, and supported by, the Belgian eID.
* [pkcs11-hist-v2.40.pdf](pkcs11-hist-v2.40.pdf): Historic PKCS#11
  mechanisms. If any mechanisms in this document are supported by the
  Belgian eID middleware, their usage is not recommended for new
  applications; you should prefer mechanisms in the "curr" document
  instead.
* [pkcs11-profiles-v2.40.pdf](pkcs11-profiles-v2.40.pdf): for
  completeness, although you should not need this document to develop
  your own application; contains a list of "profiles" to be supported by
  certain PKCS#11 implementations.
* [beidsdk\_card\_data.pdf](beidsdk_card_data.pdf): Enumerates the
  object and attribute names for all Belgian eID data elements that can
  be retrieved from the card. There is also an
  [odt](beidsdk_card_data.odt) version of this document.
* The documents in the [Applet 1.7 eID
  Cards](Applet%201.7%20eID%20Cards/) folder contain documentation on
  the physical way in which data is encoded on Belgian eID cards with
  Applet 1.7 and RSA keys. These cards are issued between 2014 and 2021.
* The documents in the [Applet 1.8 eID
  Cards](Applet%201.8%20eID%20Cards/) folder contain documentation on
  the physical way in which data is encoded on Belgian eID cards with
  Applet 1.8 and ECDSA keys. As of the time of writing, no cards with
  this applet have been issued yet; they are expected to arrive in 2021.
