#!/bin/bash

set -e

echo "****** INSTALLATION START ******"

script_path=$(cd `dirname $0`; pwd)

#----------------------------------------

sudo yum install -y epel-release
sudo yum update -y && sudo yum upgrade -y
sudo yum install -y gcc gcc-c++ make cmake
sudo yum install -y opencv opencv-devel
sudo yum install -y boost-devel

#----------------------------------------

bash $script_path/../../../SungemSDK/installer/Linux/install.sh >/dev/null

#----------------------------------------

echo "****** INSTALLATION COMPLETE ******"