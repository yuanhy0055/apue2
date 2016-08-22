// data of supported flashproms

// "manufacturer", "type", rule,
// chip_id, size, sectorsize, chip_erase_time, sector_erase_time,
// prg_timeout, secret_size, twc, block_address[]

// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};


struct flashdat Flash[]={
{
"ATMEL", "AT29C256   256 Kbit (32K x 8-bit)", AT29,
0x1FDC, 32*1024, 64, 20, 0, 10, 64, 10, VCC5|VPP5|P1L|P30L,
{-1}
},
{
"ATMEL", "AT29C512   512 Kbit (64K x 8-bit)", AT29,
0x1F5D, 64*1024, 128, 20, 0, 10, 128, 10, VCC5|VPP5|P1L|P30L,
{-1}
},
{
"ATMEL", "AT29C010A   1 Mbit (128K x 8-bit)", AT29,
0x1FD5, 128*1024, 128, 20, 0, 10, 128, 10, VCC5|VPP5|P1L|P30L,
{0x00000, 0x1FFF0, -1}
},
{
"ATMEL", "AT29C020   2 Mbit (256K x 8-bit)", AT29,
0x1FDA, 256*1024, 256, 20, 0, 10, 256, 10, VCC5|VPP5|P1L|P30A,
{0x00000, 0x3FFF0, -1}
},
{
"ATMEL", "AT29C040   4 Mbit (512K x 8-bit)", AT29,
0x1F5B, 512*1024, 512, 20, 0, 10, 0, 10, VCC5|VPP5|P1A|P30A,
{-1}
},
{
"ATMEL", "AT29C040A   4 Mbit (512K x 8-bit)", AT29,
0x1FA4, 512*1024, 256, 20, 0, 10, 256, 10, VCC5|VPP5|P1A|P30A,
{0x00000, 0x7FFF0, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"ATMEL", "AT29LV256   256 Kbit (32K x 8-bit)", AT29,
0x1FBC, 32*1024, 64, 20, 0, 20, 0, 20, 0,
{-1}
},
{
"ATMEL", "AT29LV512   512 Kbit (64K x 8-bit)", AT29,
0x1F3D, 64*1024, 128, 20, 0, 20, 0, 20, 0,
{-1}
},
{
"ATMEL", "AT29BV010A   1 Mbit (128K x 8-bit)", AT29,
0x1F35, 128*1024, 128, 20, 0, 20, 0, 20, 0,
{0x00000, 0x1FFF0, -1}
},
{
"ATMEL", "AT29BV020   2 Mbit (256K x 8-bit)", AT29,
0x1FBA, 256*1024, 256, 20, 0, 20, 0, 20, 0,
{0x00000, 0x3FFF0, -1}
},
{
"ATMEL", "AT29BV040A   4 Mbit (512K x 8-bit)", AT29,
0x1FC4, 512*1024, 256, 20, 0, 20, 0, 20, 0,
{0x00000, 0x7FFF0, -1}
},
{
"ATMEL", "AT49BV512   512 Kbit (64K x 8-bit)", V29,
0x1F03, 64*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV001N   1 Mbit (128K x 8-bit)", V29,
0x1F05, 128*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV001T   1 Mbit (128K x 8-bit)", V29,
0x1F04, 128*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x1C000, -1}
},
{
"ATMEL", "AT49BV002N   2 Mbit (256K x 8-bit)", V29,
0x1F07, 256*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV002T   2 Mbit (256K x 8-bit)", V29,
0x1F08, 256*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x3C000, -1}
},
{
"ATMEL", "AT49F004N   4 Mbit (512K x 8-bit)", V29,
0x1F11, 512*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49F004T   4 Mbit (512K x 8-bit)", V29,
0x1F10, 512*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x7C000, -1}
},
{
"ATMEL", "AT49BV010   1 Mbit (128K x 8-bit)", V29,
0x1F17, 128*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV040   4 Mbit (512K x 8-bit)", V29,
0x1F13, 512*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV8192A   8 Mbit (1M x 8-bit)", V29,
0x1FA0, 1*1024*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV008A   8 Mbit (1M x 8-bit)", V29,
0x1F22, 1*1024*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49BV8192AT   8 Mbit (1M x 8-bit)", V29,
0x1FA3, 1*1024*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0xFC000, -1}
},
{
"ATMEL", "AT49BV008AT   8 Mbit (1M x 8-bit)", V29,
0x1F21, 1*1024*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{0xFC000, -1}
},
{
"ATMEL", "AT49F010   1 Mbit (128K x 8-bit)", V29,
0x1F87, 128*1024, 0, 10*1000, 0, 1, 0, 10, 0,
{0x00000, -1}
},
{
"ATMEL", "AT49F020   2 Mbit (256K x 8-bit)", V29,
0x1F0B, 256*1024, 0, 10*1000, 0, 1, 0, 10, 0,
{0x00000, -1}
},
{
"AMD", "AM29F010   1 Mbit (128K x 8-bit)", V29,
0x0120, 128*1024, 16*1024, 15*1000, 15*1000, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM29F010A   1 Mbit (128K x 8-bit)", AM29,
0x0120, 128*1024, 16*1024, 15*1000, 15*1000, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM29F040B   4 Mbit (512K x 8-bit)", AM29,
0x01A4, 512*1024, 64*1024, 80*1000, 10*1000, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM29F080B   8 Mbit (1M x 8-bit)", AM29,
0x01D5, 1*1024*1024, 64*1024, 150*1000, 10*1000, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM29F016B   16 Mbit (2M x 8-bit)", AM29,
0x01AD, 2*1024*1024, 64*1024, 300*1000, 10*1000, 1, 0, 0, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"AMD", "AM29F032B   32 Mbit (4M x 8-bit)", AM29,
0x0141, 4*1024*1024, 64*1024, 550*1000, 10*1000, 3, 0, 0, 0,
{-1}
},
{
"AMD", "AM29F002   2 Mbit (256K x 8-bit) Top Boot Block", AM29,
0x01B0, 256*1024, 0, 8*1000, 7*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"AMD", "AM29F002   2 Mbit (256K x 8-bit) Bottom Boot Block", AM29,
0x0134, 256*1024, 0, 8*1000, 7*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"AMD", "AM29F004   4 Mbit (512K x 8-bit) Top Boot Block", AM29,
0x0177, 512*1024, 0, 8*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000,
 0x70000, 0x78000, 0x7A000, 0x7C000, -1}
},
{
"AMD", "AM29F004   4 Mbit (512K x 8-bit) Bottom Boot Block", AM29,
0x017B, 512*1024, 0, 8*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000,
 0x40000, 0x50000, 0x60000, 0x70000, -1}
},
{
"AMD", "AM28F256A   256 Kbit (32K x 8-bit)", AM28A,
0x012F, 32*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F512A   512 Kbit (64K x 8-bit)", AM28A,
0x01AE, 64*1024, 0, 6*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F010A   1 Mbit (128K x 8-bit)", AM28A,
0x01A2, 128*1024, 0, 6*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F020A   2 Mbit (256K x 8-bit)", AM28A,
0x0129, 256*1024, 0, 6*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F256   256 Kbit (32K x 8-bit)", CAT28,
0x01A1, 32*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F512   512 Kbit (64K x 8-bit)", CAT28,
0x0125, 64*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"AMD", "AM28F010   1 Mbit (128K x 8-bit)", CAT28,
0x01A7, 128*1024, 0, 11*1000, 0, 1, 0, 0, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"AMD", "AM28F020   2 Mbit (256K x 8-bit)", CAT28,
0x012A, 256*1024, 0, 11*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"MOSEL VITELIC", "V29C51000T   512 Kbit (64K x 8-bit)", V29,
0x4000, 64*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0xE000, -1}
},
{
"MOSEL VITELIC", "V29C51000B   512 Kbit (64K x 8-bit)", V29,
0x40A0, 64*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0x0000, -1}
},
{
"MOSEL VITELIC", "V29C51001T   1 Mbit (128K x 8-bit)", V29,
0x4001, 128*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0x1E000, -1}
},
{
"MOSEL VITELIC", "V29C51001B   1 Mbit (128K x 8-bit)", V29,
0x40A1, 128*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0x00000, -1}
},
{
"MOSEL VITELIC", "V29C51002T   2 Mbit (256K x 8-bit)", V29,
0x4002, 256*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0x3C000, -1}
},
{
"MOSEL VITELIC", "V29C51002B   2 Mbit (256K x 8-bit)", V29,
0x40A2, 256*1024, 512, 4*1000, 20, 1, 0, 0, 0,
{0x00000, -1}
},
{
"MOSEL VITELIC", "V29C51004T   4 Mbit (512K x 8-bit)", V29,
0x4003, 512*1024, 1024, 4*1000, 20, 1, 0, 0, 0,
{0x7C000, -1}
},
{
"MOSEL VITELIC", "V29C51004B   4 Mbit (512K x 8-bit)", V29,
0x40A3, 512*1024, 1024, 4*1000, 20, 1, 0, 0, 0,
{0x00000, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"HYUNDAI", "HY29F002T   2 Mbit (256K x 8-bit)", AM29,
0xADB0, 256*1024, 0, 55*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"HYUNDAI", "HY29F002B   2 Mbit (256K x 8-bit)", AM29,
0xAD34, 256*1024, 0, 55*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"HYUNDAI", "HY29F040A   4 Mbit (512K x 8-bit)", AM29,
0xADA4, 512*1024, 64*1024, 64*1000, 8*1000, 1, 0, 0, 0,
{-1}
},
{
"SST", "SST29EE512   512 Kbit (64K x 8-bit)", AT29,
0xBF5D, 64*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST29LE512   512 Kbit (64K x 8-bit)", AT29,
0xBF3D, 64*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST29EE010   1 Mbit (128K x 8-bit)", AT29,
0xBF07, 128*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "PH29EE010   1 Mbit (128K x 8-bit)", W29,
0xBF07, 128*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST29LE010   1 Mbit (128K x 8-bit)", AT29,
0xBF08, 128*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST29EE020   2 Mbit (256K x 8-bit)", AT29,
0xBF10, 256*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST29LE020   2 Mbit (256K x 8-bit)", AT29,
0xBF10, 256*1024, 128, 20, 0, 10, 0, 10, 0,
{-1}
},
{
"SST", "SST39VF512   512 Kbit (64K x 8-bit)", SST39,
0xBFD4, 64*1024, 4*1024, 100, 25, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39VF010   1 Mbit (128K x 8-bit)", SST39,
0xBFD5, 128*1024, 4*1024, 100, 25, 1, 0, 1, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"SST", "SST39VF020   2 Mbit (256K x 8-bit)", SST39,
0xBFD6, 256*1024, 4*1024, 100, 25, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39VF040   4 Mbit (512K x 8-bit)", SST39,
0xBFD7, 512*1024, 4*1024, 100, 25, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39SF512   512Kbit (64K x 8-bit)", SST39,
0xBFB4, 64*1024, 4*1024, 100, 0, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39SF010(A)   1 Mbit (128K x 8-bit)", SST39,
0xBFB5, 128*1024, 4*1024, 100, 0, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39SF020A   2 Mbit (256K x 8-bit)", SST39,
0xBFB6, 256*1024, 4*1024, 100, 0, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST39SF040   4 Mbit (512K x 8-bit)", SST39,
0xBFB7, 512*1024, 4*1024, 100, 0, 1, 0, 1, 0,
{-1}
},
{
"SST", "SST49LF002A  2 Mbit (256K x 8-bit)", SST39,
0xBF57, 256*1024, 16*1024, 100, 25, 1, 0, 0, 0,
{-1}
},
{
"ST", "M29F010B   1 Mbit (128K x 8-bit)", AM29,
0x2020, 128*1024, 16*1024, 6*1000, 2*1000, 1, 0, 0, 0,
{-1}
},
{
"ST", "M29F002T   1 Mbit (256K x 8-bit)", AM29,
0x20B0, 256*1024, 0, 10*1000, 4*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"ST", "M29F002B   1 Mbit (256K x 8-bit)", AM29,
0x2034, 256*1024, 0, 10*1000, 4*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"ST", "M29F040B   4 Mbit (512K x 8-bit)", AM29,
0x20E2, 512*1024, 64*1024, 20*1000, 4*1000, 1, 0, 0, 0,
{-1}
},
{
"ST", "M29F040   4 Mbit (512K x 8-bit)", V29,
0x20E2, 512*1024, 64*1024, 30*1000, 30*1000, 1, 0, 0, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"INTEL", "28F001BX-T readonly  1 Mbit (128K x 8-bit)", I28a,
0x8994, 128*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x1C000, 0x1D000, 0x1E000, -1}
},
{
"INTEL", "28F001BX-T   1 Mbit (128K x 8-bit)", I28,
0x8994, 128*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x1C000, 0x1D000, 0x1E000, -1}
},
{
"INTEL", "28F001BX-B readonly  1 Mbit (128K x 8-bit)", I28a,
0x8995, 128*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x02000, 0x03000, 0x04000, -1}
},
{
"INTEL", "28F001BX-B   1 Mbit (128K x 8-bit)", I28,
0x8995, 128*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x02000, 0x03000, 0x04000, -1}
},
{
"INTEL", "28F002BC   2 Mbit (256K x 8-bit)", I28,
0x897C, 256*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x20000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"INTEL", "28F010   1 Mbit (128K x 8-bit)", CAT28,
0x89B4, 128*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"INTEL", "28F020   2 Mbit (256K x 8-bit)", CAT28,
0x89BD, 256*1024, 0, 30*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"INTEL", "28F200B   2 Mbit (256K x 8-bit)", I28,
0x8975, 256*1024, 0, 6*1000, 2*1000, 1, 0, 0, 0,
{0x00000, 0x02000, 0x03000, 0x04000, 0x10000, -1}
},
{
"INTEL", "28F200T   2 Mbit (256K x 8-bit)", I28,
0x8974, 256*1024, 0, 14*1000, 7*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x1C000, 0x1D000, 0x1E000, -1}
},
{
"CATALYST", "CAT28F512   512 Kbit (64K x 8-bit)", CAT28,
0x31B8, 64*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"CATALYST", "CAT28F010   1 Mbit (128K x 8-bit)", CAT28,
0x31B4, 128*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"CATALYST", "CAT28F020   2 Mbit (256K x 8-bit)", CAT28,
0x31BD, 256*1024, 0, 10*1000, 0, 1, 0, 0, 0,
{-1}
},
{
"CATALYST", "CAT28F001T   1 Mbit (128K x 8-bit)", I28,
0x3194, 128*1024, 0, 65*1000, 15*1000, 1, 0, 0, 0,
{0x00000, 0x1C000, 0x1D000, 0x1E000, -1}
},
{
"CATALYST", "CAT28F001B   1 Mbit (128K x 8-bit)", I28,
0x3195, 128*1024, 0, 65*1000, 15*1000, 1, 0, 0, 0,
{0x00000, 0x02000, 0x03000, 0x04000, -1}
},
{
"WINBOND", "W29C512A   512 Kbit (64K x 8-bit)", AT29,
0xDAC8, 64*1024, 128, 50, 0, 10, 128, 10, 0,
{-1}
},
{
"WINBOND", "W29C010M   1 Mbit (128K x 8-bit)", W29,
0xDAC1, 128*1024, 128, 50, 0, 10, 128, 10, 0,
{-1}
},
{
"WINBOND", "W29C020   2 Mbit (256K x 8-bit)", AT29,
0xDA45, 256*1024, 128, 50, 0, 10, 128, 10, 0,
{0x00000, 0x3FFF0, -1}
},
{
"WINBOND", "W29C040   4 Mbit (512K x 8-bit)", AT29,
0xDA46, 512*1024, 256, 50, 0, 10, 256, 10, 0,
{0x00000, 0x7FFF0, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"WINBOND", "W49C020   2 Mbit (256K x 8-bit)", V29,
0xDA8C, 256*1024, 0, 1*1000, 0, 1, 0, 10, 0,
{0x00000, -1}
},
{
"WINBOND", "W49F002U   2 Mbit (256K x 8-bit)", SST39,
0xDA0B, 256*1024, 0, 200, 0, 1, 0, 0, VCC5|VPP5|P1H|P30A,
{-1}
},
{
"WINBOND", "W49F002U mod 2 Mbit (256K x 8-bit)", SST39a,
0xDA0B, 256*1024, 0, 200, 0, 1, 0, 0, VCC5|VPP5|P1H|P30A,
{-1}
},
{
"MACRONIX", "MX28F1000PL   1 Mbit (128K x 8-bit)", CAT28,
0xC211, 128*1024, 0, 6*1000, 6*1000, 1, 0, 0, 0,
{-1}
},
{
"MACRONIX", "MX28F1000P   1 Mbit (128K x 8-bit)", CAT28,
0xC21A, 128*1024, 0, 5*1000, 5*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x08000, 0x0C000, 0x10000, 0x14000, 0x18000,
 0x1C000, 0x1D000, 0x1E000, 0x1F000, -1}
},
{
"MACRONIX", "MX28F2000P   2 Mbit (128K x 8-bit)", CAT28,
0xC22A, 256*1024, 0, 5*1000, 5*1000, 1, 0, 0, 0,
{0x00000, 0x01000, 0x02000, 0x03000, 0x04000, 0x08000, 0x0C000,
 0x10000, 0x14000, 0x18000, 0x1C000, 0x20000, 0x24000, 0x28000,
 0x2C000, 0x30000, 0x34000, 0x38000, 0x3C000, 0x3D000, 0x3E000,
 0x3F000, -1}
},
{
"MACRONIX", "MX28F2000T   2 Mbit (128K x 8-bit)", CAT28,
0xC23C, 256*1024, 0, 5*1000, 5*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x08000, 0x0C000, 0x10000, 0x14000, 0x18000,
 0x1C000, 0x20000, 0x24000, 0x28000, 0x2C000, 0x30000, 0x34000,
 0x38000, 0x39000, 0x3A000, 0x3B000, 0x3C000, 0x3D000, 0x3E000,
 0x3F000, -1}
},
{
"MACRONIX", "MX29F040   4 Mbit (512K x 8-bit)", AM29,
0xC2A4, 512*1024, 64*1024, 32*1000, 11*1000, 1, 0, 0, 0,
{-1}
},
{
"MACRONIX", "MX29F080   8 Mbit (1M x 8-bit)", AM29,
0xC2D5, 1*1024*1024, 64*1024, 64*1000, 11*1000, 1, 0, 0, 0,
{-1}
},
{
"AMIC", "A29010   1 Mbit (128K x 8-bit)", AM29,
0x37A4, 128*1024, 32*1024, 64*1000, 8*1000, 1, 0, 0, 0,
{-1}
},
{
"AMIC", "A29040A   4 Mbit (512K x 8-bit)", AM29,
0x3786, 512*1024, 64*1024, 64*1000, 8*1000, 1, 0, 0, 0,
{-1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"AMIC", "A29001T   1 Mbit (128K x 8-bit)", AM29,
0x37A1, 128*1024, 0, 64*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x08000, 0x10000, 0x18000, 0x1C000, 0x1D000, 0x1E000, -1}
},
{
"AMIC", "A29001B   1 Mbit (128K x 8-bit)", AM29,
0x374C, 128*1024, 0, 64*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x02000, 0x03000, 0x04000, 0x08000, 0x10000, 0x18000, -1}
},
{
"AMIC", "A29002T   2 Mbit (256K x 8-bit)", AM29,
0x378C, 256*1024, 0, 64*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"AMIC", "A29002B   2 Mbit (256K x 8-bit)", AM29,
0x370D, 256*1024, 0, 64*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"Texas Instruments", "TMS29F040   4 Mbit (512K x 8-bit)", V29,
0x9794, 512*1024, 64*1024, 120*1000, 30*1000, 1, 0, 0, 0,
{-1}
},
{
"IMT", "IM29F001T   1 Mbit (128K x 8-bit)", V29,
0x7FA0, 128*1024, 512, 3*1000, 9, 1, 0, 0, 0,
{0x00000, -1}
},
{
"IMT", "IM29F001B   1 Mbit (128K x 8-bit)", V29,
0x7FA3, 128*1024, 512, 3*1000, 9, 1, 0, 0, 0,
{0x00000, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"IMT", "IM29F002T   2 Mbit (256K x 8-bit)", V29,
0x7FA1, 256*1024, 512, 3*1000, 9, 1, 0, 0, 0,
{0x00000, -1}
},
{
"IMT", "IM29F002B   2 Mbit (256K x 8-bit)", V29,
0x7FA2, 256*1024, 512, 3*1000, 9, 1, 0, 0, 0,
{0x00000, -1}
},
{
"IMT", "IM29F004T   4 Mbit (512K x 8-bit)", V29,
0x7FAF, 512*1024, 1024, 2*1000, 6, 1, 0, 0, 0,
{0x00000, -1}
},
{
"IMT", "IM29F004B   4 Mbit (512K x 8-bit)", V29,
0x7FAE, 512*1024, 1024, 2*1000, 6, 1, 0, 0, 0,
{0x00000, -1}
},
{
"PMC", "Pm29F002T   2 Mbit (256K x 8-bit)", AM29,
0x9D1D, 256*1024, 0, 100, 100, 1, 0, 0, 0,
{0x00000, 0x20000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"PMC", "Pm29F002B   2 Mbit (256K x 8-bit)", AM29,
0x9D2D, 256*1024, 0, 100, 100, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x20000, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"PMC", "Pm29F004T   4 Mbit (512K x 8-bit)", AM29,
0x9D1E, 512*1024, 0, 100, 100, 1, 0, 0, 0,
{0x00000, 0x20000, 0x40000, 0x60000, 0x78000, 0x7A000, 0x7C000, -1}
},
{
"PMC", "Pm29F004B   4 Mbit (512K x 8-bit)", AM29,
0x9D2E, 512*1024, 0, 100, 100, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x20000, 0x40000, 0x60000, -1}
},
{
"PMC", "Pm39LV512R   512 kbit (64K x 8-bit)", AM29,
0x9D1B, 64*1024, 4*1024, 100, 100, 1, 0, 0, 0,
{-1}
},
{
"PMC", "Pm39LV010R   1 Mbit (128K x 8-bit)", AM29,
0x9D1C, 128*1024, 4*1024, 100, 100, 1, 0, 0, 0,
{-1}
},
{
"EON", "EN29F002T   2 Mbit (256K x 8-bit)", EON29F00x,
0x1C92, 256*1024, 0, 35*1000, 3*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"EON", "EN29F002B   2 Mbit (256K x 8-bit)", EON29F00x,
0x1C97, 256*1024, 0, 35*1000, 3*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"EON", "EN29F040   4 Mbit (512K x 8-bit)", EON29F0x0,
0x1C04, 512*1024, 64*1024, 35*1000, 3*1000, 1, 0, 0, 0,
{ -1}
},
{
"EON", "EN29F080   8 Mbit (1M x 8-bit)", EON29F0x0,
0x1C08, 1*1024*1024, 64*1024, 35*1000, 3*1000, 1, 0, 0, 0,
{ -1}
},
{
"Fujitsu", "MBM29F002TC   2 Mbit (256K x 8-bit)", AM29,
0x04B0, 256*1024, 0, 56*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
{
"Fujitsu", "MBM29F002BC   2 Mbit (256K x 8-bit)", AM29,
0x0434, 256*1024, 0, 56*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"Fujitsu", "MBM29LV002TC   2 Mbit (256K x 8-bit)", AM29,
0x0440, 256*1024, 0, 70*1000, 10*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x38000, 0x3A000, 0x3C000, -1}
},
// struct flashdat { char *manufacturer; char *type; int rule;
// __u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
// __u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
// __u32 twc; __u32 pinmode; __s32 block_address[MaxBlock];};
{
"Fujitsu", "MBM29LV002BC   2 Mbit (256K x 8-bit)", AM29,
0x04C2, 256*1024, 0, 70*1000, 10*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000, -1}
},
{
"Fujitsu", "MBM29F004TC   4 Mbit (512K x 8-bit)", AM29,
0x0477, 512*1024, 0, 88*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000,
 0x70000, 0x78000, 0x7A000, 0x7C000, -1}
},
{
"Fujitsu", "MBM29F004BC   4 Mbit (512K x 8-bit)", AM29,
0x047B, 512*1024, 0, 88*1000, 8*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000,
 0x40000, 0x50000, 0x60000, 0x70000, -1}
},
{
"Fujitsu", "MBM29LV004TC   4 Mbit (512K x 8-bit)", AM29,
0x04B5, 512*1024, 0, 110*1000, 10*1000, 1, 0, 0, 0,
{0x00000, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000,
 0x70000, 0x78000, 0x7A000, 0x7C000, -1}
},
{
"Fujitsu", "MBM29LV004BC   4 Mbit (512K x 8-bit)", AM29,
0x04B6, 512*1024, 0, 110*1000, 10*1000, 1, 0, 0, 0,
{0x00000, 0x04000, 0x06000, 0x08000, 0x10000, 0x20000, 0x30000,
 0x40000, 0x50000, 0x60000, 0x70000, -1}
},
{
"Fujitsu", "MBM29F040C   4 Mbit (512K x 8-bit)", AM29,
0x04A4, 512*1024, 64*1024, 64*1000, 8*1000, 1, 0, 0, 0,
{-1}
},
{
"Fujitsu", "MBM29F080A   8 Mbit (1M x 8-bit)", AM29,
0x04D5, 1*1024*1024, 64*1024, 128*1000, 8*1000, 1, 0, 0, 0,
{-1}
},
{
"Fujitsu", "MBM29LV080A   8 Mbit (1M x 8-bit)", AM29,
0x0438, 1*1024*1024, 64*1024, 160*1000, 10*1000, 1, 0, 0, 0,
{-1}
},
{
"unknown", "unknown", UNKNOWN,
0, 0, 0, 0, 0, 0, 0, 0, 0,
{-1}
}
};

/* ------------------------------------------------------------------------ */
