  [BITS 32]

  GLOBAL asm_hrb_api

  EXTERN hrb_api

asm_hrb_api:
  STI
  PUSHAD
  
  PUSHAD
  CALL    hrb_api
  ADD     ESP, 32
  POPAD
  IRETD
