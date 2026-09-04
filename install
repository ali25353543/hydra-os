#!/bin/bash

function OSinstall() {
    make clean
    make
    
    if [ ! -f kernel.elf ]; then
        echo "Error: kernel.elf not found!"
        exit 1
    fi
    
    # التأكد من وجود ملفات bin و users.txt
    if [ ! -f *.bin ] 2>/dev/null; then
        echo "Error: No .bin files found!"
        exit 1
    fi
    
    if [ ! -f users.txt ]; then
        echo "Creating default users.txt..."
        echo "user=root" > users.txt
        echo "password=root" >> users.txt
    fi
    
    # التثبيت على النظام الحالي أو على جهاز مختلف
    if [ "$1" == "" ]; then
        # تثبيت على النظام الحالي
        cat /etc/grub.d/40_custom | sed '/### Hydra OS./d' | sed '/menuentry Hydra-OS {/d' | \
        sed '/multiboot \/boot\/kernel.elf/d' | sed '/module \/modules\//d' | sed '/}/d' > /tmp/40_custom_temp
        cat /tmp/40_custom_temp > /etc/grub.d/40_custom
        
        {
            echo "### Hydra OS."
            echo "menuentry Hydra-OS {"
            echo "  multiboot /boot/kernel.elf"
            for i in $(ls *.bin); do
                echo "  module /modules/$i"
            done
            echo "  module /modules/users.txt"
            echo "}"
        } >> /etc/grub.d/40_custom
        
        mv ./kernel.elf /boot/
        mkdir -p /modules
        cp ./users.txt /modules/
        cp ./*.bin /modules/
        update-grub
        
    else
        # تثبيت على جهاز آخر
        mkdir -p "$1"/boot
        mkdir -p "$1"/modules
        mkdir -p "$1"/boot/grub
        
        {
            echo "### Hydra OS."
            echo "menuentry Hydra-OS {"
            echo "  multiboot /boot/kernel.elf"
            for i in $(ls *.bin); do
                echo "  module /modules/$i"
            done
            echo "  module /modules/users.txt"
            echo "}"
        } >> "$1"/boot/grub/grub.cfg
        
        cp ./users.txt "$1"/modules/
        cp ./*.bin "$1"/modules/
        cp ./kernel.elf "$1"/boot/
    fi
    
    make clean
    echo "Installation Complete."
    
    if [ "$1" == "" ]; then
        echo -n "Reboot(y/n)? "
        read choice
        if [[ "$choice" == "y" || "$choice" == "Y" ]]; then
            reboot
        elif [[ "$choice" == "n" || "$choice" == "N" ]]; then
            exit 0
        else
            echo "Invalid Choice."
            echo "The System has been installed."
            exit 1
        fi
    fi
}

# التحقق من صلاحيات الجذر
if [ "$(whoami)" != "root" ]; then
    sudo $0 "$@"
    exit $?
fi

# معالجة الوسائط
if [ "$1" == "--device" ] && [ -n "$2" ]; then
    echo "Installing on device: /dev/$2"
    
    # إنشاء نقطة التحميل
    MOUNT_POINT="/mnt/hydra-os"
    mkdir -p "$MOUNT_POINT"
    
    # إلغاء التحميل إذا كان مثبتاً
    umount -l "/dev/$2" 2>/dev/null
    
    # تحميل الجهاز
    mount "/dev/$2" "$MOUNT_POINT"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to mount /dev/$2"
        exit 1
    fi
    
    # التحقق من وجود GRUB
    if find "$MOUNT_POINT" -iname 'grub.cfg' | grep -q .; then
        echo "GRUB found. Installing Hydra OS..."
        OSinstall "$MOUNT_POINT"
    else
        echo "GRUB is not installed on this drive."
        echo "Installing GRUB..."
        grub-install --boot-directory="$MOUNT_POINT" --force "/dev/$2"
        if [ $? -eq 0 ]; then
            echo "GRUB installed successfully."
            OSinstall "$MOUNT_POINT"
        else
            echo "Error: Failed to install GRUB"
            exit 1
        fi
    fi
    
    # إلغاء تحميل الجهاز بعد الانتهاء
    umount "$MOUNT_POINT"
else
    # التثبيت على النظام الحالي
    OSinstall
fi