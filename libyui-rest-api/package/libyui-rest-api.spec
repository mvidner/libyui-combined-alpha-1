#
# spec file for package libyui-rest-api
#
# Copyright (c) 2019 SUSE LINUX GmbH, Nuernberg, Germany.
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


%define so_version 10
%define bin_name %{name}%{so_version}
%define libyui_devel_version libyui-devel >= 3.5.0
# optionally build with code coverage reporting,
# this uses debug build, do not use in production!
%bcond_with coverage
Name:           libyui-rest-api
Version:        0.1.0
Release:        0
Summary:        Libyui - REST API plugin
License:        LGPL-2.1-only OR LGPL-3.0-only
Group:          System/Libraries
URL:            http://github.com/libyui/libyui-rest-api
Source:         %{name}-%{version}.tar.bz2
BuildRequires:  %{libyui_devel_version}
BuildRequires:  cmake >= 2.8
# Qt UI specific
BuildRequires:  fontconfig-devel
BuildRequires:  gcc-c++
BuildRequires:  jsoncpp-devel
BuildRequires:  libmicrohttpd-devel
# ncurses UI specific
BuildRequires:  libyui-ncurses-devel
BuildRequires:  libyui-qt-devel
BuildRequires:  ncurses-devel
BuildRequires:  pkgconfig
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5X11Extras)
%if 0%{?suse_version} > 1325
BuildRequires:  libboost_headers-devel
BuildRequires:  libboost_test-devel
%else
BuildRequires:  boost-devel
%endif
%if %{with coverage}
# normally the coverage feature should not be used out of CI
# but to be on the safe side...
BuildRequires:  lcov
%endif

%description
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.

%package -n %{bin_name}
Summary:        Libyui - REST API plugin
Group:          System/Libraries
URL:            http://github.com/libyui/libyui-rest-api
Requires:       libyui%{so_version}
Requires:       yui_backend = %{so_version}
Provides:       %{name} = %{version}
Provides:       yast2-rest-api = %{version}
Obsoletes:      yast2-rest-api < 0.1.0

%description -n %{bin_name}
This package provides a libyui REST API plugin.

It allows inspecting and controlling the UI remotely via
an HTTP REST API, it is designed for automated tests.

%package devel
Summary:        Libyui header files
Group:          Development/Languages/C and C++
URL:            http://github.com/libyui/
Requires:       %{bin_name} = %{version}
Requires:       glibc-devel
Requires:       libstdc++-devel
%if 0%{?suse_version} > 1325
Requires:       libboost_headers-devel
Requires:       libboost_test-devel
%else
Requires:       boost-devel
%endif

%description devel
This package provides a libyui REST API plugin.

This is a development subpackage.

%prep
%setup -q

%build

export CFLAGS="%{optflags} -DNDEBUG"
export CXXFLAGS="%{optflags} -DNDEBUG"

./bootstrap.sh %{_prefix}

mkdir build
cd build

%if %{?_with_debug:1}%{!?_with_debug:0}
# FIXME: you should use %%cmake macros
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELWITHDEBINFO
%else
# FIXME: you should use %%cmake macros
cmake .. \
        -DYPREFIX=%{_prefix} \
        -DDOC_DIR=%{_docdir} \
        -DLIB_DIR=%{_lib} \
        -DCMAKE_BUILD_TYPE=RELEASE
%endif

make %{?_smp_mflags}

%install
cd build
%make_install
install -m0755 -d %{buildroot}/%{_docdir}/%{bin_name}/
install -m0755 -d %{buildroot}/%{_libdir}/yui
install -m0644 ../COPYING* %{buildroot}/%{_docdir}/%{bin_name}/

%post -n %{bin_name} -p /sbin/ldconfig
%postun -n %{bin_name} -p /sbin/ldconfig

%files -n %{bin_name}
%dir %{_libdir}/yui
%{_libdir}/yui/lib*.so.*
%doc %dir %{_docdir}/%{bin_name}
%license %{_docdir}/%{bin_name}/COPYING*

%files devel
%dir %{_docdir}/%{bin_name}
%{_libdir}/yui/lib*.so
%{_includedir}/yui
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/cmake/%{name}

%changelog
