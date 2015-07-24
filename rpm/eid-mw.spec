#Summary: Belgium electronic identity card PKCS#11 module and Firefox plugin
# Authority: dag

Version: 4.1.4
Release: 0.%{revision}%{?dist}
License: LGPL
Group: Applications/Communications
URL: http://eid.belgium.be/
Summary: Belgium electronic identity card PKCS#11 module and Firefox plugin
Name: eid-mw

Source: http://eidmw.yourict.net/dist/sources/eid-mw-%{version}-%{revision}.tar.gz
Source1: baselibs.conf
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%if 0%{?el6}
BuildRequires: gtk2-devel
%else
BuildRequires: gtk3-devel
%endif
BuildRequires: pcsc-lite-devel
BuildRequires: desktop-file-utils
Requires(pre): /sbin/chkconfig
Requires(pre): /sbin/service
Requires: eid-mw-bin
Requires: eid-mw-libs
%if 0%{?suse_version}
Requires: pcsc-ccid
BuildRequires: gcc-c++
BuildRequires: libopenssl-devel
%else
Requires: ccid
BuildRequires: openssl-devel
%endif
BuildRequires: libcurl-devel
BuildRequires: libxml2-devel
Conflicts: openct

%description
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents.

%package devel
Summary: Belgium electronic identity card PKCS#11 module - development package
Requires: eid-mw

%description devel
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents. This package contains the files needed
to develop against the eID Middleware.

%package bin
Summary: Belgium electronic identity card PKCS#11 module - helper binaries
Requires: eid-mw

%description bin
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents. This package contains a few helper
programs needed by the eID Middleware.

%package libs
Summary: Belgium electronic identity card PKCS#11 module - libraries

%description libs
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents. This package contains the actual libraries.

%package vwr
Summary: Belgium electronic identity card viewer
Obsoletes: eid-viewer
Requires: eid-mw
%if 0%{?suse_version}
Requires: java >= 1.6.0
Requires: pcsc-ccid
%else
Requires: java >= 1:1.6.0
Requires: ccid
%endif
Requires: pcsc-lite
Conflicts: openct

%description vwr
The eid-viewer application allows the user to read out any information from
a Belgian electronic identity card. Both identity information and information
about the stored cryptographic keys can be read in a user-friendly manner,
and can easily be printed out or stored for later reviewal.

The application verifies the signature of the identity information,
checks whether it was signed by a government-issued key, and optionally
checks the certificate against the government's Trust Service.

%prep
%setup -n eid-mw-%{version}-%{revision}

%build
%configure
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"
rm -f %{buildroot}%{_datadir}/applications/about-eid-mw.desktop
desktop-file-install --dir %{buildroot}%{_datadir}/applications --vendor fedict plugins_tools/aboutmw/gtk/about-eid-mw.desktop || true

%clean
%{__rm} -rf %{buildroot}

%post
/sbin/ldconfig

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
%doc NEWS README
%{_datadir}/mozilla/
%exclude %{_libdir}/*.la
%files libs
%doc NEWS README
%{_libdir}/libbeidpkcs11.so.*
%exclude %{_libdir}/libbeidpkcs11.so
%files bin
%doc NEWS README
%{_libexecdir}/beid-askaccess
%{_libexecdir}/beid-askpin
%{_libexecdir}/beid-badpin
%{_libexecdir}/beid-changepin
%{_libexecdir}/beid-spr-askpin
%{_libexecdir}/beid-spr-changepin
%{_bindir}/about-eid-mw
%{_datadir}/applications
%{_datadir}/locale/*/LC_MESSAGES/about-eid-mw.mo
%files devel
%doc NEWS README
%{_libdir}/libbeidpkcs11.a
%{_libdir}/libbeidpkcs11.so
%{_libdir}/pkgconfig
%{_includedir}/beid/

%files vwr
%defattr(-,root,root,0755)
%{_bindir}/eid-viewer
%{_datadir}/locale/*/LC_MESSAGES/eid-viewer.mo
%{_datadir}/eid-mw
%if ! 0%{?el6}
%{_datadir}/glib-2.0/schemas
%endif

%changelog
* Thu Nov 27 2014 Wouter Verhelst <wouter.verhelst@fedict.be> - 4.0.6-0.R
- Install about-eid-mw into the eid-mw-bin package

* Thu Aug 14 2014 Wouter Verhelst <wouter.verhelst@fedict.be> - 4.0.6-0.R
- Split up somewhat further so that openSUSE-style multiarch works, too.

* Thu Jul 31 2014 Wouter Verhelst <wouter.verhelst@fedict.be> - 4.0.6-0.R
- Split package up into several subpackages so as to make multiarch work
  without much issues.

* Tue Oct 15 2013 Frank Marien <frank@apsu.be> - 4.0.6-0.R
- Upgrade to 4.0.6

* Thu May 3 2012 Frank Marien <frank@apsu.be> - 4.0.4-0.R
- Upgrade to 4.0.4

* Wed Mar 14 2012 Frank Marien <frank@apsu.be> - 4.0.2-0.R
- Upgrade to 4.0.2

* Fri Mar 18 2011 Frank Marien <frank@apsu.be> - 4.0.0-0.R
- Made Revision number variable to allow continuous builds.

* Thu Mar 17 2011 Dag Wieers <dag@wieers.com> - 4.0.0-0.6
- Split eid-mw and eid-viewer packages.

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

