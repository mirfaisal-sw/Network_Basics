


USAGE:
-------
	./pci_test.sh

Typical Output: -
-----------------
TEST-1 : PCIe CONTROLLER INIT TEST
        TEST SUCCESS: PCIE CONTROLLER INIT TEST

TEST-2 : PCIe AUTO ENUMERATION TEST
        TEST SUCCESS: PCIE AUTO ENUM. TEST

TEST-3 : PCIe DE-ENUMERATE TEST
        TEST SUCCESS: PCIE DE-ENUMERATE

TEST-4: PCIE REMOVE RESCAN TEST
        TEST SUCCESS:PCIE REMOVE RESCAN TEST

TEST-5 : PCIE ON-OFF-REMOVE-ON TEST
        TEST SUCCESS: PCIE ON-OFF-REMOVE-ON TEST

TEST-6 : PCIE REPETIVE ENUM. TEST
        TEST SUCCESS: PCIE REPETIVE ENUM SUCCEEDED EVERY 10 TIMES


IMPORTANT NOTE: -
------------------
Before running this script, we should ensure to unload PCIe client driver (for example WIFI driver).
Otherwise, it gives rise to kernel panic, as this script removes EP device and also does PCIe Link Down,
so for example if WIFI driver tries to access hardware register of WIFI chip (for example), then 
SError/Memory abort error will occur.

