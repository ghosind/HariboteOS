# HariboteOS

《30天自制操作系统》（川合秀实 著）配套代码改编版。因为作者使用了其自己编写的一套非标准的工具，且无法在非Windows平台下运行，所以本项目在原代码的基础上使用NASM、GCC、Qemu等工具进行编译、运行。

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
- [X] 第3天
- [X] 第4天
- [X] 第5天
- [X] 第6天
- [X] 第7天
- [X] 第8天
- [X] 第9天
- [X] 第10天
- [X] 第11天
- [X] 第12天
- [X] 第13天
- [X] 第14天
- [X] 第15天
- [X] 第16天
- [X] 第17天
- [X] 第18天
- [X] 第19天
- [X] 第20天
- [X] 第21天
- [ ] 第22天
- [ ] 第23天
- [ ] 第24天
- [ ] 第25天
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
