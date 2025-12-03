#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#-------------------------------------------------------------------
name=$1
url=$2
check=$3

mkdir -p ${WDIR}
cd ${WDIR}
mkdir -p $name
cd $name

if [ -f "expand.stamp" ]; then exit; fi

echo "fetch $name"
distr=`${CDIR}/getDistr.sh $url $check ${name}-`

if [[ "$distr" == *.tar.* ]]; then
    echo "untar $distr"
    tar -xf $distr
elif [[ "$distr" == *.zip ]]; then
    echo "uzip $distr"
    mkdir -p zipExtracted
    unzip -u -d zipExtracted $distr
else
    echo "keep $distr"
fi

touch expand.stamp
