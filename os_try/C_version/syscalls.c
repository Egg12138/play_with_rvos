#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define SZ_B(f) (lseek((f),0,SEEK_END))
#define SECS2BYTS(s,bps) (size_t)(((s)-1)*(bps)+(s)%(bps)) //Sector, BytesPerSec
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
void *map_file2mem(char *fname);
int test_lseek(char *fname);
void test_mmap(char *fname);
int
main(int argc, char *argv[])
{
        if (argc < 2) {
                fprintf(stderr,"Usage: %d offset\n", argc);
                // exit(-1);
        }
        test_lseek(argv[1]);
        int secs = 1024;
        int bytspersec = 512;
        long bytes = SECS2BYTS(secs,bytspersec); 
        printf("bytes => %ld", bytes);
        test_mmap(argv[1]);
        
        return 0;
}

/*
 出来的数是字节数,
 执行过lseek后，游标一定是会移动

*/
int test_lseek(char *fname) {
        int fd = open(fname, O_RDWR);

        if (fd < 0) {
                fprintf(stderr, "fd error\n, fd = %d", fd);
                exit(1);
        }
        off_t cur_position, end_position, head_position; 
        off_t offset = 29144;
        end_position = lseek(fd, 0, SEEK_END);
        cur_position = lseek(fd, 0, SEEK_CUR);
        printf("fp end: %ld, ", end_position);
        printf("fp cur -> %ld\n", cur_position);
        head_position = lseek(fd, 0, SEEK_SET);
        printf("fp set: %ld, ", head_position);
        cur_position = lseek(fd, 0, SEEK_CUR);
        printf("fp cur -> %ld\n", cur_position);
        cur_position = lseek(fd, offset, SEEK_END);
        printf("lseek end (purely add to result),with offset`%ld`fp cur -> %ld\n", offset, cur_position);
        cur_position = lseek(fd, offset, SEEK_CUR);
        printf("fp cur -> %ld\n", cur_position);
        cur_position = lseek(fd, 0, SEEK_CUR);
        printf("fp cur -> %ld\n", cur_position);

        size_t fileszB = SZ_B(fd);
        printf("File size = %ld bytes\n", fileszB);
        return 0;        
}

void test_mmap(char *fname) {
        // default, addr <- NULL
        int fd = open(fname, O_RDONLY);
        size_t filesz = SZ_B(fd);
        struct fat32hdr *hdr = mmap(NULL, filesz, 
                PROT_READ | PROT_WRITE, 
                MAP_PRIVATE,
                fd, 0
                ); 
        printf("\n%s,%s,%d,%d\n", 
                hdr->BS_jmpBoot, hdr->BS_OEMName, 
                hdr->BPB_BytsPerSec, hdr->BPB_SecPerClus);
        
        

}
void *map_file2mem(char *fname) {
        int fd = open(fname, O_RDONLY);

        if (fd < 0) { goto err_handler; }
        off_t size = lseek(fd, 0, SEEK_END);

err_handler:
        perror("wrong");
        if (fd > 0) close(fd);
        exit(1);
        
}