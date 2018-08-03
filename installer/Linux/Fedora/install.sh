#!/bin/bash

set -e

echo "****** INSTALLATION START ******"

script_path=$(cd `dirname $0`; pwd)

#----------------------------------------

sudo dnf update -y && sudo dnf upgrade -y
sudo dnf install -y gcc gcc-c++ make cmake
sudo dnf install -y opencv opencv-contrib opencv-devel
sudo dnf install -y boost-devel

#----------------------------------------

bash $script_path/../../../SungemSDK/installer/Linux/install.sh >/dev/null

#----------------------------------------

echo "****** INSTALLATION COMPLETE ******"