#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

pip3 install --upgrade pip
pip3 install meson setuptools
pip3 install jinja2 markdown markupsafe packaging pygments typogrify
