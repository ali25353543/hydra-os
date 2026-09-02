#!/bin/bash

function OSinstall() {
    make clean
    make
    if [ -f kernel.elf ]; then
        if [ -f *.bin ] ; then
            if [ -f users.txt ]; then
                (
                    echo "### Hydra OS."
                    echo "if [ -f kernel.elf ]; then"
                    echo "if [ -f *.bin ] ; then"
                    echo "if [ -f users.txt ]; then"
                    echo "menuentry Hydra OS {"
                    echo "  multiboot /boot/kernel.elf"
                    for i in $(ls *.bin); do
                    echo "  module /modules/$i"
                    done
                    echo "  module /modules/users.txt"
                    echo "}"
                    echo "fi"
                    echo "fi"
                    echo "fi"
                ) >> /etc/grub.d/40_custom
                if [ -d /modules ]; then
                    mv ./kernel.elf /boot/ 
                    cp ./users.txt /modules/
                    mv ./*.bin /modules/
                else
                    mkdir /modules
                    cp ./users.txt /modules/
                    cp ./*.bin /modules/
                fi
                update-grub
                make clean
                exit 0
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
    OSinstall
fi

exit 0