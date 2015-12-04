BeID middleware installation
============================

Code from git repository:
-------------------------

- Make sure you have libtool 2.2.7c or above, autoconf and automake installed, and in your `$PATH`
- run `autoreconf -i`
- proceed to the next step

Code from a source package, or code you just bootstrapped as above
------------------------------------------------------------------

### GNU/Linux

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

### OS X

Just open the project in Xcode (6 or above) and hit "start"

### Windows:

Just open the project in Visual Studio (2012 or above)

### Firefox

To use the Belgian eID in Firefox, we recommend the Firefox extension to handle configuration automatically. 
The extension will be installed on Linux and OSX. The default install locations:

- Linux: `DATADIR/mozilla/extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}`
  (`DATADIR` is by default `PREFIXDIR/lib` - `PREFIXDIR` is by default `/usr/local`)
- OSX: `/Library/Application Support/Mozilla/Extensions/{ec8030f7-c20a-464f-9b0e-13a3a9e97384}`

To change the install location:
 
    ./configure --with-mozext=/mozilla/firefox/extensions/directory

To create an XPI package, run:
 
    make xpipackage   
