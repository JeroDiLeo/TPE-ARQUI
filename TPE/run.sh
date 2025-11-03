#!/bin/bash
# qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512  
# test_sound

#caso linux (sega)
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2  -m 512 -audiodev sdl,id=audio0 -machine pcspk-audiodev=audio0


#jero se quedo in TPE

#caso mac (pelu)
#qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

