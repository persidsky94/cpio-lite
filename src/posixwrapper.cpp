#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "customexceptions.h"
#include "posixwrapper.h"

using namespace std;


int PosixWrapper::lstat_(const string& filename, struct stat& st)
{
	auto res = lstat(filename.c_str(), &st);
	if (res == -1)
		throw PosixException(errno, "Error while lstat");
	return res;
}

ssize_t PosixWrapper::readlink_(const string& filename, char* buf, size_t bufSize)
{
	auto res = readlink(filename.c_str(), buf, bufSize);
	if (res == -1)
		throw PosixException(errno, "Error while readlink");
	return res;
}

int PosixWrapper::mkdir_(const string& filename, mode_t mode)
{
	auto res = mkdir(filename.c_str(), mode);
	if (res == -1)
		throw PosixException(errno, "Error while mkdir");
	return res;
}

int PosixWrapper::lchown_(const string& filename, uid_t owner, gid_t group)
{
	auto res = lchown(filename.c_str(), owner, group);
	if (res == -1)
		throw PosixException(errno, "Error while lchown");
	return res;
}

int PosixWrapper::chmod_(const string& filename, mode_t mode)
{
	auto res = chmod(filename.c_str(), mode);
	if (res == -1)
		throw PosixException(errno, "Error while chmod");
	return res;
}

int PosixWrapper::symlink_(const string& filename, const string& symlinkFilename)
{
	auto res = symlink(filename.c_str(), symlinkFilename.c_str());
	if (res == -1)
		throw PosixException(errno, "Error while symlink");
	return res;
}