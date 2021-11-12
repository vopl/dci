#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

WD=${HOME}/bb-master

if [ -d ${WD} ]; then
    echo "${WD} already exists"
    exit
fi

rm -rf ${WD}
mkdir -p ${WD}
cd ${WD}

python3 -m venv sandbox
source sandbox/bin/activate
pip install --upgrade pip
pip install 'buildbot[bundle]'
pip install 'buildbot[tls]'

buildbot create-master master
cp ${CDIR}/bb_master.cfg master/master.cfg

echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && buildbot start \"\$@\" ./master" > start.sh
chmod +x start.sh
echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && buildbot restart \"\$@\" ./master" > restart.sh
chmod +x restart.sh
echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && buildbot stop \"\$@\" ./master" > stop.sh
chmod +x stop.sh
echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && buildbot checkconfig \"\$@\" ./master" > checkconfig.sh
chmod +x checkconfig.sh

#############################################
echo "[Unit]
Description=BuildBot master service
After=network.target

[Service]
User=builder
Group=builder
WorkingDirectory=${WD}
ExecStart=${WD}/start.sh --nodaemon
ExecReload=/bin/kill -HUP \$MAINPID

RemainAfterExit=no
Restart=always
RestartSec=5s

[Install]
WantedBy=multi-user.target
" > bb-master.service

echo "try as root:
cp ${WD}/bb-master.service /etc/systemd/system
systemctl enable bb-master
systemctl start bb-master
";