#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh opensans http://webfonts.ru/original/opensans/opensans.zip 9a5d66dadf37fef424ed09e202240c9d73bfcf53c1090371d219a1904db06a1c
cd ${WDIR}/opensans

if [ ! -f "install.stamp" ]; then
    mkdir -p ${LIBDIR}/fonts
    cp zipExtracted/* ${LIBDIR}/fonts/
    touch install.stamp
fi
