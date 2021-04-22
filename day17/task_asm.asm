  [BITS 32]

  GLOBAL load_tr
  GLOBAL far_jmp

load_tr:            ; void load_tr(int tr);
  LTR   [ESP+4]     ; tr
  RET

far_jmp:            ; void far_jmp(int eip, int cs);
  JMP   FAR [ESP+4]
  RET
