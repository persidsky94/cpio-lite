#include <iostream>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "../include/cpio-lite.h"
#include "../include/customexceptions.h"

using namespace std;

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
	
	try
	{
		auto filenames = getFilesListFunc("./directory.cpio");
		
		for (const auto& filename : filenames)
			cout << filename << endl;
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

