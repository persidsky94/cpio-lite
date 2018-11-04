
/* allocates memory for filenames using malloc, free should be called for each filename
 * and for ptr to filenames */
int getFilesList(const char* cpioArchivePath, char*** out_filenames, int* out_numFiles);

int unpackFile(const char* cpioArchivePath, const char* filename);

int archivateFile(const char* fileToArchivate,);
