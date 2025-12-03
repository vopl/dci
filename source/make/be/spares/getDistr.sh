#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#-------------------------------------------------------------------
url=$1
check=$2
prefixName=$3

mkdir -p ${DISTRDIR}
fname=${DISTRDIR}/${prefixName}${url##*/}
echo "${fname}"

if [ -f "${fname}.stamp" ]; then exit; fi
wget --no-check-certificate -c $url -O $fname

if [[ `sha256sum -b $fname` != *${check}* ]]; then
    >&2 echo "bad check for $fname"
    exit 1
fi
touch ${fname}.stamp
