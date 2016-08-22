#include <linux/version.h>

#ifndef __s64
#define __s64	signed long long
#endif

#ifndef __s32
#define __s32	signed long
#endif

#ifndef __s16
#define __s16	signed short
#endif

#ifndef __s8
#define __s8	signed char
#endif

#ifndef __u64
#define __u64	unsigned long long
#endif

#ifndef __u32
#define __u32	unsigned long
#endif

#ifndef __u16
#define __u16	unsigned short
#endif

#ifndef __u8
#define __u8	unsigned char
#endif

///////2.6//////extern int printk( const char* fmt, ...);

#define	MaxBlock		30
#define MAX_INFO_SIZE		512
#define	MAX_PROTECT_SIZE	40

// "manufacturer", "type", rule,
// chip_id, size, sectorsize, chip_erase_time, sector_erase_time,
// prg_timeout, software_protection_key_size, time_wait_cycle, sector_address

struct flashdat { char *manufacturer; char *type; int rule;
__u16 id; __u32 rom_size; __u32 sa_size; __u32 c_erase_timeout;
__u32 int s_erase_timeout; __u32 prg_timeout; __u16 secret_size;
__u32 twc; __u32 pinmode; __s32 block_address[MaxBlock]; };

// shame on me, but how can i do it?
#define Trust	static const char *reliability[]={"", "(sure)", "(not sure)", ""};

/* ------------------------------------------------------------------------ */
// struct for hardware details

struct flash_hw {
			struct flashdat *flash;
			ssize_t curr_pos;
			__u32 _twc;
			int trust;
			void (*flash_put)(__u8 byte,__u32 addr,int dev);
			__u8 (*flash_get)(__u32 addr,int dev);
			struct proc_dir_entry *flash_proc;
			struct proc_dir_entry *eeprom_proc;
			struct proc_dir_entry *basic_proc;
			__u32 max_romsize;
			__u32 usable_romsize;
			char * flash_name;
			char * info;
			char * path;
			char * eeprom_path;
			char * eeprom_name;
			int protect;
			int dev; };

/* ------------------------------------------------------------------------ */

struct flash_shm {
			int (*flash_open)(struct flash_hw *);
			ssize_t (*flash_read)(struct flash_hw *,struct file *,
			char *, size_t, loff_t *);
			ssize_t (*flash_write)(struct flash_hw *, struct file *,
			const char *, size_t, loff_t *);
			int (*flash_erase)(struct flash_hw *); };

#define MAGIC_KEY_FLASH		"flash35"
#define MAGIC_KEY_FLASH_A flash_io_35  
#define	Flash_Max_Devices	5

/* ------------------------------------------------------------------------ */

#define VCCMASK 0x000F
#define	VCC3	0x0001	// voltage 3.3V
#define	VCC5	0x0002	// voltage 5V
#define	VCC12	0x0003	// voltage 12V

#define VPPMASK 0x00F0
#define	VPP3	0x0010	// program voltage 3.3V
#define	VPP5	0x0020	// program voltage 5V
#define	VPP12	0x0030	// program voltage 12V

#define P1MASK	0x0F00
#define P1A	0x0000	// pin1	address
#define P1H	0x0100	// pin1 VCC (high)
#define P1L	0x0200	// pin1 ground (low)
#define P1V	0x0300	// pin1 program voltage

#define P30MASK 0xF000
#define P30A	0x0000	// pin30 address
#define P30H	0x1000	// pin30 VCC (high)
#define P30L	0x2000	// pin30 ground (low)
#define P30V	0x3000	// pin30 program voltage
