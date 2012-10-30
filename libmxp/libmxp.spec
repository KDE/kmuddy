
%define release 1

Name: libmxp
Version: 0.2.4
Release: %{release}
Summary: Library to parse MXP (MUD eXtension Protocol) streams
License: LGPL
BuildRequires: cmake gcc-c++
Group: System/Libraries
URL: http://www.kmuddy.org/libmxp/
Packager: Tomas Mecir <kmuddy@kmuddy.com>
Source: http://www.kmuddy.org/libmxp/files/libmxp-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
This library can be used by MUD clients to parse MXP chunks coming from
the MUD server.

%package -n libmxp0
Summary: Library to parse MXP (MUD eXtension Protocol) streams
Group: System/Libraries

%description -n libmxp0
This library can be used by MUD clients to parse MXP chunks coming from
the MUD server.

# **************************************************************************

%package -n libmxp-devel
Summary: Development files for the libmxp library
Group: Development/Libraries
Requires: libmxp0 = %{version}-%{release}

%description -n libmxp-devel
This package contains development headers for the libmxp library.

# **************************************************************************
# [PREPARATION]
%prep

%ifarch x86_64
%define _libdir lib64
%define _script configure64
%else
%define _libdir lib
%define _script configure
%endif

%setup -q -n libmxp-%{version}

# **************************************************************************
# [BUILD]
%build

%ifarch i386 i486 i586 i686
RPM_OPT_FLAGS=`echo $RPM_OPT_FLAGS | sed -e 's/-m\(.86\)/-mcpu=i\1/g'`
%endif

./%{_script}

make

# **************************************************************************
# [INSTALLATION]
%install

make install DESTDIR=$RPM_BUILD_ROOT

%post -n libmxp0
/sbin/ldconfig

%postun -n libmxp0
/sbin/ldconfig


# **************************************************************************
# [VERIFICATION]

# **************************************************************************
# [CLEAN]
%clean

rm -rf $RPM_BUILD_ROOT

# **************************************************************************
# [FILES]
%files -n libmxp0

%defattr(-, root, root)
%doc AUTHORS COPYING COPYING.LIB ChangeLog* NEWS README* TODO
%{_prefix}/%{_libdir}/libmxp.so.0*

%files -n libmxp-devel
%defattr(-, root, root)
%dir %{_prefix}/include/libmxp
%doc doc/USAGE
%{_prefix}/include/libmxp/libmxp.h
%{_prefix}/%{_libdir}/libmxp.so

