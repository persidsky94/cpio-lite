#ifndef CPIO_LITE_H
#define CPIO_LITE_H

#include <string>
#include <vector>

using namespace std;

/* not for use in C code, 'extern "C"' is only needed for function names not to be mangled */
/* sorry :/ */
extern "C" 
{

	/*throws CpioException, PosixException*/
	vector<string> getFilesList(const string& cpioArchivePath);

	/*throws CpioException, PosixException*/
	void unpackFile(const string& cpioArchivePath, const string& filename);

	/*throws CpioException, PosixException*/
	extern void archivateFiles(const vector<string>& filesToArchivate, const string& pathToNewarchive);

}

#endif //CPIO_LITE_H
