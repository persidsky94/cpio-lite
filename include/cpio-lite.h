#include <string>
#include <vector>

using namespace std;

/*throws CpioException*/
__attribute__ ((visibility ("default"))) vector<string> getFilesList(const string& cpioArchivePath);

/*throws CpioException*/
__attribute__ ((visibility ("default"))) void unpackFile(const string& cpioArchivePath, const string& filename);

/*throws CpioException*/
__attribute__ ((visibility ("default"))) void archivateFile(const string& fileToArchivate, const string& pathToNewarchive);
