ENTRY( entry )

SECTIONS
{
  . = 0xc200;
  .btext : { *(.btext) }

  .text : {
    *(.text.main);
    *(.text);
  }

  .data : { *(.data) }

  .rodata : { *(.rodata) }
}
