  [BITS 32]

  GLOBAL asm_hrb_api
  GLOBAL api_putchar

  EXTERN hrb_api

asm_hrb_api:
  STI
  PUSHAD
  
  PUSHAD
  CALL    hrb_api
  ADD     ESP, 32
  POPAD
  IRETD
