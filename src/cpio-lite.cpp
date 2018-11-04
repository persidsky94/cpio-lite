#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "cpio-lite.h"
#include "customexceptions.h"


using namespace std;
using header_val_type = unsigned short;

#define BIG_ENDIAN_MAGIC 0x71c7;
#define LITTLE_ENDIAN_MAGIC 0xc771;

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

bool isHeaderLittleEndian(const header_old_cpio& header) 
{
	if (header.c_magic == 0xc771) 
		return true;
	else if(header.c_magic == 0x71c7) 
		return false;
	else 
		throw CpioException(CpioException::InvalidInputArchive, "Wrong header magic value");
}

void switchEndianness(header_old_cpio& header) 
{
	auto tmp = header.c_mtime[0];
	header.c_mtime[0] = header.c_mtime[1];
	header.c_mtime[1] = tmp;
	
	tmp = header.c_filesize[0];
	header.c_filesize[0] = header.c_filesize[1];
	header.c_filesize[1] = tmp;
}

size_t getFileSizeFromBigEndianHeader(const header_old_cpio& header) 
{
	return (((size_t)header.c_filesize[0])<<(sizeof(header_val_type)*CHAR_BIT)) + (size_t)header.c_filesize[1];
}

size_t getFilenameSizeFromBigEndianHeader(const header_old_cpio& header)
{
	return header.c_namesize;
}


class DescriptorWrapper {
public:
	static DescriptorWrapper openFile(const string& filename)
	{
		int fd = open(filename.c_str(), O_RDONLY);
		if (fd == -1) 
			throw PosixException(errno, "Couldn't open file " + filename);
		return DescriptorWrapper(fd);
	}

	ssize_t readTo(void *buf, size_t count)
	{
		ssize_t sz = read(m_fd, buf, count);
		if (sz == -1)
			throw PosixException(errno, "Couldn't read from file");
		return sz;
	}

	off_t seek(off_t offset, int whence) 
	{
		off_t offt = lseek(m_fd, offset, whence);
		if (offt == (off_t)(-1))
			throw PosixException(errno, "Error while lseek");
		return offt;
	}

	~DescriptorWrapper() 
	{
		if (m_fd != -1) 
			close(m_fd);
	}

	operator int() const { return m_fd; }

private:
	DescriptorWrapper(int fd)
		: m_fd(fd)
	{}

	int m_fd;
};


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
	fd.readTo(&(header.c_magic), sizeof(header_val_type));
	fd.readTo(&(header.c_dev), sizeof(header_val_type));
	fd.readTo(&(header.c_ino), sizeof(header_val_type));
	fd.readTo(&(header.c_mode), sizeof(header_val_type));
	fd.readTo(&(header.c_uid), sizeof(header_val_type));
	fd.readTo(&(header.c_gid), sizeof(header_val_type));
	fd.readTo(&(header.c_nlink), sizeof(header_val_type));
	fd.readTo(&(header.c_rdev), sizeof(header_val_type));
	fd.readTo(&(header.c_mtime[0]), sizeof(header_val_type));
	fd.readTo(&(header.c_mtime[1]), sizeof(header_val_type));
	fd.readTo(&(header.c_namesize), sizeof(header_val_type));
	fd.readTo(&(header.c_filesize[0]), sizeof(header_val_type));
	fd.readTo(&(header.c_filesize[1]), sizeof(header_val_type));
}


vector<string> getFilesList(const string& cpioArchivePath) 
{
	ssize_t sz = -1;
	header_old_cpio header;
	std::vector<std::string> filenames;

	DescriptorWrapper fd = DescriptorWrapper::openFile(cpioArchivePath);

	for(;;) {
		sz = fd.readTo(&header, sizeof(header));
		if (sz == 0)
			throw CpioException(CpioException::InvalidInputArchive, "Unexpected EOF");
		if (isHeaderLittleEndian(header))
			switchEndianness(header);
		
		auto filenameSize = getFilenameSizeFromBigEndianHeader(header);
		auto realFilenameSize = filenameSize + filenameSize%2;
		char* cFilename = (char*)malloc(realFilenameSize);
		try 
		{
			sz = fd.readTo(cFilename, realFilenameSize);
			string filename(cFilename);
			if (filename == "TRAILER!!!")
				break;
			else
				filenames.push_back(filename);

		}
		catch (exception& e) 
		{
			free(cFilename);
			throw e;
		}
		free(cFilename);

		size_t bytesToSkip = getFileSizeFromBigEndianHeader(header);
		fd.seek((off_t)bytesToSkip, SEEK_CUR);
	}

	return filenames;
}



void unpackFile(const string& cpioArchivePath, const string& filename) 
{
	return;
}


void archivateFile(const string& fileToArchivate, const string& pathToNewarchive) 
{
	return;
}
