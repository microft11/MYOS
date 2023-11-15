#!/bin/bash

ASMPARAMS="--32"
LDPARAMS="-melf_i386"
GPPPARAMS="-m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore"

objects="kernel.o loader.o gdt.o port.o interrupts.o interruptstubs.o keyboard.o driver.o mouse.o multitasking.o"

# 编译.cpp文件为.o文件
compile_cpp() {
  g++ $GPPPARAMS -o $2 -c $1
}

# 编译.s文件为.o文件
assemble() {
  as $ASMPARAMS -o $2 $1
}

# 链接.o文件生成mykernel.bin
link() {
  ld $LDPARAMS -T $1 -o $2 $3
}

# 生成mykernel.ios
create_iso() {
  mkdir iso
  mkdir iso/boot
  mkdir iso/boot/grub
  cp $1 iso/boot
  echo 'set timeout = 5' >> iso/boot/grub.cfg
  echo 'set default = 0' >> iso/boot/grub.cfg
  echo '' >> iso/boot/grub.cfg
  echo 'menuentry "My Operating System ..." {' >> iso/boot/grub.cfg
  echo '  multiboot /boot/mykernel.bin' >> iso/boot/grub.cfg
  echo '  boot' >> iso/boot/grub.cfg
  echo '}' >> iso/boot/grub.cfg
  grub-mkrescue --output=$2 iso
  rm -rf iso
}

# 编译.cpp文件
for cpp_file in *.cpp; do
  if [ -f "$cpp_file" ]; then
    compile_cpp "$cpp_file" "${cpp_file%.cpp}.o"
  fi
done

# 编译.s文件
for asm_file in *.s; do
  if [ -f "$asm_file" ]; then
    assemble "$asm_file" "${asm_file%.s}.o"
  fi
done

# 链接生成mykernel.bin
link linker.ld mykernel.bin "$objects"

# 生成mykernel.ios
create_iso mykernel.bin mykernel.ios

echo "操作完成"

