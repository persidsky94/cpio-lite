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
	auto archivateFilesFunc = (decltype(&archivateFiles)) dlsym(handle, "archivateFiles");
	if (archivateFilesFunc == nullptr) {
		cout << "Failed to find symbol 'archivateFiles': " << dlerror() << endl;
		return -1;
	}
	vector<string> filesToArchivate {"one", "two", "three"};
	string newArchiveName("onetwothree.cpio");
	try 
	{
		archivateFilesFunc(filesToArchivate, newArchiveName);
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