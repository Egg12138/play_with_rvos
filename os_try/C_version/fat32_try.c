/*
❯ mkfs.fat -f 4 -s 8  -S 512 -v a.img

参考输出：
mkfs.fat 4.1 (2017-01-24)
a.img has 64 heads and 32 sectors per track,
hidden sectors 0x0000;
logical sector size is 512,
using 0xf8 media descriptor, with 195 sectors;
drive number 0x80;
filesystem has 4 12-bit FATs and 8 sectors per cluster.
FAT size is 1 sector, and provides 19 clusters.
There is 1 reserved sector.
Root directory contains 512 slots and uses 32 sectors.
Volume ID is 375f1720, no volume label.

(FAT12)
❯ file a.img
a.img: 
DOS/MBR boot sector, 
code offset 0x3c+2, 
OEM-ID "mkfs.fat", 
sectors/cluster 8, FATs 4, root entries 512, sectors 195 (volumes <=32 MB), 
Media descriptor 0xf8, 
sectors/FAT 1, 
sectors/track 32, 
heads 64, 
serial number 0x375f1720, 
unlabeled, FAT (12 bit)

❯ mkfs.fat -F 32 -v a.img
mkfs.fat 4.1 (2017-01-24)
WARNING: Not enough clusters for a 32 bit FAT!
a.img has 64 heads and 32 sectors per track,
hidden sectors 0x0000;
logical sector size is 512,
using 0xf8 media descriptor, with 195 sectors;
drive number 0x80;
filesystem has 2 32-bit FATs and 1 sector per cluster.
FAT size is 2 sectors, and provides 159 clusters.
There are 32 reserved sectors.
Volume ID is 48bd9085, no volume label.

❯ file a.img
a.img: DOS/MBR boot sector, 
code offset 0x58+2, 
OEM-ID "mkfs.fat", 
sectors 195 (volumes <=32 MB), 
Media descriptor 0xf8, 
sectors/track 32, 
heads 64, 
FAT (32 bit), 
sectors/FAT 2, 
serial number 0x48bd9085, unlabeled
*/
#include <stdlib.h>
/**
 * All allocation for a file must be an integral 
multiple of a cluster.
 * FAT type definitions: 
 * volume 
 * A logically contiguous sector address space as specified in the relevant standard for 
 * recording. 
 * 
*/




