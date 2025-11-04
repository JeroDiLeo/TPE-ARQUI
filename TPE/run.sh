#!/bin/bash
# Script para ejecutar QuantumOS en QEMU
# Grupo 12 - ITBA 2025

# Configuración para Linux (SDL audio)
qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev sdl,id=audio0 -machine pcspk-audiodev=audio0

# Para macOS, descomentar la siguiente línea y comentar la anterior:
# qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512 -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0

