#!/bin/bash
if [ -d ${HOME}/bin/idcevo-elina-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/ ]
then
        export PATH=$PATH:${HOME}/bin/idcevo-elina-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/
        #echo $PATH
        #aarch64-poky-linux-gcc -v
else
        echo "Seems toolchain doesn't exist inpath : ${HOME}/bin/idcevo-elina-toolchain/sysroots/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/"
        echo "Set it up first in path ${HOME}/bin/idcevo-elina-toolchain/ and then try me... Bye for now...!"
        exit
fi

source ${HOME}/bin/idcevo-elina-toolchain/environment-setup-aarch64-poky-linux 
make
$STRIP switch_config_util
