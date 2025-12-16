## This is: 
Demonstration of some unexpected behaviour with KOS and purupuru packs (only tested with original SEGA packs).

## Building
This demo relies on the enDjinn framework which is integrated here as a submodule, the complete clone and building process becomes:

```sh
git clone git@github.com:dfchil/purupuru_demo.git
cd purupuru_demo
git submodule init
git submodule update  
make
```

## Problem that needs fixing
Sometimes when sending purupuru commands, it results in the controller being unreadable/unavailable for the following 4 frames.

## How does this work
All the interesting stuff is in the [./code/game/core.c](code/game/core.c) file

This demo tries to show this by registering which controllers are present at program start and sending a puru-puru command to available packs every 31th frame. 
All controllers are scanned for availability every frame, and if a controller that was registered at program start becomes unavailable, the port number and frame number is printet to stdout. 

In my experience the error isn't always triggered, but when it is, the unavailability of the controller lasts for 4 frames. 

## example output 
```sh
dc-tool-ip: Starting a GDB server on port 2159
dc-tool-ip: Console enabled
dc-tool-ip: Mapping /pc/ to </home/drxl/projects/dreamcast/purupuru_demo/cdrom>
dc-tool-ip: Upload </home/drxl/projects/dreamcast/purupuru_demo/purudemo.elf>
dc-tool-ip: File format is ELF, start address is 0x8c010000
dc-tool-ip: Section .text, lma 0x8c010000, size 260400
dc-tool-ip: dcload-ip 2.0.1 using Broadband Adapter (HIT-0400)
dc-tool-ip: Section .init, lma 0x8c04f930, size 54
dc-tool-ip: Section .fini, lma 0x8c04f968, size 42
dc-tool-ip: Section .rodata, lma 0x8c04f994, size 45688
dc-tool-ip: Section .eh_frame, lma 0x8c05ac0c, size 360
dc-tool-ip: Section .ctors, lma 0x8c05adf4, size 12
dc-tool-ip: Section .dtors, lma 0x8c05ae00, size 8
dc-tool-ip: Section .data, lma 0x8c05ae20, size 13184
dc-tool-ip: Section .got, lma 0x8c05e1a0, size 12
dc-tool-ip: Transferred 319760 bytes at 122885344.240425 bytes / sec
dc-tool-ip: Executing at <0xac010000>
dc-tool-ip: Sending execute command (0xac010000, console=1, cdfsredir=0)...executing
dc-tool-ip: 
dc-tool-ip: --
dc-tool-ip: KallistiOS v2.2.1
dc-tool-ip:   Git revision: ad131480
dc-tool-ip:   Tue Sep  2 01:40:08 PM CEST 2025
dc-tool-ip:   drxl@ripper:/opt/toolchains/dc/kos
dc-tool-ip:   sh-elf-gcc (GCC) 15.2.0
=thread-group-added,id="i1"
GNU gdb (GDB) 16.3
Copyright (C) 2024 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.
Type "show copying" and "show warranty" for details.
This GDB was configured as "--host=x86_64-pc-linux-gnu --target=sh-elf".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<https://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
    <http://www.gnu.org/software/gdb/documentation/>.

For help, type "help".
Type "apropos word" to search for commands related to "word".
Warning: Debuggee TargetArchitecture not detected, assuming x86_64.
=cmd-param-changed,param="pagination",value="off"
dc-tool-ip: maple: active drivers:
dc-tool-ip:     Dreameye (Camera): Camera
dc-tool-ip:     Sound Input Peripheral: Microphone
dc-tool-ip:     PuruPuru (Vibration) Pack: JumpPack
dc-tool-ip:     VMU Driver: Clock, LCD, MemoryCard
dc-tool-ip:     Mouse Driver: Mouse
dc-tool-ip:     Keyboard Driver: Keyboard
dc-tool-ip:     Controller Driver: Controller
dc-tool-ip:     Lightgun: LightGun
dc-tool-ip:   DMA Buffer at ac06b220
dc-tool-ip: vid_set_mode: 640x480 VGA with 1 framebuffers.
dc-tool-ip: dc-load console support enabled
dc-tool-ip: maple: attached devices:
dc-tool-ip:   A0: Dreamcast Controller           (01000000: Controller)
dc-tool-ip:   A1: Visual Memory                  (0e000000: Clock, LCD, MemoryCard)
dc-tool-ip:   A2: Puru Puru Pack                 (00010000: JumpPack)
dc-tool-ip: 
dc-tool-ip: waiting for gdb client connection...
gdb_init () at gdb_stub.c:949
949	    BREAKPOINT();
dc-tool-ip: vid_set_mode: 640x480 VGA with 1 framebuffers.
dc-tool-ip: Expecting controllers on ports: A
dc-tool-ip: Warning: Expected controller A but not found in frame 33!
dc-tool-ip: Warning: Expected controller A but not found in frame 34!
dc-tool-ip: Warning: Expected controller A but not found in frame 35!
dc-tool-ip: Warning: Expected controller A but not found in frame 36!
dc-tool-ip: Warning: Expected controller A but not found in frame 157!
dc-tool-ip: Warning: Expected controller A but not found in frame 158!
dc-tool-ip: Warning: Expected controller A but not found in frame 159!
dc-tool-ip: Warning: Expected controller A but not found in frame 160!
dc-tool-ip: Warning: Expected controller A but not found in frame 281!
dc-tool-ip: Warning: Expected controller A but not found in frame 282!
dc-tool-ip: Warning: Expected controller A but not found in frame 283!
dc-tool-ip: Warning: Expected controller A but not found in frame 284!
dc-tool-ip: Warning: Expected controller A but not found in frame 405!
dc-tool-ip: Warning: Expected controller A but not found in frame 406!
dc-tool-ip: Warning: Expected controller A but not found in frame 407!
dc-tool-ip: Warning: Expected controller A but not found in frame 408!
dc-tool-ip: Warning: Expected controller A but not found in frame 529!
dc-tool-ip: Warning: Expected controller A but not found in frame 530!
dc-tool-ip: Warning: Expected controller A but not found in frame 531!
dc-tool-ip: Warning: Expected controller A but not found in frame 532!
dc-tool-ip:
dc-tool-ip: arch: exit return code 0
dc-tool-ip: arch: shutting down kernel
dc-tool-ip: vid_set_mode: 640x480 VGA with 1 framebuffers.
ERROR: GDB exited unexpectedly with exit code 0 (0x0). Debugging will now abort.
The program '/home/drxl/projects/dreamcast/purupuru_demo/purudemo.elf' has exited with code -1 (0xffffffff).

```
