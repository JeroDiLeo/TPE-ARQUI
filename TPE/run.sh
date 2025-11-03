#!/bin/bash
# qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512  
# test_sound


qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2  -m 512 -audiodev sdl,id=audio0 -machine pcspk-audiodev=audio0
# qemu-system-x86_64 -hda Image/test_sound -m 512 -audiodev sdl,id=audio0 -machine pcspk-audiodev=audio0