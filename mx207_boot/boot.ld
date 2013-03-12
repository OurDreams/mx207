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
       
    } > FLASH

    .ARM.extab :
    {
    	. = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH

    .ARM.exidx :
    {
    	. = ALIGN(4);
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    	. = ALIGN(4);
        __exidx_end = .;
    } > FLASH
    
    . = ALIGN(4);
    _etext = .;
    
    .data : AT(_etext)
    {
        . = ALIGN(4);
        _data = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM
    
    .bss :
    {
        . = ALIGN(4);
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
    
    .stackarea :
    {
         . = ALIGN(4);
         _stackarea = .;
         . = ALIGN(4);
    } > SRAM
}
