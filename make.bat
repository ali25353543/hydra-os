@echo off
rem this file for windows only
rem 1- download this project
rem 2- unzip this project in a folder (for example: C:\hydra-os-main)
rem 3- install:
rem QEMU URL:https://qemu.weilnetz.de/w64/ (for x64 windows) or https://qemu.weilnetz.de/w32/ (for x86 windows)
rem NASM URL:https://https://www.nasm.us/pub/nasm/releasebuilds/3.01/win64/ (for x64 windows) or https://https://www.nasm.us/pub/nasm/releasebuilds/3.01/win32/ (for x86 windows)
rem also,you'll need folder named "elftools" and "cdrtools-1.11a12-win32-bin".it is important to build hydraOS
rem put the folder in simple path.for example: C:\hydra-os-main
rem put NASM in same folder (C:\hydra-os-main\NASM)
rem put NASM in same folder (C:\hydra-os-main\qemu)
rem 4- adding tools to PATH
set /p d= < times.txt
set e=%d%
if %e% == 1 (
setx PATH "%PATH%;C:\hydra-os-main\NASM"
setx PATH "%PATH%;C:\hydra-os-main\qemu"
setx PATH "%PATH%;C:\hydra-os-main\elftools"
setx PATH "%PATH%;C:\hydra-os-main\cdrtools-1.11a12-win32-bin"
if %1 == -showlog (
nasm -f elf gdt_asm.asm -o gdt_asm.o
nasm -f elf idt_asm.asm -o idt_asm.o
nasm -f elf io_asm.asm -o io_asm.o
nasm -f elf loader.asm -o loader.o
nasm -f elf start.asm -o start.o
:A
nasm -f bin boot.asm -o boot.bin
if not exist "boot.bin" (
   code boot.asm
   goto :A
)
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c fb.c -o fb.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c gdt.c -o gdt.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c idt.c -o idt.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c keyboard.c -o keyboard.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c kmain.c -o kmain.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c serial.c -o serial.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c shell.c -o shell.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c snake.c -o snake.o
i686-elf-ld -T link.ld -m elf_i386 loader.o kmain.o io_asm.o fb.o serial.o gdt.o gdt_asm.o idt.o idt_asm.o keyboard.o shell.o snake.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
copy /b boot.bin + kernel.bin os.img
qemu-system-x86_64 -drive format=raw,file=os.img -S -s -d cpu_reset,int -no-shutdown -no-reboot -m 64m
del /s /q *.o
del /s /q *.bin
pause
) else if %1 == -hidelog (
nasm -f elf gdt_asm.asm -o gdt_asm.o
nasm -f elf idt_asm.asm -o idt_asm.o
nasm -f elf io_asm.asm -o io_asm.o
nasm -f elf loader.asm -o loader.o
nasm -f elf start.asm -o start.o
:B
nasm -f bin boot.asm -o boot.bin
if not exist "boot.bin" (
   code boot.asm
   goto :B
)
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c fb.c -o fb.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c gdt.c -o gdt.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c idt.c -o idt.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c keyboard.c -o keyboard.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c kmain.c -o kmain.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c serial.c -o serial.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c shell.c -o shell.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c snake.c -o snake.o
cls
i686-elf-ld -T link.ld -m elf_i386 loader.o kmain.o io_asm.o fb.o serial.o gdt.o gdt_asm.o idt.o idt_asm.o keyboard.o shell.o snake.o -o kernel.elf
cls
i686-elf-objcopy -O binary kernel.elf kernel.bin
cls
copy /b boot.bin + kernel.bin os.img
cls
qemu-system-x86_64 -drive format=raw,file=os.img -m 64m
cls
del /s /q *.o
cls
del /s /q *.bin
cls
pause
) else if %1 == -help (
echo make help:
echo these commands only in this makefile
echo commands:
echo -help    : show help
echo -showlog : show log
echo -hidelog : hide log
)
set /a e += 1
echo %e% > times.txt
) else (
   if %1 == -showlog (
nasm -f elf gdt_asm.asm -o gdt_asm.o
nasm -f elf idt_asm.asm -o idt_asm.o
nasm -f elf io_asm.asm -o io_asm.o
nasm -f elf loader.asm -o loader.o
nasm -f elf start.asm -o start.o
:A
nasm -f bin boot.asm -o boot.bin
if not exist "boot.bin" (
   code boot.asm
   goto :A
)
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c fb.c -o fb.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c gdt.c -o gdt.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c idt.c -o idt.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c keyboard.c -o keyboard.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c kmain.c -o kmain.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c serial.c -o serial.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c shell.c -o shell.o
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c snake.c -o snake.o
i686-elf-ld -T link.ld -m elf_i386 loader.o kmain.o io_asm.o fb.o serial.o gdt.o gdt_asm.o idt.o idt_asm.o keyboard.o shell.o snake.o -o kernel.elf
i686-elf-objcopy -O binary kernel.elf kernel.bin
copy /b boot.bin + kernel.bin os.img
qemu-system-x86_64 -drive format=raw,file=os.img -S -s -d cpu_reset,int -no-shutdown -no-reboot -m 64m
del /s /q *.o
del /s /q *.bin
pause
) else if %1 == -hidelog (
nasm -f elf gdt_asm.asm -o gdt_asm.o
nasm -f elf idt_asm.asm -o idt_asm.o
nasm -f elf io_asm.asm -o io_asm.o
nasm -f elf loader.asm -o loader.o
nasm -f elf start.asm -o start.o
:B
nasm -f bin boot.asm -o boot.bin
if not exist "boot.bin" (
   code boot.asm
   goto :B
)
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c fb.c -o fb.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c gdt.c -o gdt.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c idt.c -o idt.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c keyboard.c -o keyboard.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c kmain.c -o kmain.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c serial.c -o serial.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c shell.c -o shell.o
cls
i686-elf-gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c snake.c -o snake.o
cls
i686-elf-ld -T link.ld -m elf_i386 loader.o kmain.o io_asm.o fb.o serial.o gdt.o gdt_asm.o idt.o idt_asm.o keyboard.o shell.o snake.o -o kernel.elf
cls
i686-elf-objcopy -O binary kernel.elf kernel.bin
cls
copy /b boot.bin + kernel.bin os.img
cls
qemu-system-x86_64 -drive format=raw,file=os.img -m 64m
cls
del /s /q *.o
cls
del /s /q *.bin
cls
pause
) else if %1 == -help (
echo make help:
echo these commands only in this makefile
echo commands:
echo -help    : show help
echo -showlog : show log
echo -hidelog : hide log
)
)
