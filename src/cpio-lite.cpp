#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

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
	if (header.c_magic == LITTLE_ENDIAN_MAGIC) 
		return true; 
	else if(header.c_magic == BIG_ENDIAN_MAGIC)
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
	return ((size_t)header.c_filesize[0])<<(sizeof(header_val_type)*CHAR_BITS) + (size_t)header.c_filesize[1];
}

size_t getFilenameSizeFromBigEndianHeader(const header_old_cpio& header)
{
	return header.c_namesize;
}




class DescriptorWrapper {
public:
	static DescriptorWrapper openFile(const string& filename)
	{
		int fd = open(cpioArchivePath, O_RDONLY);
		if (fd == -1) 
			throw PosixException(errno(), "Couldn't open file " + filename);
		return DescriptorWrapper(fd);
	}

	ssize_t readTo(void *buf, size_t count)
	{
		ssize_t sz = read(m_fd, buf, count);
		if (sz == -1)
			throw PosixException(errno(), "Couldn't read from file");
		return sz;
	}

	off_t seek(off_t offset, int whence) 
	{
		off_t offt = lseek(m_fd, offset, whence);
		if (offt == (off_t)(-1))
			throw PosixException(errno(), "Error while lseek");
		return offt;
	}

	
	operator int() const { return m_fd; }

private:
	DescriptorWrapper(int fd)
		: m_fd(fd)
	{}

	~DescriptorWrapper() 
	{
		if (m_fd != -1) 
			close(m_fd);
	}

	int m_fd;
};


vector<string> getFilesList(const string& cpioArchivePath) 
{
	ssize_t sz = -1;
	header_old_cpio header;
	std::vector<std::string> filenames;

	DescriptorWrapper fd = openFile(cpioArchivePath);

	for(;;) {
		sz = fd.readTo(&header, sizeof(header_old_cpio));
		if (sz == 0)
			throw CpioException(CpioException::InvalidInputArchive, "Unexpected EOF");
		if (isHeaderLittleEndian(header))
			switchEndianness(header);
		
		auto filenameSize = getFilenameSizeFromBigEndianHeader(header);
		char* cFilename = (char*)malloc(filenameSize + filenameSize%2);
		try 
		{
			sz = fd.readTo(cFilename, filenameSize);
			string filename(cFilename);
			if (filename == "TRAILER!!!")
				break;
			else
				filenames.push_back(filename);

		}
		catch (exception& e) 
		{
			free cFilename;
			throw e;
		}
		free cFilename;

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
