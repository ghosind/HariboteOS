  [BITS 32]

  GLOBAL load_tr
  GLOBAL far_jmp, far_call

load_tr:            ; void load_tr(int tr);
  LTR   [ESP+4]     ; tr
  RET

far_jmp:            ; void far_jmp(int eip, int cs);
  JMP   FAR [ESP+4]
  RET

far_call:           ; void far_call(int eip, int cs);
  CALL  FAR [ESP+4] ;; eip, cs
  RET
