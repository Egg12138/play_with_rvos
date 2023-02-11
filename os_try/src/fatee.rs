#![allow(unused_variables, dead_code, non_snake_case)]
#![allow(clippy::upper_case_acronyms)]
/// 32-bit FAT entry是4个8bit字节... 按照小端序排列
/// 每个byte:
/// 低地址  =>   高地址
///  7 6 5 4 3 2 1 0
/// FATfs称呼自身为一个Logical Volume，
/// 下面类型的定义中存在一些显然**只适用于FAT12/16**的部分，没有说明则FAT32应该
/// 默认对此部分置零, 反之同理
use std::error::Error;
type Prog = u32;
type FatResult<T> = Result<T, Box<dyn Error>>;

pub enum FsType {
        FAT12,
        FAT16,
        FAT,
}
pub enum Region {
        Reserved,
        FATRegion,
        RDRegion,            // Root Dir Region
        FDDRegion,           // File and Dir Data Region
}

/// **NOTE**: 
/// BPB位于引导区(**V**olume, **B**oot, **R**ecord).
/// In the following description, all the fields whose names 
/// start with BPB_ are part of the BPB. 
/// All the fields whose names start with BS_ are part of 
/// the boot sector and not really part of the BPB.
/// The BPB in the boot sector of a FAT volume *must always have all 
/// of the BPB fields for either the FAT12/FAT16 or FAT32 BPB type**. 
/// This **ensures maximum compatibility of the FAT volume and 
/// will also ensure that all FAT file system drivers understand 
/// and support the volume correctly.
#[repr(align(4))]
pub(crate) struct BPB {
        BS_jmpBoot: [u8; 3],
        BS_OMEName: [u8; 8],
        /// Count of bytes per sector. This value may take on 
        /// only the following values: 512, 1024, 2048 or 4096.
        BPB_BytesPerSec: u16,
        /// This value 
        /// must be a power of 2 that is greater than 0. The 
        /// legal values are 1, 2, 4, 8, 16, 32, 64, and 128
        BPB_SecPerClus: u8,
        /// Reserved region的保留扇区数。
        /// 用以将数据区对齐为簇大小的整数倍的
        /// 一定有 RsvdSecCnt != 0 
        BPB_RsvdSecCnt: u16,
        /// Volume中的FATs数(参考`mkfs.fat`的默认值为2,起码要有一个备份吧！)
        /// 通常从第一个 FAT 读取 FAT 条目的值，
        /// 并且对 FAT 条目的任何更改都会重新映射到每个 FAT。
        BPB_NumFATs: u8,
        /// **仅FAT12/16**, FAT32: 这一字段为0
        /// RootEntCnt * 32 = BytsPerSec整数倍
        /// RootEntCnt ~ (16, 32, 64, 128)
        BPB_RootEntCnt: u16,
        /// BPB是整个卷配置信息，所以自然TotSec算的是整个Volume的old 16-bit
        /// 总扇区数,当FAT12/16卷的扇区数为 0x10000 或更大时，在该字段中应
        /// 设置为无效值0，并将实际值设置到 BPB_TotSec32 字段中。
        /// 对于FAT32此字段必须始终为 0
        BPB_TotSec16: u16,
        /// one of 
        /// (0xF0, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE and 0xFF.)
        BPB_Media: u8, 
        BPB_FATSz16: u16,
        BPB_SecPerTrk: u16,
        BPB_NumHeads: u16,
        /// 通常只和0x13上的可见媒体相关（来自05年的x86 based MS FAT Spec.仅供参考）
        BPB_HiddSec: u32,
        BPB_TotSec32: u32,
        BPB_FATSz32: u32,
        BPB_ExtFlags: u16,
        BPB_FSVer: u16,
        BPB_RootClus: u32,
        BPB_FSInfo: u16,
        BPB_BkBootSec: u16,
        BPB_Reserved: [u8; 12],
        /// int 0x13 , set to 0x80 / 0x00
        BS_DrvNum: u8,
        /// set to 0x0
        BS_Reserved1: u8,
        /// 若VoIID , VolLab 非零，就设置为0x29
        BS_BootSig: u8,
        /// 和VolLab,支持了可移除媒介的volume tracking , 
        BS_VolID: u32,
        BS_VolLab: [u8; 11],
        BS_FilSysType: [u8; 8],
        __padding_1: [u8; 420],
        Signature_word: u16,
}

pub(crate) struct FAT {

}

pub(crate) fn get_args() -> FatResult<Prog>{
        todo!()
}


pub(crate) fn run(prog: Prog) -> FatResult<()>{
        todo!()

}