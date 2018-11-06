#include <iostream>

#include "customexceptions.h"
#include "descriptorwrapper.h"
#include "util.h"

using namespace std;

bool isHeaderLittleEndian(const header_old_cpio& header) 
{
	if (header.c_magic == 0xc771) 
		return true;
	else if(header.c_magic == 0x71c7) 
		return false;
	else 
		throw CpioException(CpioException::InvalidInputArchive, "Wrong header magic value");
}

void swapBytes(unsigned short& twoByteValue) 
{
	twoByteValue = ((twoByteValue & 0x0f) << 8) + (twoByteValue >> 8);
}

void switchEndianness(header_old_cpio& header) 
{
	swapBytes(header.c_magic);
	swapBytes(header.c_dev);
	swapBytes(header.c_ino);
	swapBytes(header.c_mode);
	swapBytes(header.c_uid);
	swapBytes(header.c_gid);
	swapBytes(header.c_nlink);
	swapBytes(header.c_rdev);
	swapBytes(header.c_mtime[0]);
	swapBytes(header.c_mtime[1]);
	swapBytes(header.c_namesize);
	swapBytes(header.c_filesize[0]);
	swapBytes(header.c_filesize[1]);
}

size_t getFileSizeFromBigEndianHeader(const header_old_cpio& header) 
{
	return (((size_t)header.c_filesize[0])<<(sizeof(unsigned short)*CHAR_BIT)) + (size_t)header.c_filesize[1];
}

size_t getFilenameSizeFromBigEndianHeader(const header_old_cpio& header)
{
	return header.c_namesize;
}

void printHeader(const header_old_cpio& header)
{
	cout << hex << header.c_magic << endl;
	cout << hex << header.c_dev << endl;
	cout << hex << header.c_ino << endl;
	cout << hex << header.c_mode << endl;
	cout << hex << header.c_uid << endl;
	cout << hex << header.c_gid << endl;
	cout << hex << header.c_nlink << endl;
	cout << hex << header.c_rdev << endl;
	cout << hex << header.c_mtime[0] << endl;
	cout << hex << header.c_mtime[1] << endl;
	cout << hex << header.c_namesize << endl;
	cout << hex << header.c_filesize[0] << endl;
	cout << hex << header.c_filesize[1] << endl;
}

void readToHeader(DescriptorWrapper& fd, header_old_cpio& header)
{
	fd.readTo(&(header.c_magic), sizeof(unsigned short));
	fd.readTo(&(header.c_dev), sizeof(unsigned short));
	fd.readTo(&(header.c_ino), sizeof(unsigned short));
	fd.readTo(&(header.c_mode), sizeof(unsigned short));
	fd.readTo(&(header.c_uid), sizeof(unsigned short));
	fd.readTo(&(header.c_gid), sizeof(unsigned short));
	fd.readTo(&(header.c_nlink), sizeof(unsigned short));
	fd.readTo(&(header.c_rdev), sizeof(unsigned short));
	fd.readTo(&(header.c_mtime[0]), sizeof(unsigned short));
	fd.readTo(&(header.c_mtime[1]), sizeof(unsigned short));
	fd.readTo(&(header.c_namesize), sizeof(unsigned short));
	fd.readTo(&(header.c_filesize[0]), sizeof(unsigned short));
	fd.readTo(&(header.c_filesize[1]), sizeof(unsigned short));
}

void fillInHeader(header_old_cpio& header, struct stat& st, const string& filename)
{
	header.c_magic = 0x71c7;
	header.c_dev = (unsigned short) st.st_dev;
	header.c_ino = (unsigned short) st.st_ino;
	header.c_mode = (unsigned short) st.st_mode;
	header.c_uid = (unsigned short) st.st_uid;
	header.c_gid = (unsigned short) st.st_gid;
	header.c_nlink = (unsigned short) st.st_nlink;
	header.c_rdev = (unsigned short) st.st_rdev;
	header.c_mtime[0] = (unsigned short) (st.st_mtime >> sizeof(unsigned short)*CHAR_BIT);
	header.c_mtime[1] = (unsigned short) (st.st_mtime);
	header.c_namesize = (unsigned short) (filename.length() + 1);
	switch (header.c_mode & CP_IFMT) {
	    case S_IFDIR:
		case S_IFCHR:
	    case S_IFBLK:
	    case S_IFSOCK:
	    case S_IFIFO:
	    	header.c_filesize[0] = 0;
	    	header.c_filesize[1] = 0;
	    	break;
	    case S_IFLNK:
	    case S_IFREG:
			header.c_filesize[0] = (unsigned short) (st.st_size >> sizeof(unsigned short)*CHAR_BIT); // for filesizes <= 2Gb
			header.c_filesize[1] = (unsigned short) (st.st_size);
			break;
		default:
			throw CpioException(CpioException::InvalidInputFileToArchive, "Unknown filetype");
	}
}

void writeTrailer(DescriptorWrapper fd)
{
	header_old_cpio header;
	header.c_magic = 0x71c7;
	header.c_dev = 0;
	header.c_ino = 0;
	header.c_mode = 0;
	header.c_uid = 0;
	header.c_gid = 0;
	header.c_nlink = 1;
	header.c_rdev = 0;
	header.c_mtime[0] = 0;
	header.c_mtime[1] = 0;
	header.c_namesize = 11;
	header.c_filesize[0] = 0;
	header.c_filesize[1] = 0;

	fd.writeFrom(&header, sizeof(header_old_cpio));
	string trailerFilename("TRAILER!!!");
	char additionalNullTerminator = 0;
	fd.writeFrom((void*)trailerFilename.c_str(), header.c_namesize);
	if (header.c_namesize % 2 == 1)
		fd.writeFrom(&additionalNullTerminator, 1);
}