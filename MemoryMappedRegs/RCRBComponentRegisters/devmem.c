/*
 * devmem.c: Simple program to read/write from/to any location in memory.
 *
 *  Copyright (C) 2000, Jan-Derk Bakker (jdb@lartmaker.nl)
 *
 *
 * This software has been developed for the LART computing board
 * (http://www.lart.tudelft.nl/). The development has been sponsored by
 * the Mobile MultiMedia Communications (http://www.mmc.tudelft.nl/)
 * and Ubiquitous Communications (http://www.ubicom.tudelft.nl/)
 * projects.
 *
 *   Copyright (C) 2015, Trego Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// <time.h> requires a specific C source version (greater than)
#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <inttypes.h>
#ifdef X128
#include <emmintrin.h>
#endif
#include <time.h>
#include "configHeader.h"
#define RAW 1
#define printerr(fmt, ...)               \
    do                                   \
{                                        \
    fprintf(stderr, fmt, ##__VA_ARGS__); \
    fflush(stderr);                      \
} while (0)






int main(int argc, char** argv)
{
    if(argc!=2){
        printerr("Error: Input number not correct! Only RCRB's physical addr, for example '0xd1401000' . \n");
        return -1;
    }
    struct memAddrHeader memRegs;
    uint64_t target = -1; // store what received from cmd line
    target = strtoull(argv[1], NULL, 0);
    uint32_t sz = 0x100;
    //sz = strtoull(argv[2], NULL, 0);
    
    //printf("Hufei:target is %lu", target);
        printf("Entered CXL upstream Port's RCRB physical address is %s.\n", argv[1]);
        memRegs.upRCRB = target;
        memRegs.dpRCRB = getDpRCRB(target);
        memRegs.upComp = getComp(memRegs.upRCRB);
        memRegs.dpComp = getComp(memRegs.dpRCRB);

    if(RAW) {
        printf("\nKey Link Status:\n");
        printLnkSpeed(memRegs.upRCRB, memRegs.dpRCRB);
        printLnkWidth(memRegs.dpRCRB, memRegs.dpRCRB);
        printf("\nUpstream RCRB first %d bytes:\n\n", RCRB_SIZE);
        printBlock(memRegs.upRCRB, RCRB_SIZE);
        printf("\nDownstream RCRB first %d bytes:\n\n", RCRB_SIZE);
        printBlock(memRegs.dpRCRB, RCRB_SIZE);
        printf("\nUpstream Component first %d bytes:\n\n", COMP_CACHE_MEM_SIZE);
        printBlock(memRegs.upComp, COMP_CACHE_MEM_SIZE);
        printf("\nDownstream Component first %d bytes:\n\n", COMP_CACHE_MEM_SIZE);
        printBlock(memRegs.dpComp, COMP_CACHE_MEM_SIZE);
    } else {

    //printf("\nUpstream RCRB first %d bytes:\n\n", sz);
    //printBlock(memRegs.upRCRB,sz);



    
    printf("\nUpstream RCRB capabilities:\n\n");
    printRCRBCap(memRegs.upRCRB);


    printf("\nUpstream RCRB Extended capabilities:\n\n");
    printRCRBExtendCap(memRegs.upRCRB);


    //printf("\nDownstream RCRB first %d bytes:\n\n", sz);
    printRCRBExtendCap(memRegs.dpRCRB);
    printf("\nDownstream RCRB Extended capabilities:\n\n");
    //printBlock(memRegs.dpRCRB,sz);

    printLnkSpeed(memRegs.upRCRB, memRegs.dpRCRB);
    printLnkWidth(memRegs.dpRCRB, memRegs.dpRCRB);

    
    
    //uint64_t  addr;
    //uint64_t  addrHex = argv[1];
    //addr = readPhyMem(addrHex);
    //printf("Hufei: testing the RCRBs we got! upRCRB is 0x%lx, dpRCRB is 0x%lx !\n" , memRegs.upRCRB, memRegs.dpRCRB );
    //printf("Hufei: testing the Comps we got! upComp is 0x%lx, dpComp is 0x%lx !\n" , memRegs.upComp, memRegs.dpComp );
    //free(addr);
    }

    return 0;
}

uint64_t getDpRCRB(uint64_t upRCRB) {
    uint64_t addr;
    addr = upRCRB;
    addr -= 0x1000;
    return addr;
}

uint64_t getComp(uint64_t RCRB) {
    uint64_t addrMembar; //declare membar integer value
    addrMembar = RCRB;
    uint64_t comp;
    addrMembar += 0x0010; // membar location computed.
    comp = readPhyMem(addrMembar);
    comp &= 0xFFFFFFFFFFFFFFF0;
    comp += 0x1000;
    return comp;
}


void printRCRBExtendCap(uint64_t RCRB) {
    uint64_t addr_base, addr, offset, content;
    uint8_t  version;
    uint16_t capID;
    addr_base = RCRB;
    content = readPhyMem(addr_base);
        //printf("offset is %lx\n", offset);
    offset = (content & 0xFFF00000)>>20;

    while(offset) {
        addr = offset + addr_base;
        content = readPhyMem(addr);
        offset = (content & 0xFFF00000)>>20;
        capID = (content & 0xFFFF);
        version = (content & 0x000F0000)>>16;
        printf("Extended Capability ID==0x%x,\tVersion==%x,\tNext Capability offset is 0x%lx.\n", capID, version, offset);
    } 

}

void printRCRBCap(uint64_t RCRB) {
    uint64_t addr_base, addr, offset, content;
    uint8_t  version;
    uint16_t capID;
    addr_base = RCRB;
    content = readPhyMem(addr_base);
        //printf("offset is %lx\n", offset);
    offset = (content & 0xFFF00000)>>20;

    while(offset) {
        addr = offset + addr_base;
        content = readPhyMem(addr);
        offset = (content & 0xFFF00000)>>20;
        capID = (content & 0xFFFF);
        version = (content & 0x000F0000)>>16;
        printf("Extended Capability ID==0x%x,\tVersion==%x,\tNext Capability offset is 0x%lx.\n", capID, version, offset);
    } 

}


void printRCRBHeader(uint64_t RCRB){
    uint64_t addr = RCRB;
    uint32_t content;
    for(uint64_t i = 0; i < 0x40; i=i+4) {
        content = (uint32_t)readPhyMem(addr);
        printf("PCIE capability: Dword Address:0x%lx,\tRegister Value:0x%08x.\t\n", addr,content);
        addr = addr + 4;
        parsePCIE(i,content);

    }



}


void printBlock(uint64_t RCRB, uint16_t sz) {
    uint64_t addr = RCRB;
    uint32_t content;
    for(uint64_t i = 0; i < sz; i=i+4) {
        content = (uint32_t)readPhyMem(addr);
        printf("Dword Address:0x%lx,\tRegister Value:0x%08x.\t\n", addr,content);
        addr = addr + 4;
        //RCRBParse(i,content);

    }
        printf("\n");
  
}


void RCRBParse(uint16_t offset, uint32_t content){

    switch (offset) {
        case HEADER:
            parseHeader(content);
        case STATUS_CMD:
            ;
        default:
            ;
    }
}


uint64_t readPhyMem(uint64_t target)
{
    int fd; // Hufei:file handle needed to grab mem content
    void *map_base, *virt_addr; //Hufei: pointers to addresses
    uint64_t read_result = -1; //Hufei: the mem content.
    int access_size = 4; // Hufei: param2 needed to specify access byte length.
    unsigned int pagesize = (unsigned)sysconf(_SC_PAGESIZE); //Hufei: page size obtained. in header <unistd.h>
    //printf("Hufei: page/map size obtained is %u\n", pagesize);
    unsigned int map_size = pagesize;
    unsigned offset;//Hufei: what is this offset???????????????????????????????
    int f_align_check = 1; // flag to require alignment
        
    //printf("Hufei: the target manipulated is %8lu.\n", target);
    access_size = 4;

    if ((target + access_size - 1) < target)
    {
        printerr("ERROR: rolling over end of memory\n");
        exit(2);
    }
    offset = (unsigned int)(target & (pagesize - 1));
    //printf("Hufei: the offset calculated is %u !\n", offset);
    if (offset + access_size > pagesize)
    {
        // Access straddles page boundary:  add another page:
        map_size += pagesize;
       // printf("Hufei: offset size and map_size are %u and %u!\n", offset, map_size);
    }

    if (f_align_check && offset & (access_size - 1))
    {
        printerr("ERROR: address not aligned on %d!\n", access_size);
        exit(2);
    }

    fd = open("/dev/mem", O_RDWR | O_SYNC); //Hufei: here we read the file in Linux!!
    if (fd == -1)
    {
        printerr("Error opening /dev/mem (%d) : %s\n", errno, strerror(errno));
        exit(1);
    }
    //printf("/dev/mem opened.\n");
    //fflush(stdout);
    //printf("Hufei: map_size is %u!\n",  map_size);
    //printf("Hufei: file handle is %u!\n", fd);
    //printf("Hufei: ultimate target offset is %lx!\n", target & ~((typeof(target))pagesize - 1));
    //printf("Hufei: target offset mask is %lx!\n", ~((typeof(target))pagesize - 1));

    map_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED,
            fd,
            target & ~((typeof(target))pagesize - 1));

    //printf("Hufei: map base calculated is %p!\n", map_base); //Hufei: virtual address keeps changing

    if (map_base == (void *)-1)
    {
        printerr("Error mapping (%d) : %s\n", errno, strerror(errno));
        exit(1);
    }
    virt_addr = map_base + offset;
    //printf("Hufei: virtual addr calculated is %p!\n", virt_addr); 

   
    
    
     //   printf("Hufei: This is what we have read out!\n");
        read_result = *((volatile uint32_t *)virt_addr);
        
           // printf("%08" PRIX64 "\n", read_result);
           // printf("Hufei: only read is done here!\n");
            fflush(stdout);
    

    if (munmap(map_base, map_size) != 0)
    {
        printerr("ERROR munmap (%d) %s\n", errno, strerror(errno));
    }


    close(fd);
    //printf("read_result = 0x%x", read_result);
    //sprintf(retval,"0x%lx", read_result);

    //return read_result;
    return read_result;
}

void parseHeader ( uint32_t  val) {
    printf("\nRCRB header parsing:\n");
    uint32_t nextCap, version, nullID;
    nextCap = (val & 0xfff00000) >> 20;
    version = (val & 0xf0000)>>16;
    nullID = (val & 0xffff);
    printf("Next Capability pointer offset is 0x%x.\n",nextCap);
    printf("Version number is 0x%x.\n",version);
    printf("Null Capability ID is 0x%x.\n",nullID);
}

void parseStatuscmd ( uint32_t  val) {
    printf("\nRCRB status and cmd parsing:\n");
    uint32_t nextCap, version, nullID;
    nextCap = (val & 0xfff00000) >> 20;
    version = (val & 0xf0000)>>16;
    nullID = (val & 0xffff);
    printf("Next Capability pointer offset is 0x%x.\n",nextCap);
    printf("Version number is 0x%x.\n",version);
    printf("Null Capability ID is 0x%x.\n",nullID);
}

void parsePCIE (uint16_t offset, uint32_t val) {

    switch(offset) {
        case 0:
        parseCapHeader(val);
        break;
        case 0x04:
        break;
        case 0x08:
        break;
        case 0x0c:
        break;
        case 0x10:
        break;
        case 0x14:
        break;
        case 0x18:
        break;
        case 0x1c:
        break;
        case 0x20:
        break;
        default:
        break;
    }
}

void parseCapHeader(uint32_t val) {
    printf("\nRCRB Capability structure parsing:\n");
    uint8_t capID;
    uint8_t nxtCap;
    uint16_t capReg;
    capID = val & 0xff;
    nxtCap = val & 0xff00;
    capReg = val & 0xffff0000;
    printf("Capability ID is 0x%x.\n", capID);
    printf("Next Capability offset is 0x%x.\n", nxtCap);
    printf("Capability Version is 0x%x.\n", capReg & 0xf);
    printf("Device port type is 0x%x.\n", capReg & 0xf0);
    printf("Slot implemented bit is 0x%x.\n", capReg & 0x100);
    printf("Interrupt message Number is 0x%x.\n", capReg & 0x3e00);
}

void printLnkSpeed(uint64_t upRCRB, uint64_t dpRCRB ) {
    uint64_t offset = 0x50;
    uint64_t addrUp, addrDp;
    uint32_t content;
    uint8_t currSpd;
    addrUp = upRCRB+offset;
    addrDp = dpRCRB+offset;

    content = (uint32_t)readPhyMem(addrUp);
    currSpd = (content & 0x000f0000)>>16;
    printf("Current Link Speed according to Upstream RCRB is Gen %x.\n", currSpd);
    
    content = (uint32_t)readPhyMem(addrDp);
    currSpd = (content & 0x000f0000)>>16;
    printf("Current Link Speed according to Downstream RCRB is Gen %x.\n", currSpd);

}

void printLnkWidth(uint64_t upRCRB, uint64_t dpRCRB ) {
    uint64_t offset = 0x50;
    uint64_t addrUp, addrDp;
    uint32_t content;
    uint8_t currWidth;
    addrUp = upRCRB+offset;
    addrDp = dpRCRB+offset;

    content = (uint32_t)readPhyMem(addrUp);
    currWidth = (content & 0x03f00000)>>20;
    printf("Current Link Width according to Upstream RCRB is  X%d.\n", currWidth);
    content = (uint32_t)readPhyMem(addrDp);
    currWidth = (content & 0x03f00000)>>20;
    printf("Current Link Width according to Downstream RCRB is X%d.\n", currWidth);

}
