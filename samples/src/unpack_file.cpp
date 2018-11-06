#include <iostream>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "cpio-lite.h"
#include "customexceptions.h"

using namespace std;

int main() 
{
	void* handle = dlopen("../libcpio-lite.so", RTLD_NOW);
	if (handle == nullptr) {
		cout << "Failed to load cpio-lite.so: " << dlerror() << endl;
		return -1;
	}
	auto unpackFileFunc = (decltype(&unpackFile)) dlsym(handle, "unpackFile");
	if (unpackFileFunc == nullptr) {
		cout << "Failed to find symbol 'unpackFile': " << dlerror() << endl;
		return -1;
	}
	auto getFilesListFunc = (decltype(&getFilesList)) dlsym(handle, "getFilesList");
	if (getFilesListFunc == nullptr) {
		cout << "Failed to find symbol 'getFilesList': " << dlerror() << endl;
		return -1;
	}

	vector<string> archivePaths{"./testfiles/cpio_archives/links_dirs_regs.cpio"};

	try 
	{
		for (const auto& archivePath : archivePaths) {
			cout << "Unpacking all files from archive " << archivePath << ":" << endl;
			auto filesList = getFilesListFunc(archivePath);
			for (const auto& file: filesList) {
				unpackFileFunc(archivePath, file);
				cout << "	Unpacked file " << file << endl;
			}
			cout << "Finished unpacking all files from archive " << archivePath << endl;
		}
	}
	catch (CpioException& e)
	{
		cout << e.what() << ", error type: " << e.errorType() << endl;
		return -1;
	}
	catch (PosixException& e)
	{
		cout << e.what() << ", error code: " << e.errorCode() << endl;
		return -1;
	}
	return 0;
}