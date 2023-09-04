

#include <gtest/gtest.h>
#include <string>
#include <iostream>

#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include "forthtestlib.h"

const char* allfilenames[] = {
	"./testresult/words_status0.txt", // 1
	"./testresult/words_status.txt",  // 1 
};

const char* allFilesInSuite[] = {
	"words-check.frt" ,				// 1
	"words-check.frt" ,				// 1
	""
};

TEST(TestSuit1, ForthTestSuit023IncludeAllFileSTest)
{
	RunAndPrint(allFilesInSuite, allfilenames, 2,1); // read and execute 1 file 
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

