  [BITS 32]

  GLOBAL load_tr, task_switch3, task_switch4

load_tr:            ; void load_tr(int tr);
  LTR   [ESP+4]     ; tr
  RET

task_switch3:       ; void task_switch3(void);
  JMP   3*8:0
  RET

task_switch4:       ; void task_switch4(void);
  JMP   4*8:0
  RET
