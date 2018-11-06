#ifndef CPIO_H
#define CPIO_H

#define BIG_ENDIAN_MAGIC 0x71c7;
#define LITTLE_ENDIAN_MAGIC 0xc771;

#define CP_IFMT 0170000

struct header_old_cpio {
	unsigned short c_magic;
	unsigned short c_dev;
	unsigned short c_ino;
	unsigned short c_mode;
	unsigned short c_uid;
	unsigned short c_gid;
	unsigned short c_nlink;
	unsigned short c_rdev;
	unsigned short c_mtime[2];
	unsigned short c_namesize;
	unsigned short c_filesize[2];
};

#endif //CPIO_H
