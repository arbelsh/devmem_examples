#!/usr/bin/python3

import mmap
import os
from struct import pack,unpack

def mapMem(uiOffset, uiLen):
    f = os.open('/dev/mem', os.O_RDWR)
    pagesize = os.sysconf(os.sysconf_names['SC_PAGESIZE'])
    pagebase = (uiOffset // pagesize) * pagesize
    pageoffset = uiOffset - pagebase
    length = uiLen + pageoffset
    
    pmem = mmap.mmap(f, length, flags=mmap.MAP_SHARED, prot=(mmap.PROT_READ | mmap.PROT_WRITE), offset=pagebase)
    os.close(f)
    return pmem,pageoffset

def getUInt(pMem, uiOffset):
    return unpack('<I', pMem[uiOffset:uiOffset+4])

def setUInt(pMem, uiOffset, val):
    pMem.seek(uiOffset)
    pMem.write(pack('<I', val))

length = 4 * 1024;
mem,off = mapMem(0xFE00B000, length)
setUInt(mem, off+0x400, 8000)
setUInt(mem, off+0x408, 0x003200c0)
print(getUInt(mem, off+0x404)[0])
print(getUInt(mem, off+0x404)[0])
print(getUInt(mem, off+0x404)[0])
setUInt(mem, off+0x408, 0x00320020)
print(getUInt(mem, off+0x404)[0])
print(getUInt(mem, off+0x404)[0])
print(getUInt(mem, off+0x404)[0])
mem.close()