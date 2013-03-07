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
        *(.ARM.extab* .gnu.linkonce.armextab.*)

    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH


    . = ALIGN(4);
     _etext = .;
    /* This is used by the startup in order to initialize the .data secion */
    the_sidata = _etext;
    .data : AT(the_sidata)
    {
        the_data = .;
        *(vtable)
        *(.data*)
        the_edata = .;
    } > SRAM
    
    
    .bss :
    {
        . = ALIGN(4);
        the_bss = .;
        *(.bss*)
        *(COMMON) 
        the_ebss = .;
    } > SRAM
    
}
