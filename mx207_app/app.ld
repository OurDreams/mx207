/* Entry Point */
ENTRY(resetRoutine)

cstack_top = 0x20020000;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00008000, LENGTH = 496K
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}
    
    
SECTIONS
{
    .text :
    {   
        . = ALIGN(4);
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
       . = ALIGN(4);
        *(.rodata*)
       . = ALIGN(4);
        __shell_cmd_start = .;
        KEEP(*(.shell_cmd)) 
        __shell_cmd_end = .;
       . = ALIGN(4);
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
    _sidata = _etext;
    .data : AT(_sidata)
    {
        . = ALIGN(4);
        _data = .;
        *(vtable)
        . = ALIGN(4);
        *(.data*)
        . = ALIGN(4);
        _edata = .;
        . = ALIGN(4);
    } > SRAM
    
    
    .bss :
    {
        _bss = .;
        . = ALIGN(4);
        *(.bss)
        . = ALIGN(4);
        *(.bss*)
        . = ALIGN(4);
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM
    
	 heap_low = .; /* for _sbrk */
}
