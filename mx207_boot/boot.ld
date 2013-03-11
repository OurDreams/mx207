MEMORY
{
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K    
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
       
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    . = ALIGN(4);
    .data : AT(_etext)
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM
    
    
    .bss(NOLOAD) :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)
        *(COMMON) 
        _ebss = .;
    } > SRAM
    
    .stackarea(NOLOAD):
    {
         . = ALIGN(8);
         _stackarea = .;
         . = ALIGN(8);
    } > SRAM
}
