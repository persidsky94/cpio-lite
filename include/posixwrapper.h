#ifndef POSIXWRAPPER_H
#define POSIXWRAPPER_H

#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

using namespace std;

class PosixWrapper {
public:	
	static int lstat_(const string& filename, struct stat& st);

	static ssize_t readlink_(const string& filename, char* buf, size_t bufSize);

	static int mkdir_(const string& filename, mode_t mode);

	static int lchown_(const string& filename, uid_t owner, gid_t group);

	static int chmod_(const string& filename, mode_t mode);

	static int symlink_(const string& filename, const string& symlinkFilename);
};

#endif //POSIXWRAPPER_H