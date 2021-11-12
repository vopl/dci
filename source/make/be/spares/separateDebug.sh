#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

${CDIR}/listElfs.sh|while read FNAME; do
    DBGLINK=`readelf --string-dump=.gnu_debuglink ${FNAME}  2>/dev/null|grep -E "\[ *0\]"|awk '{print $3}'`
    if [[ -z "$DBGLINK" ]]; then

        TMPFNAME=${FNAME}.temporary
        DBGFNAME=${FNAME}.debug
        echo "separate debug info: $FNAME -> $DBGFNAME"

        rm -f $DBGFNAME
        rm -f $TMPFNAME

        cp $FNAME $TMPFNAME
        objcopy --only-keep-debug $TMPFNAME $DBGFNAME
        objcopy --strip-all $TMPFNAME
        objcopy --add-gnu-debuglink=$DBGFNAME $TMPFNAME
        mv -f $TMPFNAME $FNAME
    fi
done