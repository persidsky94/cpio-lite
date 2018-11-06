#include "customexceptions.h"
#include "descriptorwrapper.h"

using namespace std;

DescriptorWrapper DescriptorWrapper::openFile(const string& filename, int flags)
{
	int fd = open(filename.c_str(), flags);
	if (fd == -1) 
		throw PosixException(errno, "Couldn't open file " + filename);
	return DescriptorWrapper(fd);
}

DescriptorWrapper DescriptorWrapper::createFile(const string& filename, int flags, mode_t mode)
{
	int fd = open(filename.c_str(), flags, mode);
	if (fd == -1) 
		throw PosixException(errno, "Couldn't open file " + filename);
	return DescriptorWrapper(fd);
}

ssize_t DescriptorWrapper::readTo(void *buf, size_t count)
{
	ssize_t sz = read(m_fd, buf, count);
	if (sz == -1)
		throw PosixException(errno, "Couldn't read from file");
	return sz;
}

ssize_t DescriptorWrapper::writeFrom(void *buf, size_t count)
{
	ssize_t sz = write(m_fd, buf, count);
	if (sz == -1)
		throw PosixException(errno, "Couldn't write to file");
	return sz;

}

off_t DescriptorWrapper::seek(off_t offset, int whence) 
{
	off_t offt = lseek(m_fd, offset, whence);
	if (offt == (off_t)(-1))
		throw PosixException(errno, "Error while lseek");
	return offt;
}

DescriptorWrapper::~DescriptorWrapper() 
{
	if (m_fd != -1) 
		close(m_fd);
}
