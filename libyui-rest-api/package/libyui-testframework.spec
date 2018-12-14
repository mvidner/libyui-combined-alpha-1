#
# spec file for package libyui-testframework
#
# Copyright (c) 2018 SUSE LINUX GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           libyui-testframework
Version:        0.1.0
Release:        0
Source:         %{name}-%{version}.tar.bz2

%define so_version 9
%define bin_name %{name}%{so_version}

# optionally build with code coverage reporting,
# this uses debug build, do not use in production!
%bcond_with coverage

%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
BuildRequires:  libboost_test-devel
%else
BuildRequires:  boost-devel
%endif
%define libyui_devel_version libyui-devel >= 3.4.0
BuildRequires:  %{libyui_devel_version}
BuildRequires:  cmake >= 2.8
BuildRequires:  gcc-c++
BuildRequires:  jsoncpp-devel
BuildRequires:  libmicrohttpd-devel
BuildRequires:  libyui-devel >= 3.0.4
BuildRequires:  pkg-config

%if %{with coverage}
# normally the coverage feature should not be used out of CI
# but to be on the safe side...
BuildRequires:  lcov
%endif

Url:            http://github.com/libyui/
Summary:        GUI-abstraction library
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          System/Libraries

%description
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can now be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.

%package -n %{bin_name}

Provides:       yast2-libyui = 2.42.0
Obsoletes:      yast2-libyui < 2.42.0
Requires:       libyui%{so_version}
Requires:       yui_backend = %{so_version}

Url:            http://github.com/libyui/
Summary:        Libyui - GUI-abstraction library
Group:          System/Libraries

%description -n %{bin_name}
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can now be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.

%package devel

%if 0%{?suse_version} > 1325
Requires:       libboost_headers-devel
Requires:       libboost_test-devel
%else
Requires:       boost-devel
%endif
Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel

Url:            http://github.com/libyui/
Summary:        Libyui header files
Group:          Development/Languages/C and C++

%description devel
This is the user interface engine that provides the abstraction from
graphical user interfaces (Qt, Gtk) and text based user interfaces
(ncurses).

Originally developed for YaST, it can now be used independently of
YaST for generic (C++) applications. This package has very few
dependencies.

This can be used independently of YaST for generic (C++) applications.
This package has very few dependencies.

%prep
%setup -q -n %{name}-%{version}

%build

export CFLAGS="$RPM_OPT_FLAGS -DNDEBUG"
export CXXFLAGS="$RPM_OPT_FLAGS -DNDEBUG"

./bootstrap.sh %{_prefix}

mkdir build
cd build

%if %{?_with_debug:1}%{!?_with_debug:0}
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELWITHDEBINFO
%else
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELEASE
%endif

make %{?jobs:-j%jobs}

%install
cd build
make install DESTDIR="$RPM_BUILD_ROOT"
install -m0755 -d $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/
install -m0755 -d $RPM_BUILD_ROOT/%{_libdir}/yui
install -m0644 ../COPYING* $RPM_BUILD_ROOT/%{_docdir}/%{bin_name}/

%clean
rm -rf "$RPM_BUILD_ROOT"

%post -n %{bin_name} -p /sbin/ldconfig

%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%defattr(-,root,root)
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_prefix}/include/yui
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
