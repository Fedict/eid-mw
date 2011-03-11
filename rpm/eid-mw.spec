# $Id$
# Authority: dag

### Disable Maven2 build as it fails on 32bit (and it takes an awful long time)
%{?fc14:%define _without_maven2 1}
%{?fc13:%define _without_maven2 1}

### RHEL5/RHEL6 do not ship with Maven2 (and it takes an awful long time)
%{?el6:%define _without_maven2 1}
%{?el5:%define _without_maven2 1}

Summary: Belgium electronic identity card PKCS#11 module and Firefox plugin
Name: eid-mw
Version: 4.0.0
Release: %{revision}%{?dist}
License: LGPL
Group: Applications/Communications
URL: http://eid.belgium.be/

Source0: http://eidmw.yourict.net/dist/sources/eid-mw-%{version}-%{revision}.tar.gz
Source1: eid-viewer-%{version}-%{revision}.tar.gz
Source2: eid-viewer.png
Source3: eid-applet-core-patched.tar.bz2
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Obsoletes: beid-mw
Obsoletes: eid-belgium

BuildRequires: desktop-file-utils
BuildRequires: gtk2-devel
BuildRequires: pcsc-lite-devel
%{!?_without_maven2:BuildRequires: maven2}
Requires(pre): /sbin/chkconfig
Requires(pre): /sbin/service
Requires: ccid
Requires: pcsc-lite-acr38u

%description
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents.

%package -n eid-viewer
Summary: Belgium electronic identity card viewer
Group: Applications/Communications

Requires: %{name} = %{version}-%{release}
Requires: java-1.6.0-openjdk
Obsoletes: eid-mw-viewer

%description -n eid-viewer
The eid-viewer application allows the user to read out any information from
a Belgian electronic identity card. Both identity information and information
about the stored cryptographic keys can be read in a user-friendly manner,
and can easily be printed out or stored for later reviewal.

The application verifies the signature of the identity information,
checks whether it was signed by a government-issued key, and optionally
checks the certificate against the government's Certificate Revocation List
(CRL) and/or by using the Online Certificate Status Protocol (OCSP) against
the government's servers.

%prep
%if %{?_without_maven2:1}0
%setup -a1
%else
%setup -a3
%endif

%{__cat} <<EOF >eid-viewer.sh
#!/bin/bash
java -jar %{_datadir}/eid-viewer/eid-viewer-gui-4.0.0-SNAPSHOT.jar
EOF

%{__cat} <<EOF >eid-viewer.desktop
[Desktop Entry]
Encoding=UTF-8
Name=eID Card Reader
Comment=Display and administer your eID card
Name[nl]=eID Kaart Lezer
Comment[nl]=Weergeven en beheren van uw eID kaart
Name[fr]=Lecteur de Carte eID
Comment[fr]=Affichage et gestion de votre carte eID
GenericName=eid-viewer
Exec=%{_bindir}/eid-viewer
Terminal=false
Type=Application
Icon=eid-viewer.png
Categories=Application;Utility;
EOF

### Disable the maven build when requested
%if %{?_without_maven2:1}0
echo -e "all:\n\ninstall:" >eid-viewer/Makefile
echo "#!/bin/true" >eid-viewer/configure
%else
pushd eid-applet-read-only-patched2
mvn -e clean install
popd
%endif

%build
%configure --disable-static
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"

%if %{?_without_maven2:1}0
%{__install} -Dp -m0755 eid-viewer.sh %{buildroot}%{_bindir}/eid-viewer
%{__install} -d -m0755 %{buildroot}%{_datadir}/eid-viewer/lib/
%{__cp} -av eid-viewer-%{version}/README.txt README-eid-viewer.txt
%{__cp} -av eid-viewer-%{version}/*.jar %{buildroot}%{_datadir}/eid-viewer/
%{__cp} -av eid-viewer-%{version}/lib/*.jar %{buildroot}%{_datadir}/eid-viewer/lib/
%else
%{__rm} %{buildroot}%{_datadir}/eid-viewer/eid-viewer.exe
%{__rm} %{buildroot}%{_datadir}/eid-viewer/eid-viewer.sh
%endif

%{__install} -d -m0755 %{buildroot}%{_datadir}/applications/
desktop-file-install \
    --dir %{buildroot}%{_datadir}/applications \
    eid-viewer.desktop
%{__install} -Dp -m0644 %{SOURCE2} %{buildroot}%{_datadir}/icons/eid-viewer.png

%clean
%{__rm} -rf %{buildroot}

%post
/sbin/ldconfig

### openct and pcscd are mutual exclusive and we need pcscd for eID Middleware.
### Not nice but if people need the eID Middleware, this is what is required !
if /sbin/service openct status &>/dev/null; then
    /sbin/service openct stop || :
fi

### Disable openct on boot during first install only !
if (( $1 == 1 )) && /sbin/chkconfig --list | grep -qP '^openct\s.+\s3:on\s'; then
    echo "WARNING: The openct service is now disabled on boot." >&2
    /sbin/chkconfig openct off
fi

### Make sure pcscd is enabled and make pcscd reread configuration and rescan USB bus.
if /sbin/service pcscd status &>/dev/null; then
    /usr/sbin/pcscd -H &>/dev/null || :
elif /sbin/chkconfig --list | grep -qP '^pcscd\s'; then
    /sbin/service pcscd start || :
else
    echo "ERROR: Your pcscd installation is seriously broken." >&2
    exit 1
fi

### Enable pcscd on boot during first install only !
if (( $1 == 1 )) && /sbin/chkconfig --list | grep -qP '^pcscd\s.+\s3:off\s'; then
    echo "INFO: The pcscd service is now enabled on boot." >&2
    /sbin/chkconfig pcscd on
fi

### Notify user if an action is required for the eID plugin to work.
if /usr/bin/pgrep 'firefox' &>/dev/null; then
    echo "INFO: You may have to restart Firefox for the Belgium eID add-on to work." >&2
elif /usr/bin/pgrep 'iceweasel' &>/dev/null; then
    echo "INFO: You may have to restart Iceweasel for the Belgium eID add-on to work." >&2
fi

%postun
/sbin/ldconfig

### Make pcscd reread configuration and rescan USB bus.
if /sbin/service pcscd status &>/dev/null; then
    %{_sbindir}/pcscd -H &>/dev/null || :
fi

%files
%defattr(-, root, root, 0755)
### Include license files
%doc ChangeLog NEWS README
### FIXME: Include a man-page about eid-mw and troubleshooting
#%doc %{_mandir}/man1/eid-mw.1*
%{_datadir}/mozilla/extensions/
### FIXME: It links against *.so, not against *.so.*
%{_libdir}/libbeidcardlayer.so
%{_libdir}/libbeidcommon.so.*
%{_libdir}/libbeiddialogs.so
%{_libdir}/libbeidpkcs11.so.*
%{_libdir}/libcardpluginbeid.so
%{_libdir}/libcardpluginsis.so
%{_libdir}/libcardpluginsis_acr38u.so
%exclude %{_libdir}/libbeidcommon.so
%exclude %{_libdir}/libbeidpkcs11.so
%exclude %{_libdir}/*.la

%files -n eid-viewer
%defattr(-, root, root, 0755)
%doc README-eid-viewer.txt
### FIXME: Include a man-page about eid-viewer
#%doc %{_mandir}/man8/eid-viewer.8*
%{_bindir}/eid-viewer
%{_datadir}/applications/eid-viewer.desktop
%{_datadir}/eid-viewer/
%{_datadir}/icons/eid-viewer.png

%changelog
* Thu Feb 24 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.5
- Added post-install script and desktop file.

* Thu Feb 24 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.4
- Included pre-built JAR files.

* Wed Feb 23 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.3
- Added patched eid-applet core.

* Sun Feb 13 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.2
- Included eid-viewer build using maven.

* Mon Feb  7 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.1
- Initial package.

