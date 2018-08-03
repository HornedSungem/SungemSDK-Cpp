#!/bin/bash

set -e

echo "****** INSTALLATION START ******"

script_path=$(cd `dirname $0`; pwd)

#----------------------------------------

sudo apt update -y && sudo apt upgrade -y
sudo apt install -y make cmake
sudo apt install -y libopencv-dev libboost-dev

#----------------------------------------

bash $script_path/../../../SungemSDK/installer/Linux/install.sh >/dev/null

#----------------------------------------

echo "****** INSTALLATION COMPLETE ******"