#include <iostream>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "cpio-lite.h"
#include "customexceptions.h"

using namespace std;

void getAndPrintFilesList(decltype(&getFilesList) getFilesListFunc, const string& archivePath)
{
	auto filenames = getFilesListFunc(archivePath);
	cout << "Archive " << archivePath << " contains files:" << endl;
	for (const auto& filename : filenames)
		cout << "	" << filename << endl;
}

int main() 
{
	void* handle = dlopen("../libcpio-lite.so", RTLD_NOW);
	if (handle == nullptr) {
		cout << "Failed to load cpio-lite.so: " << dlerror() << endl;		
		return -1;
	}
	auto getFilesListFunc = (decltype(&getFilesList)) dlsym(handle, "getFilesList");
	if (getFilesListFunc == nullptr) {
		cout << "Failed to find symbol 'getFilesList': " << dlerror() << endl;
		return -1;
	}

	vector<string> archivePaths{"./testfiles/cpio_archives/directory.cpio", "./testfiles/cpio_archives/onetwothree.cpio", "./testfiles/cpio_archives/links_dirs_regs.cpio"};	

	try
	{
		for (const auto& archivePath : archivePaths)
			getAndPrintFilesList(getFilesListFunc, archivePath);
		
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
}

