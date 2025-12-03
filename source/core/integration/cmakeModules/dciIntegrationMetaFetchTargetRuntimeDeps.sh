#!/bin/bash

set -e

UNIT=$1
TARGET=$2
FILE=$3
LIBDIR=$4

UNAME=$( command -v uname)

if [[ $( "${UNAME}" | tr '[:upper:]' '[:lower:]') =~ ^.*(msys|mingw).*$ ]]; then
    PATH="${LIBDIR}:${PATH}"
    function doOne() {
        objdump -p $1|grep "DLL Name:" | awk '{print $3}' | xargs -I{} whereis {} | awk -F.': ' '{print $2}' | xargs -I{} cygpath -m {}
    }
else
    function doOne() {
        lddOur=`ldd $1 2> /dev/null`
        objdump -p $1|grep NEEDED | awk '{print $2}' | while read NEEDED; do
            echo -n "$lddOur" | grep "$NEEDED => " | awk '{print $3}' | grep -v -e '^$' | xargs -I{} realpath -s {}
        done
    }
fi

function doOneCached() {
    CACHE=rtdepsCache/${1//\//_}
    CACHE=${CACHE//\\/_}
    CACHE=${CACHE//:/_}
    [ -f $CACHE -a $CACHE -nt $1 ] || (mkdir -p rtdepsCache && doOne $1 >> $CACHE)
    cat $CACHE
}

declare -A ALL
declare -A NEW
NEW[$FILE]=1

while [ "0" -lt ${#NEW[@]} ]; do
    for NEXT in ${!NEW[@]}; do
        #echo NEXT: $NEXT
        unset NEW[$NEXT]
        while IFS= read -r ONE; do
            if [ "" != "$ONE" ]; then
                #echo ONE: $ONE
                if [ ! ${ALL[$ONE]} ]; then
                    NEW[$ONE]=1
                fi
                ALL[$ONE]=1
            fi
        done <<< $(doOneCached $NEXT)
    done
done

echo -n "UNIT[${UNIT}] TARGET[${TARGET}] TARGET_DEPS["
for ONE in ${!ALL[@]}; do
    echo -n "$ONE;"
done
echo "] "
