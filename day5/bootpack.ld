ENTRY( main )

SECTIONS
{
  . = 0x00;

  .text : { *(.text) }
  .data : { *(.data) }
}
