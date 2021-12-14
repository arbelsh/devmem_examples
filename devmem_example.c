#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

uint8_t* mapMem(off_t uiOffset, size_t* uiLen)
{
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    off_t pageBase = (uiOffset / pageSize) * pageSize; // align offset to memory page
    off_t pageOffset = uiOffset - pageBase;

    int fd = open("/dev/mem", O_RDWR);  // get a file descriptor to the device
    
    *uiLen += pageOffset;  // adjust len to fit offset
    uint8_t* pMem = (uint8_t*)mmap(NULL, *uiLen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pageBase); // do the actual mapping 
    close(fd);  // after the mapping we don't need the file desscriptor anymore

    if(pMem == MAP_FAILED)
    {
        perror("failed to map memory");
        return NULL;
    }

    return &pMem[pageOffset];
}

void unmapMem(uint8_t* pMem, size_t uiLen)
{
    size_t pageSize = sysconf(_SC_PAGE_SIZE);
    uint8_t* aligned = (uint8_t*)((size_t)pMem & ~(pageSize-1));
    munmap(aligned, uiLen);
}

uint32_t getUInt(uint8_t* pMem, off_t uiOffset)
{
    return *(uint32_t*)&pMem[uiOffset];
}

void setUInt(uint8_t* pMem, off_t uiOffset, uint32_t val)
{
    *(uint32_t*)&pMem[uiOffset] = val;
}

int main(int argc, char* argv[])
{
    // see https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf
    size_t len = 4 * 1024;
    uint8_t* mem = mapMem(0xFE00B000, &len); // map ARM timer registers memory

    if(mem)
    {
        setUInt(mem, 0x400, 8000);           // init timer value
        setUInt(mem, 0x408, 0x003200c0);     // start the timer counter
        printf("%d\n", getUInt(mem, 0x404)); // read current value
        printf("%d\n", getUInt(mem, 0x404));
        printf("%d\n", getUInt(mem, 0x404));
        setUInt(mem, 0x408, 0x00320020);     // stop the timer counter
        printf("%d\n", getUInt(mem, 0x404));
        printf("%d\n", getUInt(mem, 0x404));
        printf("%d\n", getUInt(mem, 0x404));
        unmapMem(mem, len);                  // unmap registers memory
    }

    return 0;
}
