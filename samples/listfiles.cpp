#include <iostream>
#include <string>
#include <vector>

#include <dlfcn.h>

#include "../include/cpio-lite.h"

using namespace std;

int main() 
{
	void* handle = dlopen("../libcpio-lite.so", RTLD_NOW);
	if (handle == nullptr) {
		cout << "Failed to load cpio-lite.so" << endl;
		return -1;
	}
	vector<string> (*listFilesFunc) (const string&) = (vector<string> (*) (const string&)) dlsym(handle, "_Z12getFilesListRKNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE");
	if (listFilesFunc == nullptr) {
		cout << "Failed to find symbol 'getFilesList': " << dlerror() << endl;
		return -1;
	}
	auto filenames = listFilesFunc("/home/pers/code/getmobit/cpio-lite/samples/directory.cpio");
	for (const auto& filename : filenames)
	{
		cout << filename << endl;
	}
}

