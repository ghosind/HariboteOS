  [BITS 32]

  GLOBAL load_gdtr, load_idtr

  [SECTION .text]
load_gdtr:               ; void load_gdt(int limit, int addr);
  MOV   AX, [ESP+4]     ; limit
  MOV   [ESP+6], AX
  LGDT  [ESP+6]
  RET

load_idtr:               ; void load_idt(int limit, int addr);
  MOV   AX, [ESP+4]     ; limit
  MOV   [ESP+6], AX
  LIDT  [ESP+6]
  RET
