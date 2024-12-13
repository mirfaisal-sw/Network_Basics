



Error Building LKM: -
======================

WARNING: modpost: "memset" [/data/home/mirfaisal/Test_Apps/Kasan_Test/Kasan_Test_Linux_Module/kasan_test.ko] undefined!
WARNING: modpost: "__arch_copy_from_user" [/data/home/mirfaisal/Test_Apps/Kasan_Test/Kasan_Test_Linux_Module/kasan_test.ko] undefined!
  CC [M]  /data/home/mirfaisal/Test_Apps/Kasan_Test/Kasan_Test_Linux_Module/kasan_test.mod.o
make[3]: *** No rule to make target 'scripts/module.lds', needed by '/data/home/mirfaisal/Test_Apps/Kasan_Test/Kasan_Test_Linux_Module/kasan_test.ko'.  Stop.
scripts/Makefile.modpost:139: recipe for target '__modpost' failed
make[2]: *** [__modpost] Error 2
Makefile:1833: recipe for target 'modules' failed
make[1]: *** [modules] Error 2
make[1]: Leaving directory '/data/home/mirfaisal/BMW_IDC/ES6_Codes/ES6_Artifact_176_CodeBase_19122023/sources/kernel'
Makefile:7: recipe for target 'all' failed
make: *** [all] Error 2
mirfaisal@awsmblx427bs004:~/Test_Apps/Kasan_Test/Kasan_Test_Linux_Module$ make all


Solution: -
============
cd <kernel/src/folder>

make modules_prepare
