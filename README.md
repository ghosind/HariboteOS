# HariboteOS

《30天自制操作系统》（川合秀实 著）配套代码改编版。因为作者使用了其自己编写的一套非标准的工具，且无法在非Windows平台下运行，所以本项目在原代码的基础上使用NASM、GCC、Qemu等工具进行编译、运行。

## 项目特色

- 采用GCC、NASM作为工具链，可跨平台使用
- 基于LinkerScript构建内核
- 不依赖于自定义HRB文件格式
- 支持ELF文件
- 支持部分C标准库函数
- 头文件拆分，结构更为清晰
- 中文键盘支持

## 环境依赖

- `nasm`
- `gcc`
- `binutils`
- `qemu-system-i386`
- `mtools`

### 可选依赖

- `gdb`（调试使用）

### Mac下安装

MacOS下默认Xcode Command Line Tools使用的gcc为clang的别名，在本项目实现中未对clang进行测试，可能会存在问题。

在Mac下推荐使用`x86_64-elf-gcc`工具链进行编译，可使用`brew install x86_64-elf-gcc x86_64-elf-binutils x86_64-elf-gdb`安装。

### 运行

进入对应的文件夹，使用下面的命令运行：

```bash
$ make qemu
# 若使用工具链包含前缀，如x86_64-elf-，可以使用GCCPREFIX
$ make qemu GCCPREFIX=x86_64-elf-
```

若要使用GDB进行调试，请设置`DEBUG`：

```bash
$ make qemu DEBUG=1
```

## 项目进度

- [X] 第1天：Hello world
- [X] 第2天：汇编与Makefile
- [X] 第3天：进入32位模式
- [X] 第4天：画面显示
- [X] 第5天：GDT/IDT
- [X] 第6天：中断处理
- [X] 第7天：FIFO
- [X] 第8天：鼠标与32位模式
- [X] 第9天：内存管理
- [X] 第10天：窗口叠加
- [X] 第11天：窗口处理
- [X] 第12天：定时器1
- [X] 第13天：定时器2
- [X] 第14天：键盘输入
- [X] 第15天：多任务1
- [X] 第16天：多任务2
- [X] 第17天：命令行窗口
- [X] 第18天：命令行命令
- [X] 第19天：应用程序
- [X] 第20天：API
- [X] 第21天：保护操作系统
- [X] 第22天：C语言应用程序（修改为ELF格式）
- [X] 第23天：应用程序图形处理
- [X] 第24天：窗口操作
- [X] 第25天：更多窗口
- [ ] 第26天
- [ ] 第27天
- [ ] 第28天
- [ ] 第29天
- [ ] 第30天

## 博客文章

- [第0天：前言](https://www.ghosind.com/2021/03/31/hariboteos-0)
- [第1天：Hello world](https://www.ghosind.com/2021/04/28/hariboteos-1)

## 版权信息

项目中代码版权主要归原书作者所有，本人仅针对无法直接运行的部分对该代码作出部分修改。
