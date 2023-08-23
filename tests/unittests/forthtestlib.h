#include <string>

void RunAndCompare(const std::string &command, const std::string &result,bool trace=false);
void RunAndPrint(const std::string &command, std::string &result,
    const std::string &filename);
void RunCommandAndPrint(const char *command,
    const char *filename[], int numberOfTestFiles);
void RunAndPrint(const char *command[],
    const char *filename[], int numberOfTestFiles, int Start=0);
            
