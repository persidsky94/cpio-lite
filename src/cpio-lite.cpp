#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "cpio.h"
#include "cpio-lite.h"
#include "customexceptions.h"
#include "descriptorwrapper.h"
#include "posixwrapper.h"
#include "util.h"

using namespace std;


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
		auto cFilename = make_unique<char[]>(realFilenameSize);

		sz = fd.readTo(cFilename.get(), realFilenameSize);
		string filename(cFilename.get());
		if (filename == "TRAILER!!!")
			break;
		else
			filenames.push_back(filename);

		auto bytesToSkip = getFileSizeFromBigEndianHeader(header);
		auto realBytesToSkip = bytesToSkip + bytesToSkip%2;
		fd.seek((off_t)realBytesToSkip, SEEK_CUR);
	}

	return filenames;
}


void set_stat(header_old_cpio &header, const string& filename) 
{
	PosixWrapper::lchown_(filename, header.c_uid, header.c_gid);
    PosixWrapper::chmod_(filename, header.c_mode);
}


void unpackFileInternal(header_old_cpio& header, DescriptorWrapper& fd, const string& filename)
{
	switch(header.c_mode & CP_IFMT) {
		case S_IFCHR:
    	case S_IFBLK:
    	case S_IFSOCK:
    	case S_IFIFO:
    	{
    		throw CpioException(CpioException::UnsupportedDeviceType, "Cannot unpack device file");
    		break;
    	}
		case S_IFDIR:
    	{
    		PosixWrapper::mkdir_(filename, header.c_mode);

    		try
    		{
    			set_stat(header, filename);
    		}
    		catch (exception& e)
    		{
    			rmdir(filename.c_str());
    			throw;
    		}

    		break;
    	}
    	case S_IFLNK:
    	{
    		auto pointedFilenameSize = getFileSizeFromBigEndianHeader(header);
    		auto cPointedFilename = make_unique<char[]>(pointedFilenameSize);
    		fd.readTo(cPointedFilename.get(), pointedFilenameSize);
    		/* symlink body in cpio archive may not have null-terminator, so use   
    		 * string constructor from buffer with size */
    		string pointedFilename(cPointedFilename.get(), pointedFilenameSize);
    		PosixWrapper::symlink_(pointedFilename, filename);

    		try
    		{
    			PosixWrapper::lchown_(filename, header.c_uid, header.c_gid);
    		}
    		catch (exception& e)
    		{
    			unlink(filename.c_str());
    			throw;
    		}

    		break;
    	}
    	case S_IFREG:
    	{
			auto fileContentSize = getFileSizeFromBigEndianHeader(header);
			auto unpackedFd = DescriptorWrapper::createFile(filename, O_WRONLY | O_CREAT, ACCESS_RW_RW_R__);

			try
			{
				auto leftToCopy = fileContentSize;
				auto bufPtr = make_unique<char[]>(BUFFER_BLOCK);
				while (leftToCopy > 0)
				{
					auto sz = fd.readTo(bufPtr.get(), min(BUFFER_BLOCK, leftToCopy));
					unpackedFd.writeFrom(bufPtr.get(), sz);
					leftToCopy -= sz;
				}

				set_stat(header, filename);
			}
			catch (exception& e)
    		{
    			unlink(filename.c_str());
    			throw;
    		}

			break;
		}
		default:
		{
			throw CpioException(CpioException::InvalidInputFileToArchive, "Unknown filetype");
		}
	}
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
		auto cFilename = make_unique<char[]>(realFilenameSize);

		sz = fd.readTo(cFilename.get(), realFilenameSize);
		string filenameFromArchive(cFilename.get());
		if (filenameFromArchive == "TRAILER!!!") {
			break;
		}
		else if (filenameFromArchive == filename) {
			unpackFileInternal(header, fd, filenameFromArchive);
			return;
		}

		auto bytesToSkip = getFileSizeFromBigEndianHeader(header);
		auto realBytesToSkip = bytesToSkip + bytesToSkip%2;
		fd.seek((off_t)realBytesToSkip, SEEK_CUR);
	}

	throw CpioException(CpioException::FileNotFoundInArchive, "Couldn't find specified file in archive");
	return;
}


void archivateFiles(const vector<string>& filesToArchivate, const string& pathToOutArchive) 
{
	// check all files can be opened
	for (const auto& filename: filesToArchivate) {
		DescriptorWrapper fd = DescriptorWrapper::openFile(filename, O_RDONLY);
	}

	auto fdArch = DescriptorWrapper::createFile(pathToOutArchive, O_WRONLY | O_CREAT, ACCESS_RW_RW_R__);
	header_old_cpio header;
	char additionalNullTerminator = 0;
	struct stat st;

	try
	{
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
		    		auto bufPtr = make_unique<char[]>(st.st_size);
	    			auto size = PosixWrapper::readlink_(filename, bufPtr.get(), st.st_size);
	    			fdArch.writeFrom(bufPtr.get(), size);
	    			if (size % 2 == 1)
	    				fdArch.writeFrom(&additionalNullTerminator, 1);
		    		
		    		break;
		    	}
		    	case S_IFREG:
		    	{
					auto fdToArchivate = DescriptorWrapper::openFile(filename, O_RDONLY); 
					auto leftToCopy = st.st_size;
					auto bufPtr = make_unique<char[]>(BUFFER_BLOCK);
					while (leftToCopy > 0)
					{
						auto sz = fdToArchivate.readTo(bufPtr.get(), min<unsigned long>(BUFFER_BLOCK, leftToCopy));
						fdArch.writeFrom(bufPtr.get(), sz);
						leftToCopy -= sz;
					}
					if (st.st_size % 2 == 1)
						fdArch.writeFrom(&additionalNullTerminator, 1);
					
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
		unlink(pathToOutArchive.c_str());
		throw;
	}

	return;
}
