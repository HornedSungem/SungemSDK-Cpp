#!/bin/bash

set -e

echo "****** INSTALLATION START ******"

script_path=$(cd `dirname $0`; pwd)

#----------------------------------------

bash $script_path/../../SungemSDK/installer/macOS/install.sh >/dev/null

#----------------------------------------

brew install cmake opencv boost

#----------------------------------------

echo "****** INSTALLATION COMPLETE ******"