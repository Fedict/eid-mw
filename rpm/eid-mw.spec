Summary: Belgium electronic identity card PKCS#11 module and Firefox plugin
# Authority: dag

Summary: Belgium electronic identity card PKCS#11 module and Firefox plugin
Name: eid-mw
Version: 4.0.6
Release: 0.%{revision}%{?dist}
License: LGPL
Group: Applications/Communications
URL: http://eid.belgium.be/

Source: http://eidmw.yourict.net/dist/sources/eid-mw-%{version}-%{revision}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: gtk2-devel
BuildRequires: pcsc-lite-devel
Requires(pre): /sbin/chkconfig
Requires(pre): /sbin/service
%if 0%{?suse_version}
Requires: pcsc-ccid
%else
Requires: ccid
%endif

%description
The eID Middleware provides the libraries, a PKCS#11 module and a Firefox
plugin to use Belgian eID (electronic identity) card in order to access
websites and/or sign documents.

%prep
%setup

%build
%configure --disable-static
%{__make} %{?_smp_mflags}

%install
%{__rm} -rf %{buildroot}
%{__make} install DESTDIR="%{buildroot}"

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
%{_libexecdir}/beid-askaccess
%{_libexecdir}/beid-askpin
%{_libexecdir}/beid-badpin
%{_libexecdir}/beid-changepin
%{_libexecdir}/beid-spr-askpin
%{_libexecdir}/beid-spr-changepin
%exclude %{_libdir}/libbeidcommon.so
%exclude %{_libdir}/libbeidpkcs11.so
%exclude %{_libdir}/*.la

%changelog
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

