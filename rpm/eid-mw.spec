Name:           eid-mw
Version:        4.0.0
Release:        M2M2%{?dist}
Summary:        Belgium Electronic Identity Card PKCS#11 Module and Firefox Plugin

Group:          Applications/Communications
License:        LGPL
URL:            http://eid.belgium.be/
Source0:        http://eidmw.yourict.net/dist/sources/%{name}-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires: pcsc-lite-devel gtk2-devel
Requires: pcsc-lite gtk2

%description
Belgium Electronic Identity Card PKCS#11 Module and Firefox Plugin
Install if you want to access websites and/or sign documents using Belgian e-ID cards

%prep
%setup -q


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)
%{_libdir}/*
%{_datadir}/*
%doc



%changelog
