; hello-os

CYLS   EQU   10               ; 读取的柱面数量（CYLS = cylinders）

  ORG   0x7c00            ; 指明程序的装载地址

; 用于标准FAT12格式的软盘
  JMP   entry             ; 跳转指令
  NOP                     ; NOP指令
  DB    "HARIBOTE"        ; OEM标识符（8字节）
  DW    512               ; 每个扇区（sector）的字节数（必须为512字节）
  DB    1                 ; 每个簇（cluster）的扇区数（必须为1个扇区）
  DW    1                 ; FAT的预留扇区数（包含boot扇区）
  DB    2                 ; FAT表的数量，通常为2
  DW    224               ; 根目录文件的最大值（一般设为224项）
  DW    2880              ; 磁盘的扇区总数，若为0则代表超过65535个扇区，需要使用22行记录
  DB    0xf0              ; 磁盘的种类（本项目中设为0xf0代表1.44MB的软盘）
  DW    9                 ; 每个FAT的长度（必须为9扇区）
  DW    18                ; 1个磁道（track）拥有的扇区数（必须是18）
  DW    2                 ; 磁头数（必须为2）
  DD    0                 ; 隐藏的扇区数
  DD    2880              ; 大容量扇区总数，若16行记录的值为0则使用本行记录扇区数
  DB    0                 ; 中断0x13的设备号
  DB    0                 ; Windows NT标识符
  DB    0x29              ; 扩展引导标识
  DD    0xffffffff        ; 卷序列号
  DB    "HARIBOTEOS "     ; 卷标（11字节）
  DB    "FAT12   "        ; 文件系统类型（8字节）
  RESB  18                ; 空18字节

; 程序核心

entry:
  MOV   AX, 0             ; 初始化寄存器
  MOV   SS, AX
  MOV   SP, 0x7c00
  MOV   DS, AX

; 读取硬盘
  MOV   AX, 0x0820
  MOV   ES, AX
  MOV   CH, 0             ; 柱面0
  MOV   DH, 0             ; 磁头0
  MOV   CL, 2             ; 扇区2

readloop:
  MOV   SI, 0             ; 记录失败次数的寄存器
retry:
  MOV   AH, 0x02          ; AH=0x02：读盘
  MOV   AL, 1             ; 1个扇区
  MOV   BX, 0
  MOV   DL, 0x00          ; A驱动器
  INT   0x13              ; 调用磁盘BIOS
  JNC   next              ; 没出错跳转到next

  ADD   SI, 1             ; 失败次数+1
  CMP   SI, 5             ; 失败次数是否达到5次
  JAE   error             ; 失败次数达到5次跳转到error
  MOV   AH, 0x00
  MOV   DL, 0x00          ; A驱动器
  INT   0x13              ; 重置驱动器
  JMP   retry

next:
  MOV   AX, ES            ; 把内存地址后移0x200
  ADD   AX, 0x0020
  MOV   ES, AX            ; 实现ES += 0x0020的目的

  ; 扇区范围 1～18
  ADD   CL, 1             ; 扇区加1
  CMP   CL, 18            ; 扇区是否达到18
  JBE   readloop          ; 小于等于18扇区则跳转到readloop

  MOV   CL, 1             ; 恢复到扇区1
  ; 磁头范围 0～1（正面0，反面1）
  ADD   DH, 1
  CMP   DH, 2
  JB    readloop          ; 磁头未达到2则跳转到readloop

  MOV   DH, 0
  ; 柱面范围 0 ～ 79
  ADD   CH, 1
  CMP   CH, CYLS
  JB    readloop          ; 读取指定数量的柱面，未达到CYLS则跳转readloop

; 读取完毕，跳转到haribote.sys执行
  MOV   [0x0ff0], CH      ; 记下IPL读了多远（谷歌翻译自IPLがどこまで読んだのかをメモ）
  JMP   0xc200

fin:
  HLT                     ; CPU停止，等待指令
  JMP   fin               ; 无限循环

error:
  MOV   SI, msg

putloop:
  MOV   AL, [SI]
  ADD   SI, 1             ; SI加1
  CMP   AL, 0

  JE    fin
  MOV   AH, 0x0e          ; 显示一个文字
  MOV   BX, 15            ; 指定字符颜色
  INT   0x10              ; 调用显卡BIOS
  JMP   putloop

msg:
  DB    0x0a, 0x0a        ; 两个换行
  DB    "load error"
  DB    0x0a              ; 换行
  DB    0

  RESB  0x1fe - ($ - $$)  ; 填写0x00，直到0x001fe
  DB    0x55, 0xaa
