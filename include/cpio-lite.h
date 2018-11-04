#include <string>
#include <vector>

using namespace std;

/*throws CpioException*/
vector<string> getFilesList(const string& cpioArchivePath);

/*throws CpioException*/
void unpackFile(const string& cpioArchivePath, const string& filename);

/*throws CpioException*/
void archivateFile(const string& fileToArchivate, const string& pathToNewarchive);
