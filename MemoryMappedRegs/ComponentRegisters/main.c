#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

//#define IO_CONFIG 0xf8
//#define IO_BYTE_1 0xfc
#define DBG 0

struct capStruct {
    struct capStruct* next;
    uint32_t offset;
    uint32_t length;
};

uint8_t headerDecode(uint8_t* base);
void capHeaderDecode(uint8_t* base, struct capStruct* cap);
void parseCapID (uint16_t capID);
uint32_t getLen(struct capStruct* head);
void dumpRaw(uint8_t* base, uint32_t len);
void freeMem(struct capStruct*  head);


int main(int argc, char *argv[]) {
    uint32_t i; 
    int fd;
    uint32_t offset, config[4096];
    uint8_t *bar0, *bar, value;
    uint32_t cfg_sz_bytes;
    struct capStruct* headCaps, *nxtCaps, *prevCaps;
    uint32_t total;
    
    fd = open("/sys/bus/pci/devices/0000:98:00.0/resource2", O_RDWR | O_SYNC);

    if (fd < 0) {
        perror("Error opening BAR's resource file");
        return -1;
    }

    bar0 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //bar0 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, fd, 0);
    //bar0 = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    close(fd);

    /* Let's check if we have a valid pointer */
    if(bar0 == MAP_FAILED) {
        perror("Memory mapping of BAR failed");
        return -1;
    }

    
    uint8_t capCount;
    uint8_t j;
    
    capCount = headerDecode(bar0);
    bar = bar0;
    bar0 = bar0 + 16;
    
    for(j=0;j<capCount;j++) {
        nxtCaps = (struct capStruct*)malloc(sizeof(struct capStruct)); //new pointer to obj
        if(nxtCaps==NULL){
            printf("Memory not allocated.\n");
            exit(0);
        } else {
            capHeaderDecode(bar0,nxtCaps);
            bar0 = bar0 + 16;
            if(j==0) {
                headCaps = nxtCaps;
            }
            if(j!=0) {
                prevCaps->next = nxtCaps;
            }
            if(DBG) {
                printf("\n\n\n\nCap next value  is %02p.",prevCaps->next);
                printf("prev value  is %02p.",prevCaps);
                printf("Cap next value  is %02p.\n\n\n\n\n",nxtCaps);
                printf("Cap offset is 0x%04x.\n",prevCaps->offset);
                printf("Cap length in byte is 0x%04x.\n",prevCaps->length);
            }
            prevCaps = nxtCaps;
        }
    
    }

    bar0 = bar0 - capCount*16;
    total = getLen(headCaps);
    printf("Print out all %x bytes of raw data!!\n\n\n",total);
    dumpRaw(bar0,total);
    munmap(bar, 4096);
    freeMem(headCaps);
    return 0;

}


uint8_t headerDecode(uint8_t* base) {
    uint16_t capID;
    uint8_t ver;
    uint16_t capCnt;
    capID = *base + (*(base+1)) <<8;
    ver = *(base+2);
    capCnt = *(base+4) + (*(base+5)<<8);
    printf("Cap ID is 0x%04x.\n",capID);
    printf("Version is 0x%02x.\n",ver);
    printf("Cap Count is 0x%04x.\n",capCnt);
    printf("\n");
    return capCnt;
}

void capHeaderDecode(uint8_t* base, struct capStruct* cap ) {
    uint16_t capID;
    uint8_t ver;
    uint32_t offset;
    uint32_t length;
    capID = *base + (*(base+1) <<8);
    ver = *(base+2);
    offset = *(base+4) + (*(base+5)<<8)+ (*(base+6)<<16)+ (*(base+7)<<24);
    length = *(base+8) + (*(base+9)<<8)+ (*(base+10)<<16)+ (*(base+11)<<24);
    printf("Cap ID is 0x%04x.",capID);
    parseCapID(capID);
    printf("Version is 0x%02x.\n",ver);
    printf("Cap offset is 0x%04x.\n",offset);
    printf("Cap length in byte is 0x%04x.\n",length);
    cap->offset = offset;
    cap->length = length;
    cap->next = NULL;
    printf("\n");
}

void parseCapID(uint16_t capID) {
    switch(capID) {
    case 0x0001:
        printf(" Device Status Registers.\n");
        break;
    case 0x0002:
        printf(" Primary Mailbox Registers.\n");
        break;
    case 0x4000:
        printf(" Memory Device Status Registers.\n");
        break;
    default:
        printf(" Cannot find the registers type.\n");
        break;
    }
}

void dumpRaw(uint8_t* base, uint32_t len) {
    uint8_t value;
    for (uint32_t i = 0; i != len; i=i+1) {
        value = *(base + i);
        printf("addr=0x%03x, value=0x%02x\t",i, value);
        printf("bit fileds: [%3d:%3d]\t", i%16*8+7 , i%16*8);
        printf("byte number: %4d, word number %4d.\n", i, i/4);
    }
}

uint32_t getLen(struct capStruct*  head) {
    while(head->next!=NULL){
        head=head->next;
        //printf("while loop not over\n");
    }
    return head->offset+head->length;
}

void freeMem(struct capStruct*  head) {
    struct capStruct* temp;
     while(head!=NULL){
        temp = head;
        head=temp->next;
        free(head);
        //printf("while loop not over\n");
    }
}
