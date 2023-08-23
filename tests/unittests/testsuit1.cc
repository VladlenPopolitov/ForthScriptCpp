

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
		"../testresult/alltests1.txt", // 1
		"../testresult/alltests2.txt", // 2
		"../testresult/alltests3.txt", // 3
		"../testresult/alltests4.txt", // 4
		"../testresult/alltests5.txt", // 5
		"../testresult/alltests6.txt", // 6
		"../testresult/alltests7.txt", // 7
		"../testresult/alltests8.txt", // 8
		"../testresult/alltests9.txt", // 9
		"../testresult/alltests10.txt",// 10
		"../testresult/alltests11.txt",// 11
		"../testresult/alltests12.txt",
		"../testresult/alltests13.txt",
		"../testresult/alltests14.txt",
		"../testresult/alltests15.txt",
		"../testresult/alltests16.txt",
		"../testresult/alltests17.txt",
		"../testresult/alltests18.txt",
	};

	const char* allFilesInSuite[] = {
		"../suit1/tester.fr" ,				// 1
		"../suit1/utilities.fth" ,			// 2
		"../suit1/errorreport.fth" ,		// 3
		"../suit1/prelimtest.fth" ,			// 4
		//		"../suit1/ttester.fs" ,
		"../suit1/core.fr" ,				// 5
		"../suit1/coreplustest.fth" ,		// 6
		"../suit1/coreexttest.fth" ,		// 7
		"../suit1/blocktest.fth" ,			// 8
		"../suit1/doubletest.fth" ,			// 9
		"../suit1/exceptiontest.fth" ,		// 10
		"../suit1/facilitytest.fth" ,		// 11
		"../suit1/filetest.fth" ,			// 12
		"../suit1/localstest.fth" ,			// 13
		"../suit1/memorytest.fth" ,			// 14
		"../suit1/toolstest.fth" ,			// 15
		"../suit1/searchordertest.fth" ,	// 16
		"../suit1/stringtest.fth" ,			// 17
		"../suit1/fp/fpzero-test.4th",		// 18
		"../suit1/fp/ak-fp-test.fth",		// 19
		"../suit1/fp/paranoia.4th",			// 20

		""
	};

	void OKtest(cppforth::Forth* ptr) {
		EXPECT_TRUE(true);
		//std::cerr << "OK";
	}
	void WrongStackTest(cppforth::Forth* ptr) {
		ADD_FAILURE() << "WrongStackTest";
		//std::cerr << "Error";
	}
	void WrongValuesOnStackTest(cppforth::Forth* ptr) {
		ADD_FAILURE() << "WrongValuesOnStackTest";
		//std::cerr << "Error";
	}


	void RunAndPrintSuiteAdvanced(const char* command[],
		const char* filename[], int numberOfTestFiles, int Start = 0) {
		std::string res{}, content{}, error{};
		try {
			cppforth::Forth forth{};
			
			// Execute string  f.e. from configuration file

			for (int i = Start; i < numberOfTestFiles; ++i) {
				try {
					if (std::strlen(command[i]) == 0) break;
					if (i > 2 && i != (numberOfTestFiles - 1)) continue; // skip all cycles except tested
					if (i == 1) {
						forth.forth_setcfunction(OKtest, "GtestOK");
						forth.forth_setcfunction(WrongStackTest, "GtestFailStack");
						forth.forth_setcfunction(WrongValuesOnStackTest, "GtestFailValues");

						forth.ExecuteString(
				R"( 
				VARIABLE #GTESTERRORS 
				: }T							\ ( ... -- ) COMPARE STACK (EXPECTED) CONTENTS WITH SAVED
				0 #GTESTERRORS !
												\ (ACTUAL)CONTENTS.
				DEPTH ACTUAL-DEPTH @ = IF		 \ IF DEPTHS MATCH
				DEPTH ?DUP IF					\ IF THERE IS SOMETHING ON THE STACK
				0  DO							\ FOR EACH STACK ITEM
				ACTUAL-RESULTS I CELLS + @		\ COMPARE ACTUAL WITH EXPECTED
				= 0= IF 1 #GTESTERRORS ! GtestFailStack S" INCORRECT RESULT: " ERROR LEAVE THEN
				LOOP
				THEN
				ELSE							\ DEPTH MISMATCH
				1 #GTESTERRORS ! GtestFailValues 
				S" WRONG NUMBER OF RESULTS: " ERROR
				THEN 
				#GTESTERRORS @ 0 = IF GtestOK THEN
				; )" );
					}
					std::ifstream is;
					is.open(command[i], std::ios_base::in);
					if (is.is_open() && !is.bad()) {
						std::stringstream inStrStream{};
						inStrStream << is.rdbuf();//read the file
						content = inStrStream.str();//
						is.close();
					}
					else {
						content.clear();
					}
					forth.ExecuteString(content);
				}
				catch (cppforth::Forth::AbortException& ex) {
					const char* aaa = ex.what();
					error = aaa;
					ADD_FAILURE() << "Uncaught exception Forth: " << command[i] << std::endl << ex.what();
				}
				catch (std::exception& ex) {
					const char* aaa = ex.what();
					error = aaa;
					ADD_FAILURE() << "Uncaught exception : " << command[i] << std::endl << ex.what();
				}
				catch (...) {
					error = " Unknown exception";
					ADD_FAILURE() << "Uncaught exception unknown type " << command[i] << std::endl;
				}

				res = forth.ExecutionOutput();
				forth.ExecutionOutputReset();
				std::ofstream aa;
				aa.open(filename[i], std::ios_base::out);
				aa << "C++ test suite message (" << command[i] << "):" << std::endl << res << std::endl << error;

			}
			//std::cout << res << std::endl;
		}
		catch (cppforth::Forth::AbortException& ex) {
			const char* aaa = ex.what();
			ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
		}
		catch (std::exception& ex) {
			const char* aaa = ex.what();
			ADD_FAILURE() << "Uncaught exception : " << ex.what();
		}
		catch (...) {
			ADD_FAILURE() << "Uncaught exception unknown type ";
		}
	}


TEST(TestSuit1All, ForthTestSuit000AllTestsInOnce)
{
	//RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 17);
}
TEST(TestSuit1, ForthTestSuit001Tester)
{
	RunAndPrint(infilenames, filenames, 1);
}

TEST(TestSuit1, ForthTestSuit002Utilities)
{
	//RunAndPrint(infilenames, filenames, 5);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 2);
}
TEST(TestSuit1, ForthTestSuit003ErrorReports)
{
	RunAndPrint(infilenames, filenames, 3);
}
TEST(TestSuit1, ForthTestSuit004Prelim)
{
    RunAndPrint(infilenames, filenames, 4);
}
TEST(TestSuit1,ForthTestSuit005Core)
{
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 5);
}
TEST(TestSuit1,ForthTestSuit006CorePlus)
{
    //RunAndPrint(infilenames, filenames, 4);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 6);
}
TEST(TestSuit1, ForthTestSuit007CoreExt)
{
	// RunAndPrint(infilenames, filenames, 13);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 7);
}
TEST(TestSuit1, ForthTestSuit008BlockTest)
{
	//RunAndPrint(infilenames, filenames, 8);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 8);
}
TEST(TestSuit1, ForthTestSuit009DoubleTest)
{
	//RunAndPrint(infilenames, filenames, 9);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 9);
}
TEST(TestSuit1, ForthTestSuit010ExceptionTest)
{
	//RunAndPrint(infilenames, filenames, 8);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 10);
}
TEST(TestSuit1,ForthTestSuit011facilitytest)
{
    //RunAndPrint(infilenames, filenames, 6);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 11);
}
TEST(TestSuit1, ForthTestSuit012FileTest)
{
	//RunAndPrint(infilenames, filenames, 14);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 12);
}
TEST(TestSuit1, ForthTestSuit013LocalsTest)
{
	//RunAndPrint(infilenames, filenames, 14);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 13);
}
TEST(TestSuit1,ForthTestSuit014MemoryTest)
{
    //RunAndPrint(infilenames, filenames, 7);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 14);
}
TEST(TestSuit1, ForthTestSuit015toolstest)
{
	//RunAndPrint(infilenames, filenames, 12);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 15);

}
TEST(TestSuit1, ForthTestSuit016searchordertesttest)
{
	//RunAndPrint(infilenames, filenames, 12);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 16);

}
TEST(TestSuit1, ForthTestSuit017stringtest)
{
	//RunAndPrint(infilenames, filenames, 12);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 17);

}

TEST(TestSuit1, ForthTestSuit018Float1Test)
{
	//RunAndPrint(infilenames, filenames, 9);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 18);
}

TEST(TestSuit1,ForthTestSuit019Float2Test)
{
    //RunAndPrint(infilenames, filenames, 10);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 19);
}

TEST(TestSuit1,ForthTestSuit020Float3Test)
{
     //RunAndPrint(infilenames, filenames, 11 ,10);
	RunAndPrintSuiteAdvanced(allFilesInSuite, allfilenames, 20);
}


TEST(TestSuit1,ForthTestSuit021words)
{
		RunCommandAndPrint("words", filenames, 21);
}




int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

