#!/bin/bash

set -e

if [ "builder" != `whoami` ]; then
    echo "try this: sudo -u builder $0"
    exit 1
fi

#-------------------------------------------------------------------
CDIR=`realpath ${BASH_SOURCE%/*}`

${CDIR}/spares/install_gcc.sh 1
${CDIR}/spares/install_binutils.sh 1
${CDIR}/spares/install_gcc.sh 2
${CDIR}/spares/install_binutils.sh 2

${CDIR}/spares/install_chrpath.sh
${CDIR}/spares/install_gdb.sh
${CDIR}/spares/install_valgrind.sh

${CDIR}/spares/install_zlib.sh
${CDIR}/spares/install_zstd.sh
${CDIR}/spares/install_openssl.sh
${CDIR}/spares/install_openssh.sh

${CDIR}/spares/install_icu.sh
${CDIR}/spares/install_boost.sh
${CDIR}/spares/install_libxml.sh
${CDIR}/spares/install_sqlite.sh
${CDIR}/spares/install_pcre.sh

${CDIR}/spares/install_cmake.sh
${CDIR}/spares/install_pugixml.sh
${CDIR}/spares/install_gtest.sh

${CDIR}/spares/install_qt.sh
${CDIR}/spares/install_opensans.sh

${CDIR}/spares/install_curl.sh
${CDIR}/spares/install_git.sh

#-------------------------------------------------------------------
${CDIR}/spares/separateDebug.sh
${CDIR}/spares/fixRpath.sh

#-------------------------------------------------------------------
#${CDIR}/spares/install_bb_master.sh
#${CDIR}/spares/install_bb_worker.sh

#-------------------------------------------------------------------
echo 'all done'
