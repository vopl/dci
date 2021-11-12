#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

${CDIR}/listElfs.sh|while read FNAME; do
    if (readelf -S ${FNAME} |grep -q " .dynamic "); then
        DIR=`dirname ${FNAME}`
        CUR=`chrpath --list ${FNAME} | awk -F'=' '{print $2}'`
        RES=:$CUR:

        RES=`echo "${RES}" | sed 's/:_\+:/:/g'`
        RES=`echo "${RES}" | sed "s;:${PREFIX}.\+:;:;g"`
        RES=`echo "${RES}" | sed 's;/:;:;g'`

        RP1=`realpath --relative-to=${DIR}/X ${PREFIX}/lib`
        RP1=${RP1/../\$ORIGIN}
        if [[ "${RES}" != *:${RP1}:* ]]; then
            RES=$RES:$RP1:
        fi

        RES=`echo "${RES}" | sed 's/:\+/:/g' | sed 's/^:*//g' | sed 's/:*$//g'`
        if [[ "$CUR" != "${RES}" ]]; then
            echo "fix rpath: $FNAME: [$CUR] -> [$RES]"
            rm -f $FNAME.temporary
            cp $FNAME $FNAME.temporary
            chrpath --replace $RES $FNAME.temporary > /dev/null
            mv -f $FNAME.temporary $FNAME
        fi
    fi
done