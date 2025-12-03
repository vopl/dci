#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh opensans https://www.1001fonts.com/download/open-sans.zip 956967e70836627cd1df713480484078dedd45a338218b902002f348c1c44358
cd ${WDIR}/opensans

if [ ! -f "install.stamp" ]; then
    mkdir -p ${LIBDIR}/fonts
    cp zipExtracted/* ${LIBDIR}/fonts/
    touch install.stamp
fi
