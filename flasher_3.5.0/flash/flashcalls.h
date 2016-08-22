void chip_reset_unknown(struct flash_hw * flash_hw);
void chip_reset_am29(struct flash_hw * flash_hw);
void chip_reset_at29(struct flash_hw * flash_hw);
void chip_reset_am28a(struct flash_hw * flash_hw);
void chip_reset_v29(struct flash_hw * flash_hw);
void chip_reset_cat28(struct flash_hw * flash_hw);
void chip_reset_i28a(struct flash_hw * flash_hw);
void chip_reset_i28(struct flash_hw * flash_hw);
void chip_reset_sst39a(struct flash_hw * flash_hw);
void chip_reset_sst39(struct flash_hw * flash_hw);
void chip_reset_w29(struct flash_hw * flash_hw);
void chip_reset_eon29F00x(struct flash_hw * flash_hw);
void chip_reset_eon29F0x0(struct flash_hw * flash_hw);
void (*chip_reset[])(struct flash_hw * flash_hw)={
	chip_reset_unknown,
	chip_reset_am29,
	chip_reset_at29,
	chip_reset_am28a,
	chip_reset_v29,
	chip_reset_cat28,
	chip_reset_i28a,
	chip_reset_i28,
	chip_reset_sst39a,
	chip_reset_sst39,
	chip_reset_w29,
	chip_reset_eon29F00x,
	chip_reset_eon29F0x0	};
ssize_t flash_write_data_unknown(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_am29(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_at29(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_am28a(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_v29(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_cat28(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_i28a(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_i28(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_sst39a(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_sst39(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_w29(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_eon29F00x(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t flash_write_data_eon29F0x0(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos);
ssize_t (*flash_write_data[])(struct flash_hw * flash_hw, struct file * file, const char * buf, size_t count, loff_t *ppos)={
	flash_write_data_unknown,
	flash_write_data_am29,
	flash_write_data_at29,
	flash_write_data_am28a,
	flash_write_data_v29,
	flash_write_data_cat28,
	flash_write_data_i28a,
	flash_write_data_i28,
	flash_write_data_sst39a,
	flash_write_data_sst39,
	flash_write_data_w29,
	flash_write_data_eon29F00x,
	flash_write_data_eon29F0x0	};
void hwmod_put_byte_unknown(struct flash_hw * flash_hw, __u8 data, __u32 addr);
void hwmod_put_byte_i28a(struct flash_hw * flash_hw, __u8 data, __u32 addr);
void hwmod_put_byte_sst39a(struct flash_hw * flash_hw, __u8 data, __u32 addr);
void (*hwmod_put_byte[])(struct flash_hw * flash_hw, __u8 ,__u32 addr)={
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_i28a,
	hwmod_put_byte_unknown,
	hwmod_put_byte_sst39a,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown,
	hwmod_put_byte_unknown	};
int protect_check_unknown(struct flash_hw * flash_hw);
int write_protect_check_am29(struct flash_hw * flash_hw);
int write_protect_check_at29(struct flash_hw * flash_hw);
int write_protect_check_v29(struct flash_hw * flash_hw);
int write_protect_check_eon29F00x(struct flash_hw * flash_hw);
int write_protect_check_eon29F0x0(struct flash_hw * flash_hw);
int (*write_protect_check[])(struct flash_hw * flash_hw)={
	protect_check_unknown,
	write_protect_check_am29,
	write_protect_check_at29,
	protect_check_unknown,
	write_protect_check_v29,
	protect_check_unknown,
	protect_check_unknown,
	protect_check_unknown,
	protect_check_unknown,
	protect_check_unknown,
	protect_check_unknown,
	write_protect_check_eon29F00x,
	write_protect_check_eon29F0x0	};
__u8 hwmod_get_byte_unknown(struct flash_hw * flash_hw, __u32 addr);
__u8 hwmod_get_byte_i28a(struct flash_hw * flash_hw, __u32 addr);
__u8 hwmod_get_byte_sst39a(struct flash_hw * flash_hw, __u32 addr);
__u8 (*hwmod_get_byte[])(struct flash_hw * flash_hw, __u32 addr)={
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_i28a,
	hwmod_get_byte_unknown,
	hwmod_get_byte_sst39a,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown,
	hwmod_get_byte_unknown	};
int chip_erase_unknown(struct flash_hw * flash_hw);
int chip_erase_am29(struct flash_hw * flash_hw);
int chip_erase_at29(struct flash_hw * flash_hw);
int chip_erase_am28a(struct flash_hw * flash_hw);
int chip_erase_v29(struct flash_hw * flash_hw);
int chip_erase_cat28(struct flash_hw * flash_hw);
int chip_erase_i28a(struct flash_hw * flash_hw);
int chip_erase_i28(struct flash_hw * flash_hw);
int chip_erase_sst39a(struct flash_hw * flash_hw);
int chip_erase_sst39(struct flash_hw * flash_hw);
int chip_erase_w29(struct flash_hw * flash_hw);
int chip_erase_eon29F00x(struct flash_hw * flash_hw);
int chip_erase_eon29F0x0(struct flash_hw * flash_hw);
int (*chip_erase[])(struct flash_hw * flash_hw)={
	chip_erase_unknown,
	chip_erase_am29,
	chip_erase_at29,
	chip_erase_am28a,
	chip_erase_v29,
	chip_erase_cat28,
	chip_erase_i28a,
	chip_erase_i28,
	chip_erase_sst39a,
	chip_erase_sst39,
	chip_erase_w29,
	chip_erase_eon29F00x,
	chip_erase_eon29F0x0	};
int verify_id_unknown(struct flash_hw * flash_hw,__u16 id);
int verify_id_am29(struct flash_hw * flash_hw,__u16 id);
int verify_id_at29(struct flash_hw * flash_hw,__u16 id);
int verify_id_am28a(struct flash_hw * flash_hw,__u16 id);
int verify_id_v29(struct flash_hw * flash_hw,__u16 id);
int verify_id_cat28(struct flash_hw * flash_hw,__u16 id);
int verify_id_i28a(struct flash_hw * flash_hw,__u16 id);
int verify_id_i28(struct flash_hw * flash_hw,__u16 id);
int verify_id_sst39a(struct flash_hw * flash_hw,__u16 id);
int verify_id_sst39(struct flash_hw * flash_hw,__u16 id);
int verify_id_w29(struct flash_hw * flash_hw,__u16 id);
int verify_id_eon29F00x(struct flash_hw * flash_hw,__u16 id);
int verify_id_eon29F0x0(struct flash_hw * flash_hw,__u16 id);
int (*verify_id[])(struct flash_hw * flash_hw,__u16 id)={
	verify_id_unknown,
	verify_id_am29,
	verify_id_at29,
	verify_id_am28a,
	verify_id_v29,
	verify_id_cat28,
	verify_id_i28a,
	verify_id_i28,
	verify_id_sst39a,
	verify_id_sst39,
	verify_id_w29,
	verify_id_eon29F00x,
	verify_id_eon29F0x0	};
__u16 autoselect_unknown(struct flash_hw * flash_hw);
__u16 autoselect_am29(struct flash_hw * flash_hw);
__u16 autoselect_at29(struct flash_hw * flash_hw);
__u16 autoselect_am28a(struct flash_hw * flash_hw);
__u16 autoselect_v29(struct flash_hw * flash_hw);
__u16 autoselect_cat28(struct flash_hw * flash_hw);
__u16 autoselect_i28a(struct flash_hw * flash_hw);
__u16 autoselect_i28(struct flash_hw * flash_hw);
__u16 autoselect_sst39a(struct flash_hw * flash_hw);
__u16 autoselect_sst39(struct flash_hw * flash_hw);
__u16 autoselect_w29(struct flash_hw * flash_hw);
__u16 autoselect_eon29F00x(struct flash_hw * flash_hw);
__u16 autoselect_eon29F0x0(struct flash_hw * flash_hw);
__u16 (*autoselect[])(struct flash_hw * flash_hw)={
	autoselect_unknown,
	autoselect_am29,
	autoselect_at29,
	autoselect_am28a,
	autoselect_v29,
	autoselect_cat28,
	autoselect_i28a,
	autoselect_i28,
	autoselect_sst39a,
	autoselect_sst39,
	autoselect_w29,
	autoselect_eon29F00x,
	autoselect_eon29F0x0	};
