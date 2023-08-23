

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

	const char *filenames[] = { 
        "../testresult/test1.txt",
		"../testresult/test2.txt",
		"../testresult/test3.txt",
		"../testresult/test4.txt",
		"../testresult/test5.txt",
		"../testresult/test6.txt",
		"../testresult/test7.txt",
		"../testresult/test8.txt",
		"../testresult/test9.txt",
		"../testresult/test10.txt",
		"../testresult/test11.txt",
		"../testresult/test12.txt",
		"../testresult/test13.txt",
        "../testresult/test14.txt",
        "../testresult/test15.txt",
        "../testresult/test16.txt",
        "../testresult/test17.txt",
        "../testresult/test18.txt",
	};
	const char *infilenames[] = {
		"../suit1/prelimtest.fth",
		"../suit1/ttester.fs",
		"../suit1/core.fr",
		"../suit1/coreplustest.fth",
		"../suit1/utilities.fth",
		"../suit1/errorreport.fth",
		"../suit1/memorytest.fth",
		"../suit1/exceptiontest.fth",
		"../suit1/fp/fpzero-test.4th",
		"../suit1/fp/ak-fp-test.fth",
		"../suit1/fp/paranoia.4th",
    	"../suit1/tester.fr",
        "../suit1/coreexttest.fth",
	    "../suit1/filetest.fth",
		"../suit1/runtestsall.fth",

      	"",
	};
	const char* allfilenames[] = {
		"../testresult/alltests1.txt",
		"../testresult/alltests2.txt",
		"../testresult/alltests3.txt",
		"../testresult/alltests4.txt",
		"../testresult/alltests5.txt",
		"../testresult/alltests6.txt",
		"../testresult/alltests7.txt",
		"../testresult/alltests8.txt",
		"../testresult/alltests9.txt",
		"../testresult/alltests10.txt",
		"../testresult/alltests11.txt",
		"../testresult/alltests12.txt",
		"../testresult/alltests13.txt",
		"../testresult/alltests14.txt",
		"../testresult/alltests15.txt",
		"../testresult/alltests16.txt",
		"../testresult/alltests17.txt",
		"../testresult/alltests18.txt",
	};

	const char* allFilesInSuite[] = {
	"../suit1/prelimtest.fth" ,
		"../suit1/tester.fr" ,
//		"../suit1/ttester.fs" ,
		"../suit1/core.fr" ,
		"../suit1/coreplustest.fth" ,
		"../suit1/utilities.fth" ,
		"../suit1/errorreport.fth" ,
		"../suit1/coreexttest.fth" ,
		"../suit1/blocktest.fth" ,
		"../suit1/doubletest.fth" ,
		"../suit1/exceptiontest.fth" ,
		"../suit1/facilitytest.fth" ,
		"../suit1/filetest.fth" ,
		"../suit1/localstest.fth" ,
		"../suit1/memorytest.fth" ,
		"../suit1/toolstest.fth" ,
		"../suit1/searchordertest.fth" ,
		"../suit1/stringtest.fth" ,
		""
	};

TEST(TestSuit1All, ForthTestSuit000AllTestsInOnce)
{
	RunAndPrintSuite(allFilesInSuite, allfilenames, 17);
}
TEST(TestSuit1, ForthTestSuit001Prelim)
{
    RunAndPrint(infilenames, filenames, 1);
}
TEST(TestSuit1,ForthTestSuit002Tester)
{
    RunAndPrint(infilenames, filenames, 2);
}
TEST(TestSuit1,ForthTestSuit003Core)
{
    RunAndPrint(infilenames, filenames, 3);
}
TEST(TestSuit1,ForthTestSuit004CorePlus)
{
    RunAndPrint(infilenames, filenames, 4);
}
TEST(TestSuit1,ForthTestSuit005Utilities)
{
    RunAndPrint(infilenames, filenames, 5);
}
TEST(TestSuit1,ForthTestSuit006ErrorReports)
{
    RunAndPrint(infilenames, filenames, 6);
}
TEST(TestSuit1,ForthTestSuit010MemoryTest)
{
    RunAndPrint(infilenames, filenames, 7);
}
TEST(TestSuit1,ForthTestSuit011ExceptionTest)
{
    RunAndPrint(infilenames, filenames, 8);
}
TEST(TestSuit1,ForthTestSuit011Float1Test)
{
    RunAndPrint(infilenames, filenames, 9);
}
TEST(TestSuit1,ForthTestSuit011Float2Test)
{
    RunAndPrint(infilenames, filenames, 10);
}

TEST(TestSuit1,ForthTestSuit011Float3Test)
{
    RunAndPrint(infilenames, filenames, 11 ,10);
}
TEST(TestSuit1,ForthTestSuit007CoreExt)
{
 RunAndPrint(infilenames, filenames, 13);
}
TEST(TestSuit1,ForthTestSuit1995test)
{
 RunAndPrint(infilenames, filenames, 12);
}
TEST(TestSuit1,ForthTestSuit008CommandList)
{
		RunCommandAndPrint("words", filenames, 16);
}
TEST(TestSuit1,ForthTestSuit009FileTest)
{
    RunAndPrint(infilenames, filenames, 14);
}
TEST(TestSuit1, ForthTestSuit000AllTests)
{
	RunAndPrint(infilenames, filenames, 15);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

