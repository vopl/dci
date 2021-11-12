#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

WD=${HOME}/bb-worker
echo ${WD}

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

buildbot-worker create-worker --umask=0o022 worker 127.0.0.1 centos7 scout

mkdir -p ${HOME}/.cache/xdgr
chmod 0700 ${HOME}/.cache/xdgr

echo "#!/bin/bash" > env.sh
echo "export export XDG_RUNTIME_DIR=${HOME}/.cache/xdgr" >> env.sh
echo "export PATH=${PREFIX}/bin:\${PATH}" >> env.sh
echo "export LD_LIBRARY_PATH=${PREFIX}/lib:\${LD_LIBRARY_PATH}" >> env.sh
echo "export DCI_BE_DIR=${PREFIX}" >> env.sh
echo "export NPROC=\`nproc\`" >> env.sh
echo "export AUPSDIR=${HOME}/dci-aups" >> env.sh
chmod +x env.sh

echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && source env.sh && buildbot-worker start \"\$@\" ./worker" > start.sh
chmod +x start.sh

echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && source env.sh && buildbot-worker restart \"\$@\" ./worker" > restart.sh
chmod +x restart.sh

echo "#!/bin/bash
cd ${WD} && source sandbox/bin/activate && source env.sh && buildbot-worker stop \"\$@\" ./worker" > stop.sh
chmod +x stop.sh


#############################################
echo "[Unit]
Description=BuildBot worker service
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
" > bb-worker.service

echo "try as root:
cp ${WD}/bb-worker.service /etc/systemd/system
systemctl enable bb-worker
systemctl start bb-worker
";
