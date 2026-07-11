/* MPS2 AN505 memory map (QEMU mps2-an505) */
ENTRY(Reset_Handler)

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 512K
  /* AN505 SSRAM0: keep within first 32 KiB (verified stack @ 0x30008000). */
  RAM   (rwx) : ORIGIN = 0x30000000, LENGTH = 32K
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

SECTIONS
{
  .text : {
    KEEP(*(.vectors))
    *(.text*)
    *(.rodata*)
  } > FLASH

  .init_array : {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP(*(.init_array .ctors))
    PROVIDE_HIDDEN(__init_array_end = .);
  } > FLASH

  . = ALIGN(4);
  _sidata = .;

  .data : AT(_sidata) {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  .bss (NOLOAD) : {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > RAM
}
