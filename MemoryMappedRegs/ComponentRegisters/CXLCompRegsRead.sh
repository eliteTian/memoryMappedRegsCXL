#!/bin/sh
echo "Enable mem bar access"
#setpci -s 98:0.0 COMMAND=0x142
echo "Enable mmap to access more memory"
sysctl vm.max_map_count
sysctl -w vm.max_map_count=2097152
echo "Decode the CXL2.0 Comp Register address"

pciLoc=$(lspci | grep 0d93 | grep -o '[0-9][0-9]:[0-9][0-9].0' )
headAddr=$(lspci -s $pciLoc -v | grep "ID=0008" | cut -d "[" -f2 | cut -d "]" -f1)

echo "The returned ninjastring is $headAddr !"
setpci -s $pciLoc COMMAND=0x142
setpci -s $pciLoc $headAddr.l
rm -rf main
gcc -o main main.c
#./main
./main > CXLCompRegs.txt

echo "CXL 2.0 Component Registers have been dumped to CXLCompRegs.txt"
exit
