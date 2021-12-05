#!/bin/bash
set -e

# https://www.msys2.org/

pacman -Sq --needed \
	git openssh \
	mingw-w64-x86_64-openssl \
	mingw-w64-x86_64-gcc mingw-w64-x86_64-binutils \
	mingw-w64-x86_64-lldb \
	mingw-w64-x86_64-cmake \
	mingw-w64-x86_64-ninja mingw-w64-x86_64-make \
	mingw-w64-x86_64-gtest \
	mingw-w64-x86_64-icu mingw-w64-x86_64-boost \
	mingw-w64-x86_64-zlib mingw-w64-x86_64-zstd mingw-w64-x86_64-libxml2 mingw-w64-x86_64-sqlite3 mingw-w64-x86_64-pcre2 mingw-w64-x86_64-pugixml \
	mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-qt6-declarative mingw-w64-x86_64-qt-creator \


# добавить к путям C:\msys64\usr\bin\;C:\msys64\mingw64\bin\
