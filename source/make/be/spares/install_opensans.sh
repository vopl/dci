#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh opensans http://webfonts.ru/original/opensans/opensans.zip fb2f1b546693bf296d96aed3f5776ec19f6be95312d2c157555ff4139ee2e6ea
cd ${WDIR}/opensans

if [ ! -f "install.stamp" ]; then
    mkdir -p ${LIBDIR}/fonts
    cp zipExtracted/* ${LIBDIR}/fonts/
    touch install.stamp
fi
