#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

${CDIR}/listElfs.sh|while read FNAME; do
    ldd $FNAME |grep " => /"|awk '{print $3}'
done|sort|uniq