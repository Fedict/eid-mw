# BeID middleware

## About

This repository contains the software and viewer for the Belgian
electronic identity card. With this, you can:
- Communicate with secure websites that require eID authentication
- Sign documents and emails using your eID
- Using the viewer, read the identity data on eID cards, verify their
  validity, and store them for future usage
- Using the provided API, do all of the above in custom applications of
  your own.

If you wish to contribute, use the normal github procedures (i.e., file
an issue, open a pull request, ...).

For help with getting the software to work, please contact the support
desk of the administration whose services you are attempting to use.

## Installation

### Code from git repository:

- Make sure you have libtool 2.2.7c or above, autoconf and automake installed, and in your `$PATH`
- run `autoreconf -i`
- proceed to the next step

### Code from a source package, or code you just bootstrapped as above

#### Ubuntu 16.04.2 LTS 64-bit (automated script)

https://mark911.wordpress.com/2014/01/04/how-to-get-the-eid-electronic-card-reader-acr38u-working-in-ubuntu-14-04-64-bit/

#### GNU/Linux

Make sure you have pcsc-lite and GTK+ and and their development libraries
installed. After that it's just:

    ./configure 
    make
    sudo make install

You should now have beid* libraries in /usr/local/lib and one beid* executable in /usr/local/bin

At runtime, you will need a running pcsc daemon ("pcscd") with the correct driver for your type of card reader
(ccid drivers are generally installed automatically along with the pcscd package)

Pin dialogs can be disabled eg for applications that have their own dialogs:

    ./configure --enable-dialogs=no # the default is yes

#### OS X

Just open the project in Xcode (6 or above) and hit "start"

#### Windows:

Just open the project in Visual Studio (2012 or above)

#### Firefox

To use the Belgian eID in Firefox, we recommend the Firefox extension to handle configuration automatically. 
The extension will be installed on Linux and OSX. The default install locations:

- Linux: `DATADIR/mozilla/extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}`
  (`DATADIR` is by default `PREFIXDIR/lib` - `PREFIXDIR` is by default `/usr/local`)
- OSX: `/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}`

To change the install location:
 
    ./configure --with-mozext=/mozilla/firefox/extensions/directory

To create an XPI package, run:
 
    make xpipackage   

# Status badges

[![Coverage Status](https://coveralls.io/repos/github/Fedict/eid-mw/badge.svg?branch=v4.2)](https://coveralls.io/github/Fedict/eid-mw?branch=v4.2) [![Build Status](https://travis-ci.org/Fedict/eid-mw.svg?branch=master)](https://travis-ci.org/Fedict/eid-mw) [![Coverity Scan Build Status](https://scan.coverity.com/projects/2576/badge.svg?flat=1)](https://scan.coverity.com/projects/eid-middleware) [![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/1067/badge)](https://bestpractices.coreinfrastructure.org/projects/1067)
