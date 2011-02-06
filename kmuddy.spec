# This is a generic spec-file for the OpenSuSE BuildService. It likely won't work for anything else without adjustments.

%define _name kmuddy
%define _version 1.0.1
%define _release 1
%define _prefix /usr

Summary:   MUD client for KDE4
BuildRequires: gcc-c++ cmake >= 2.6.0 libmxp-devel

%if 0%{?suse_version}
Requires: kdelibs4 >= 4.1 libqt4 >= 4.4.0 libkde4 >= 4.1
BuildRequires: libqt4-devel >= 4.4.0 libkde4-devel >= 4.1
%endif

%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
Requires: kdelibs4 >= 4.1 qt4 >= 4.4.0
BuildRequires: qt4-devel >= 4.4.0 kdelibs4-devel >= 4.1
%endif

%if 0%{?mandriva_version}
Requires: kdelibs4-core >= 4.1 libqt4 >= 4.4.0
BuildRequires: libqt4-devel >= 4.4.0 kdelibs4-devel >= 4.1
%endif

Name:      %{_name}
Version:   %{_version}
Release:   %{_release}
Vendor:    Tomas Mecir <kmuddy@kmuddy.com>
Url:      http://www.kmuddy.com/
Packager:  %{_packager}
Group:     Amusement/Games/Other
Source:    %{_name}-%{_version}.tar.gz
License: GPL
BuildRoot: %{_tmppath}/%{name}-%{version}-build

%description
KMuddy is an extensible MUD client for KDE4. It includes many features such as ANSI color output with split-screen functionality, aliases, triggers (including colorization, rewrite and gag triggers), timers, scripting, recursive grouping for all object types, session transcript, a mapper with auto-mapping support and much more. The MCCP, MSP and MXP protocols are supported. A plug-in interface is also available.

%package devel
Summary: Files needed to develop KMuddy plugins
Group:   Amusement/Games/Other
Requires: kmuddy libkde4-devel >= 4.1

%description devel
This package contains the header files needed to compile KMuddy plug-ins.

%package mapper
Summary: A mapper module for KMuddy
Group:   Amusement/Games/Other
Requires: kmuddy

%description mapper
This package contains the mapper plug-in for KMuddy.


%prep
%setup

%ifarch x86_64
%define _libdir lib64
%else
%define _libdir lib
%endif

KDEDIR=%{_prefix}
cmake . -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=%{_prefix} $LOCALFLAGS
%build

# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

# properly find Qt4 on Mandriva
%if 0%{?mandriva_version}
export PATH=/usr/lib/qt4/bin:$PATH  
export QTDIR=%{_prefix}/lib/qt4/  
%endif  

make -j$numprocs

%install
make install DESTDIR=$RPM_BUILD_ROOT

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/%{_name}-%{version}

%post
/sbin/ldconfig

%postun
/sbin/ldconfig

%files
%defattr(-,root,root)
%dir %{_prefix}/share/kde4/apps/kmuddy
%{_prefix}/bin/kmuddy
%{_prefix}/%{_libdir}/libkmuddycore.so.1.0.0
%{_prefix}/%{_libdir}/libkmuddycore.so.1
%{_prefix}/share/kde4/servicetypes/kmuddyplugin.desktop
%{_prefix}/share/icons/hicolor/16x16/apps/kmuddy.png
%{_prefix}/share/icons/hicolor/32x32/apps/kmuddy.png
%{_prefix}/share/icons/locolor/16x16/apps/kmuddy.png
%{_prefix}/share/icons/locolor/32x32/apps/kmuddy.png
%{_prefix}/share/kde4/apps/kmuddy/muds.xml
%{_prefix}/%{_libdir}/kde4/kmuddyconverterplugin.so
%{_prefix}/%{_libdir}/kde4/kmuddystringsplugin.so
%{_prefix}/%{_libdir}/kde4/kmuddyscriptingplugin.so
%{_prefix}/%{_libdir}/kde4/kmuddyvartrigplugin.so
%{_prefix}/%{_libdir}/kde4/kmuddyvarviewerplugin.so
%{_prefix}/share/kde4/services/kmuddyconverterplugin.desktop
%{_prefix}/share/kde4/services/kmuddystringsplugin.desktop
%{_prefix}/share/kde4/services/kmuddyvartrigplugin.desktop
%{_prefix}/share/kde4/services/kmuddyvarviewerplugin.desktop
%{_prefix}/share/kde4/services/kmuddyscriptingplugin.desktop
%{_prefix}/share/kde4/apps/kmuddy/kmuddy-usock-test.pl
%{_prefix}/share/kde4/apps/kmuddy/pre.c
%{_prefix}/share/kde4/apps/kmuddy/vartest.c
%{_prefix}/share/kde4/apps/kmuddy/kmuddy-usock-client.pl
%{_prefix}/share/kde4/apps/kmuddy/kmuddyvars.h
%{_prefix}/share/kde4/apps/kmuddy/kmuddymapper_speedwalk.rc
%{_prefix}/share/kde4/apps/kmuddy/kmuddymapper_standard.rc
%{_prefix}/share/kde4/apps/kmuddy/tmpserver.pl
%{_prefix}/share/kde4/apps/kmuddy/README
%{_prefix}/share/kde4/apps/kmuddy/locktest.c
%{_prefix}/share/kde4/apps/kmuddy/testserver.cpp


%files devel
%defattr(-,root,root)
%{_prefix}/%{_libdir}/libkmuddycore.so
%if 0%{?fedora_version} || 0%{?rhel_version} || 0%{?centos_version}
%dir /usr/include/kde4/kmuddy
%{_prefix}/include/kde4/kmuddy/*.h
%else
%dir /usr/include/kmuddy
%{_prefix}/include/kmuddy/*.h
%endif

%files mapper
%defattr(-,root,root)
%dir %{_prefix}/share/kde4/apps/kmuddy/pics
%dir %{_prefix}/share/kde4/apps/kmuddy/toolbar
%{_prefix}/%{_libdir}/kde4/kmuddymapper.so
%{_prefix}/%{_libdir}/kde4/libkmuddymapper_standard.so
%{_prefix}/%{_libdir}/kde4/libkmuddymapper_speedwalk.so
%{_prefix}/share/kde4/services/kmuddymapper.desktop
%{_prefix}/share/kde4/services/kmuddymapper_standard.desktop
%{_prefix}/share/kde4/services/kmuddymapper_speedwalk.desktop
%{_prefix}/share/kde4/apps/kmuddy/kmuddymapperpart.rc
%{_prefix}/share/kde4/servicetypes/kmuddymapperplugin.desktop
%{_prefix}/share/kde4/apps/kmuddy/pics/kmud_follow.png
%{_prefix}/share/kde4/apps/kmuddy/pics/kmud_properties.png
%{_prefix}/share/kde4/apps/kmuddy/pics/kmud_inactive.png
%{_prefix}/share/kde4/apps/kmuddy/pics/kmud_active.png
%{_prefix}/share/kde4/apps/kmuddy/pics/kmud_delete.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_eraser.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_select.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_grid.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_text.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_path.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_room.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_zone.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_directions.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_lvlup.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_create.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_zoneup.png
%{_prefix}/share/kde4/apps/kmuddy/toolbar/kmud_lvldown.png

