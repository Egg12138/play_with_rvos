/*
 * 定义一下内存布局等信息，手写个链接
 *
 */

/* 定义 target 架构*/
OUTPUT_ARCH( "riscv" )
/* OUTPUT_ARCH只描述了输出target的架构，在编译的时候我们才会确定具体是哪一个rv版本*/

/* 运行程序的第一条指令的入口 */
ENTRY( _start )

/* MEMORY 描述target中的内存布局, 一行定义一个区域，ram, rom都是区域，
 * 我们在这里只定义RAM 
 * Writable, eXecutable, Allocatable, !Readonly, Initialized
 * ram的attri: wax!ri,  ORIGIN起始:0x80000000, 长度128M
 */
MEMORY
{
    ram  (wax!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    /*下面的星号会作为通配符*/
    .text : {
        PROVIDE(_text_start = .);
        *(.text .text.*)
        PROVIDE(_text_end = .);
    } > ram

    .rodata : {
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        PROVIDE(_rodata_end = .);
    } > ram

    .data : {
        . = ALIGN(4096);
        PROVIDE(_data_start = .);
        *(.sdata, .sdata.*)
        *(.data .data.*)
        PROVIDE(_data_end = .);
    } > ram

    .bss : {
        PROVIDE(_bss_start = .);
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } > ram

    PROVIDE(_memory_start = ORIGIN(ram));
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
    PROVIDE(_heap_start = _bss_end);/*Head在bss结束开始（自下向上生长）*/
    PROVIDE(_heap_size = _memory_end - _heap_start);
}

