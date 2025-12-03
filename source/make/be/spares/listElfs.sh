#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

find ${PREFIX}/ -type f \( -executable -or -name "*.so*" \) |grep -v -E ".debug$"|xargs -L1 file -F " " |grep -E "ELF 64-bit LSB (executable|shared object), x86-64" |awk '{print $1}'