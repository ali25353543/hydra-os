rem this file for windows only
rem 1- download this project
rem 2- unzip this project in a folder (for example: C:\hydra-os-main)
rem 3- install:
rem QEMU URL:https://www.qemu.org
rem NASM URL:https://https://www.nasm.us/pub/nasm/releasebuilds/3.01/win64/ (for x64 windows) or https://https://www.nasm.us/pub/nasm/releasebuilds/3.01/win32/ (for x86 windows)
rem also,you'll need folder named "elftools".it is important to build hydraOS
rem put the folder in simple path.for example: C:\hydra-os-main
rem put NASM in same folder (C:\hydra-os-main\NASM)
rem put NASM in same folder (C:\hydra-os-main\qemu)
rem 4- adding tools to PATH
setx PATH "%PATH%;C:\hydra-os-main\NASM"
setx PATH "%PATH%;C:\hydra-os-main\qemu"
setx PATH "%PATH%;C:\hydra-os-main\elftools"
rem 
rem 
rem 
rem 
rem 
rem 
rem 
rem 
rem 
rem 
