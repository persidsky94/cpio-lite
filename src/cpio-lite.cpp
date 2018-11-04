#include <fcntl.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "cpio-lite.h"


using header_val_type = unsigned short;

struct header_old_cpio {
	header_val_type c_magic;
	header_val_type c_dev;
	header_val_type c_ino;
	header_val_type c_mode;
	header_val_type c_uid;
	header_val_type c_gid;
	header_val_type c_nlink;
	header_val_type c_rdev;
	header_val_type c_mtime[2];
	header_val_type c_namesize;
	header_val_type c_filesize[2];
};





int getFilesList(const char* cpioArchivePath, char*** out_filenames, int* out_numFiles) {
	bool littleEndian = true;
	int err = 0;
	int fd = -1;
	ssize_t sz = -1;
	header_old_cpio header;

	fd = open(cpioArchivePath, O_RDONLY);
	if (fd == -1) {
		err = errno();
		goto rtrn;
	}

	std::vector<std::string> filenames;

	for(;;) {
		sz = read(fd, &header, sizeof(header_old_cpio));
		if (sz == -1) {
			err = errno();
			goto close_fd;
		}

		(header.c_magic == 0x71c7) ? littleEndian = false : littleEndian = true;


	}







close_fd:
	close(fd);
rtrn:
	return 0;
}



int unpackFile(const char* cpioArchivePath, const char* filename) {
	return 0;
}



int archivateFile(const char* fileToArchivate) {
	return 0;
}