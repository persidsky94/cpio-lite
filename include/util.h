#ifndef UTIL_H
#define UTIL_H

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include "cpio.h"

using namespace std;

constexpr size_t BUFFER_BLOCK=512;

bool isHeaderLittleEndian(const header_old_cpio& header);

void swapBytes(unsigned short& twoByteValue);

void switchEndianness(header_old_cpio& header);

size_t getFileSizeFromBigEndianHeader(const header_old_cpio& header);

size_t getFilenameSizeFromBigEndianHeader(const header_old_cpio& header);

void printHeader(const header_old_cpio& header);

void readToHeader(DescriptorWrapper& fd, header_old_cpio& header);

void fillInHeader(header_old_cpio& header, struct stat& st, const string& filename);

void writeTrailer(DescriptorWrapper fd);


#endif //UTIL_H
