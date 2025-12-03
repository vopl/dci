#!/bin/bash
#virtualbox + CentOS-8
#http://isoredirect.centos.org/centos/8-stream/isos/x86_64/

set -e

#-------------------------------------------------------------------
dnf -y install net-tools
dnf -y install sudo mc
dnf -y install wget tar bzip2 unzip
dnf config-manager --set-enabled powertools
dnf -y install flex bison texinfo glibc-devel gcc gcc-c++ make python3-devel python3 ninja-build
alternatives --set python /usr/bin/python3
dnf -y install systemd-devel libinput-devel pulseaudio-libs-devel dbus-devel at-spi2-core-devel mesa-libGL-devel mesa-libEGL-devel mesa-libGLES-devel
dnf -y install libidn2-devel

dnf -y install fontconfig-devel freetype-devel harfbuzz-devel
dnf -y install libxcb-devel xcb-util-devel xcb-util-image-devel xcb-util-keysyms-devel xcb-util-renderutil-devel xcb-util-wm-devel
dnf -y install libX11-devel libICE-devel libSM-devel libXScrnSaver-devel libXext-devel libXfont2-devel libXft-devel libXi-devel libXinerama-devel
dnf -y install libXmu-devel libXp-devel libXpm-devel libXrandr-devel libXrender-devel libXt-devel libXtst-devel libXv-devel libXvMC-devel
dnf -y install libXxf86dga-devel libXxf86misc-devel libXxf86vm-devel libfontenc-devel libXext-devel libXfixes-devel libXi-devel libXinerama-devel
dnf -y install libXrender-devel libxkbcommon-devel libxkbcommon-x11-devel libxkbfile-devel
dnf -y install pulseaudio-libs-devel
dnf -y install libjpeg-turbo-devel libpng-devel
dnf -y install perl-Pod-Html

dnf -y install git

#-------------------------------------------------------------------
if [[ `id -u builder` ]]; then
    echo "user already exists: builder"
else
    echo "create user: builder"
    useradd --create-home builder
fi

#-------------------------------------------------------------------
echo "all done"
