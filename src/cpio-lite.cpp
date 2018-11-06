#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include "cpio.h"
#include "cpio-lite.h"
#include "customexceptions.h"
#include "descriptorwrapper.h"
#include "util.h"

using namespace std;


class PosixWrapper {
public:	
	static int lstat_(const string& filename, struct stat& st)
	{
		auto res = lstat(filename.c_str(), &st);
		if (res == -1)
			throw PosixException(errno, "Error while lstat");
		return res;
	}

	static ssize_t readlink_(const string& filename, char* buf, size_t bufSize)
	{
		auto res = readlink(filename.c_str(), buf, bufSize);
		if (res == -1)
			throw PosixException(errno, "Error while readlink");
		return res;
	}
};


vector<string> getFilesList(const string& cpioArchivePath) 
{
	ssize_t sz = -1;
	header_old_cpio header;
	std::vector<std::string> filenames;

	DescriptorWrapper fd = DescriptorWrapper::openFile(cpioArchivePath, O_RDONLY);

	for(;;) {
		sz = fd.readTo(&header, sizeof(header));
		if (sz == 0)
			throw CpioException(CpioException::InvalidInputArchive, "Unexpected EOF");
		if (isHeaderLittleEndian(header))
			switchEndianness(header);
		
		auto filenameSize = getFilenameSizeFromBigEndianHeader(header);
		auto realFilenameSize = filenameSize + filenameSize%2;
		char* cFilename = (char*)malloc(realFilenameSize);
		try 
		{
			sz = fd.readTo(cFilename, realFilenameSize);
			string filename(cFilename);
			if (filename == "TRAILER!!!")
				break;
			else
				filenames.push_back(filename);

		}
		catch (exception& e) 
		{
			free(cFilename);
			throw;
		}
		free(cFilename);

		auto bytesToSkip = getFileSizeFromBigEndianHeader(header);
		auto realBytesToSkip = bytesToSkip + bytesToSkip%2;
		fd.seek((off_t)realBytesToSkip, SEEK_CUR);
	}

	return filenames;
}



void unpackFile(const string& cpioArchivePath, const string& filename) 
{
	ssize_t sz = -1;
	header_old_cpio header;


	DescriptorWrapper fd = DescriptorWrapper::openFile(cpioArchivePath, O_RDONLY);

	for(;;) {
		sz = fd.readTo(&header, sizeof(header));
		if (sz == 0)
			throw CpioException(CpioException::InvalidInputArchive, "Unexpected EOF");
		if (isHeaderLittleEndian(header))
			switchEndianness(header);

		auto filenameSize = getFilenameSizeFromBigEndianHeader(header);
		auto realFilenameSize = filenameSize + filenameSize%2;
		char* cFilename = (char*)malloc(realFilenameSize);
		try 
		{
			sz = fd.readTo(cFilename, realFilenameSize);
			string filename(cFilename);
			if (filename == "TRAILER!!!") {
				break;
			}
			else if (filename == filename) {
				
				// TODO: unpack file (hard, eh?)
			}

		}
		catch (exception& e) 
		{
			free(cFilename);
			throw;
		}
		free(cFilename);
	}

	return;
}


void archivateFiles(const vector<string>& filesToArchivate, const string& pathToNewarchive) 
{
	constexpr size_t BUFFER_BLOCK=512;
	// check all files can be opened
	for (const auto& filename: filesToArchivate) {
		DescriptorWrapper fd = DescriptorWrapper::openFile(filename, O_RDONLY);
	}

	auto fdArch = DescriptorWrapper::createFile(pathToNewarchive, O_WRONLY | O_CREAT, ACCESS_RW_RW_R__);
	header_old_cpio header;
	char additionalNullTerminator = 0;
	struct stat st;

	try
	{
		// write all files
		for (const auto& filename: filesToArchivate) {
			PosixWrapper::lstat_(filename, st);
			fillInHeader(header, st, filename);
			// write header 
			fdArch.writeFrom(&header, sizeof(header_old_cpio));
			// write filename
			fdArch.writeFrom((void*)filename.c_str(), header.c_namesize);
			if (header.c_namesize % 2 == 1)
				fdArch.writeFrom(&additionalNullTerminator, 1);
			// write file content 
			switch (header.c_mode & CP_IFMT) {
		    	case S_IFDIR:
				case S_IFCHR:
		    	case S_IFBLK:
		    	case S_IFSOCK:
		    	case S_IFIFO:
		    	{
		    		break;
		    	}
		    	case S_IFLNK:
		    	{
		    		char* buf = (char*)malloc(st.st_size);
		    		try {
		    			auto size = PosixWrapper::readlink_(filename, buf, st.st_size);
		    			fdArch.writeFrom(buf, size);
		    			if (size % 2 == 1)
		    				fdArch.writeFrom(&additionalNullTerminator, 1);
		    		}
		    		catch (exception& e) {
		    			free(buf);
		    			throw;
		    		}
		    		free(buf);
		    		break;
		    	}
		    	case S_IFREG:
		    	{
					auto fdToArchivate = DescriptorWrapper::openFile(filename, O_RDONLY); 
					auto leftToCopy = st.st_size;
					void* buf = (void*)malloc(BUFFER_BLOCK);
					try 
					{
						while (leftToCopy > 0)
						{
							auto sz = fdToArchivate.readTo(buf, BUFFER_BLOCK);
							fdArch.writeFrom(buf, sz);
							leftToCopy -= sz;
						}
						if (st.st_size % 2 == 1)
							fdArch.writeFrom(&additionalNullTerminator, 1);
					}
					catch (exception& e) 
					{
						free(buf);
						throw;
					}
					free(buf);
					break;
				}
				default:
				{
					throw CpioException(CpioException::InvalidInputFileToArchive, "Unknown filetype");
				}
			}
		}

		writeTrailer(fdArch);
	}
	catch (exception& e) 
	{
		unlink(pathToNewarchive.c_str());
		throw;
	}

	return;
}
