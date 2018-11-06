#ifndef DESCRIPTORWRAPPER_H
#define DESCRIPTORWRAPPER_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define ACCESS_RW_RW_R__ 0664

class DescriptorWrapper {
public:
	static DescriptorWrapper openFile(const string& filename, int flags);

	static DescriptorWrapper createFile(const string& filename, int flags, mode_t mode);

	ssize_t readTo(void *buf, size_t count);

	ssize_t writeFrom(void *buf, size_t count);

	off_t seek(off_t offset, int whence);

	~DescriptorWrapper();

	operator int() const { return m_fd; }

private:
	DescriptorWrapper(int fd)
		: m_fd(fd)
	{}

	int m_fd;
};

#endif //DESCRIPTORWRAPPER_H
