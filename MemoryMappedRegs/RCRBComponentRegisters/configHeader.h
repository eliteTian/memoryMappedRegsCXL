/*
*
*
*
*/
#define HEADER                          0x0
#define STATUS_CMD                      0x4
#define CLASS_REVISION_                 0x8
#define BIST_HEADERTYPE_LATENCY_CACHE   0xC
#define BAR0                            0x10
#define BAR1                            0x14
/******************Type 0 Header specific********************/
#define BAR2                            0x18
#define BAR3                            0x1C
#define BAR4                            0x20
#define BAR5                            0x24
#define CARDBUS_CIS_POINTER             0x28
#define SUBID_SUBVENDORID               0x2C
#define EXPANSION_ROM_BA_TYPE0          0x30
#define CAP_POINTER                     0x34
#define RESERVED                        0x38
#define MAX_MIN_INTERRUPT               0x3C
/******************Type 1 Header specific********************/
#define SECON_TIMER_NUMBERS             0x18
#define SECON_STATUS_IO                 0x1C
#define MEM_LIMIT_BASE                  0x20
#define PREFETCH_MEM_LIMIT_BASE         0x24
#define PREF_MEM_BASE_UPPER             0x28
#define PREF_MEM_LIMIT_UPPER            0x2C
#define IO_BASE                         0x30
#define EXPANSION_ROM_BA_TYPE1          0x38
#define BRIDGE_INTERRUPT                0x3C


#define RCRB_SIZE                       0x1000
#define COMP_CACHE_MEM_SIZE             0x1000  


uint64_t readPhyMem(uint64_t addr);
uint64_t getDpRCRB(uint64_t upRCRB);
uint64_t getComp(uint64_t upRCRB);
void printBlock(uint64_t RCRB, uint16_t sz);

void RCRBParse(uint16_t offset, uint32_t content);
void parseHeader ( uint32_t  val);
void printRCRBHeader(uint64_t RCRB);

void printLnkSpeed(uint64_t upRCRB, uint64_t dpRCRB);

void printLnkWidth(uint64_t upRCRB, uint64_t dpRCRB);

void printRCRBCap(uint64_t RCRB);
void printRCRBExtendCap(uint64_t RCRB);

void parsePCIE (uint16_t offset, uint32_t val);
void parseCapHeader(uint32_t val);



void printRCRBEcap(uint64_t RCRB);


struct memAddrHeader {
    uint64_t  upRCRB;
    uint64_t  dpRCRB;
    uint64_t  upComp;
    uint64_t  dpComp;
};




