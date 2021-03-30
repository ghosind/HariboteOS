ENTRY( main )

MEMORY
{
  rom : ORIGIN = 0x000000, LENGTH = 0x280000
  ram : ORIGIN = 0x280000, LENGTH = 0x020000
}

SECTIONS
{
  . = 0x0000;
  .text :
  {
    *(.text.main);
    *(.text);
  } > rom AT > ram

  .rodata :
  {
    *(.rodata)
  } > ram

  .data : 
  {
    *(.data)
  } > ram

  .bss : 
  {
    *(.bss)
  } > ram
}
