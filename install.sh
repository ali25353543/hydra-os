#!/bin/bash

function OSinstall() {
    make clean
    make
    if [ -f kernel.elf ]; then
        if [ -f *.bin ] ; then
            if [ -f users.txt ]; then
		if [ "$1" == "" ]; then
                    cat /etc/grub.d/40_custom | grep '### Hydra OS.'
		    cat /etc/grub.d/40_custom | sed '/### Hydra OS./d' | sed '/menuentry Hydra-OS {/d' | sed '\|multiboot /boot/kernel.elf|d' | \
                    sed '\|multiboot /boot/kernel.elf|d' | sed '\|module /modules/|d' | sed '/}/d' >> /etc/grub.d/40_custom
		    (
                    	echo "### Hydra OS."
                    	echo "menuentry Hydra-OS {"
                    	echo "  multiboot /boot/kernel.elf"
                    	for i in $(ls *.bin); do
                    	    echo "  module /modules/$i"
                    	done
                    	echo "  module /modules/users.txt"
                    	echo "}"
                    ) >> /etc/grub.d/40_custom
		    mv ./kernel.elf /boot/
                    if [ -d /modules ]; then
                    	cp ./users.txt /modules/
                    	mv ./*.bin /modules/
                    else
                    	mkdir /modules
                    	cp ./users.txt /modules/
                    	cp ./*.bin /modules/
                    fi
                    update-grub
		else
		    (
                    	echo "### Hydra OS."
                    	echo "menuentry Hydra-OS {"
                    	echo "  multiboot /boot/kernel.elf"
                    	for i in $(ls *.bin); do
                    	    echo "  module /modules/$i"
                    	done
                    	echo "  module /modules/users.txt"
                    	echo "}"
                    ) >> "$1"/boot/grub/grub.cfg || \
		    (
                  	echo "### Hydra OS."
                    	echo "menuentry Hydra-OS {"
                    	echo "  multiboot /boot/kernel.elf"
                    	for i in $(ls *.bin); do
                    	    echo "  module /modules/$i"
                    	done
                    	echo "  module /modules/users.txt"
                    	echo "}"
                    ) >> "$1"/grub/grub.cfg
		    mkdir -p "$1"/boot
		    mkdir -p "$1"/modules
		    cp ./users.txt "$1"/modules
		    mv ./*.bin "$1"/modules
		    mv ./kernel.elf "$1"/boot
		fi
                make clean
                echo "Installation Compelete."
                echo -n "Reboot(y/n)? "
                read choice
                if [ $choice == "y" ] || [ $choice == "Y" ]; then
                    reboot
                elif [ $choice == "n" ] || [ $choice == "N" ]; then
                    exit 0
                else
                    echo "Invailid Choice."
                    echo "The System has been installed."
                    exit 1
                fi
            else
                echo "user=root" > users.txt
                echo "password=root" >> users.txt
                OSinstall
            fi
        else
            OSinstall
        fi
    else
        OSinstall
    fi
}

if [ "$(whoami)" != "root" ]; then
    sudo $0
else
    if [ "$1" == "" ]; then
    	OSinstall
    elif [ "$1" == "--device" ]; then
	umount -l /dev/"$2"
	if [ -d /mnt/hydra-os ]; then
	    exec
        else
	    mkdir /mnt/hydra-os
	fi
       	mount /dev/"$2" /mnt/hydra-os
	if find -iname 'grub.cfg' /mnt/hydra-os; then
	    exec
	else
		echo "GRUB is not installed in this drive."
		echo "Installing GRUB..."
		install-grub --boot-directory=/mnt/hydra-os --force "$2"
		OSinstall /mnt/hydra-os
	fi
    fi
fi
