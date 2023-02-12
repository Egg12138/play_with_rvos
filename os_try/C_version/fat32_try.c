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
/**
 * All allocation for a file must be an integral 
multiple of a cluster.
 * FAT type definitions: 
 * volume 
 * A logically contiguous sector address space as specified in the relevant standard for 
 * recording. 
 * 使用fat[N]来表示卷中的簇， 
 * 
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

// Copied from the manual
// Should be placed in a header file
struct fat32hdr {
        u8  BS_jmpBoot[3];
        u8  BS_OEMName[8];
        u16 BPB_BytsPerSec;
        u8  BPB_SecPerClus;
        u16 BPB_RsvdSecCnt;
        u8  BPB_NumFATs;
        u16 BPB_RootEntCnt;
        u16 BPB_TotSec16;
        u8  BPB_Media;
        u16 BPB_FATSz16;
        u16 BPB_SecPerTrk;
        u16 BPB_NumHeads;
        u32 BPB_HiddSec;
        u32 BPB_TotSec32;
        u32 BPB_FATSz32;
        u16 BPB_ExtFlags;
        u16 BPB_FSVer;
        u32 BPB_RootClus;
        u16 BPB_FSInfo;
        u16 BPB_BkBootSec;
        u8  BPB_Reserved[12];
        u8  BS_DrvNum;
        u8  BS_Reserved1;
        u8  BS_BootSig;
        u32 BS_VolID;
        u8  BS_VolLab[11];
        u8  BS_FilSysType[8];
        u8  __padding_1[420];
        u16 Signature_word;
} __attribute__((packed));

struct fat32dent {
        u8  DIR_Name[11];
        u8  DIR_Attr;
        u8  DIR_NTRes;
        u8  DIR_CrtTimeTenth;
        u16 DIR_CrtTime;
        u16 DIR_CrtDate;
        u16 DIR_LastAccDate;
        u16 DIR_FstClusHI;
        u16 DIR_WrtTime;
        u16 DIR_WrtDate;
        u16 DIR_FstClusLO;
        u32 DIR_FileSize;
} __attribute__((packed));

typedef enum FatType {
        FAT=1,
        FAT12,
        FAT16,
}  fatType;

#define FAT12_MAX 4085
#define FAT16_MAX 65525
#define ENTRY_SZ 32   // 32 bits
#define ENTRY16_BYTS 2
#define ENTRY32_BYTS 4
#define DIR_N

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTE 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

struct fat32hdr *hdr;
void *map_disk2mem(const char *fname);
void *read_fat_bin(const char *fname);
void report_fat(char *fname);
void report_parameter(void);
//TODO: 返回类型确定一下，目前都是void.
void *access_entry4cluster(int N);

fatType fat_type(void);
int input_invalid(int argcnt, char *argvec);
u32 to_cluster(int N);


int
main(int argc, char *argv[])
{
        if (input_invalid(argc, argv))
                exit(1);


        assert(sizeof(struct fat32hdr) == 512);
        assert(sizeof(struct fat32dent) == 32);
        return 0;
}


int input_invalid(int argc, char *argv) {
        if (argc != 2) { 
                fprintf(2, "Wrong arg num. Usage: %s fs.img\n", argv[0]);
                return 1;
        }
        return 0;
}

void *map_disk2mem(const char *fname) {
        int fd = open(fname, O_RDONLY);

}

void report_fat(char *fname) {
        u16 totsec;
        if (hdr->BPB_FATSz16 != 0)  totsec = hdr->BPB_TotSec16;
        else totsec = hdr->BPB_TotSec32;
        u32 sec_per_fat;
        if (hdr->BPB_FATSz16 != 0)  sec_per_fat = hdr->BPB_FATSz16;
        else sec_per_fat = hdr->BPB_FATSz32;

        printf("%s DOS/MBR boot sector, ", fname);
        printf("code offset (TODO)\n");
        prirntf("OEM-ID \"%s\"\n", hdr->BS_OEMName);
        printf("sectors %d, \n", totsec);
        // printf("Media descriptor %x\n", mediad);
        printf("sectors/FAT %d, \n", sec_per_fat);
        printf("serial number 0x%x\n", hdr->BS_VolID);
        report_parameter();

}

void *read_fat_bin(const char *fname) {
        int fd = open(fname, O_RDONLY);
        
}

// count by Sectors
u32 fatsz(void) {
        u32 fatsize;
        if (hdr->BPB_FATSz16 != 0)  fatsize = hdr->BPB_FATSz16;
        else fatsize = hdr->BPB_FATSz32; 
        return fatsize;
}

u32 totsecs(void) {
        u32 totsecs;
        if (hdr->BPB_TotSec16 != 0) totsecs = hdr->BPB_TotSec16;
        else totsecs = hdr->BPB_TotSec32;
        return totsecs;
}
/**
 * FATType由卷的簇数量决定
 * 1. 确定根目录占用的扇区数
 * 2. 确定数据区的扇区数
 * 3. 确定簇数: 数据区大小(扇区数来表示) / 每个簇包含的扇区数
*/
fatType fat_type(void) {
        // **NOTE**: fat32中, bpb_rootentent == 0
        u32 fatsize = fatsz();
        u32 totsec = totsecs();
        u32 datasec;
        u32 clusters;
        // 1
        u32 RootDirSecs = ((hdr->BPB_RootEntCnt * 32) + (hdr->BPB_BytsPerSec - 1)) / hdr-> BPB_BytsPerSec;
        // 2
        datasec = totsec - (hdr->BPB_RsvdSecCnt + 
                        (hdr->BPB_NumFATs * fatsize) + 
                        RootDirSecs);
        // 3
        clusters = datasec / hdr->BPB_SecPerClus; 

        if (clusters < FAT12_MAX) {
                return FAT12;
        } else if (clusters < FAT16_MAX){
                return FAT16;
        } else {
                return FAT;
        }
}


void  report_parameter(void) {
        u32 FatStartSector = hdr->BPB_RsvdSecCnt;
        u32 fatsize = fatsz();
        u32 FatSectors = fatsize * hdr->BPB_NumFATs; 
        printf("FatStartSector: %s, \n", FatStartSector);
        printf("FatSectorsCnt: %s", FatSectors); 

        u32 RootDirStartSector = FatStartSector + FatSectors;
        u32 RootDirSectors = (ENTRY_SZ * hdr->BPB_RootEntCnt + hdr->BPB_BytsPerSec - 1) / hdr->BPB_BytsPerSec;
        u32 totsec = totsecs();
        u32 DataStartSector = RootDirStartSector = RootDirSectors;
        u32 DataSectors = totsec - DataStartSector;
        printf("DataStartSector: %s, \n", DataStartSector);
        printf("DataSectors: %s", DataSectors);
}


// given a valid cluster integer N, we will determine the section.
// (Section number and it's offset)
// 不考虑FAT12的情况
// 默认在第一个FAT中找
void *access_entry4cluster(int N) {

        u32 fatsize = fatsz();
        u16 fatByteOffset; 
        if (fat_Type() == FAT16) fatByteOffset = N * ENTRY16_BYTS;
        else fatByteOffset = N * ENTRY32_BYTS;

        u16 thisFatsEntSec = hdr->BPB_RsvdSecCnt + (fatByteOffset / hdr->BPB_BytsPerSec);
        u16 thisFatEntSecOffset = (fatByteOffset) % hdr->BPB_BytsPerSec; 
}


void *cluster2datasec(int N) {
        // init: dataStartSecton
        u32 dataSec = hdr->BPB_RsvdSecCnt +  (hdr->BPB_NumFATs) * fatsz();
        // firstSecOfCluster = DataStartSec + (N-2) * BPB_SecPerCluster;
        dataSec += (N - 2) *hdr->BPB_SecPerClus;
        // NOTICE : u8 可能要换成char
        return ((u8 *)hdr + dataSec * hdr->BPB_BytsPerSec);
}

u32 to_cluster(int N) {
        u32 *clusters =
                (u32 *)
                ((u8 *)hdr + hdr->BPB_RsvdSecCnt * hdr->BPB_BytsPerSec);
        return clusters[N];
}