#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <vector>
#include <regex>

#define FORTHSCRIPTCPP_ENABLE_FILE 1
#define FORTHSCRIPTCPP_ENABLE_MEMORY 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
// #define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"

const char *allfilenames[] = {
    "./testresult/alltests1.txt",  // 1
    "./testresult/alltests2.txt",  // 2
    "./testresult/alltests3.txt",  // 3
    "./testresult/alltests4.txt",  // 4
    "./testresult/alltests5.txt",  // 5
    "./testresult/alltests6.txt",  // 6
    "./testresult/alltests7.txt",  // 7
    "./testresult/alltests8.txt",  // 8
    "./testresult/alltests9.txt",  // 9
    "./testresult/alltests10.txt", // 10
    "./testresult/alltests11.txt", // 11
    "./testresult/alltests12.txt",
    "./testresult/alltests13.txt",
    "./testresult/alltests14.txt",
    "./testresult/alltests15.txt",
    "./testresult/alltests16.txt",
    "./testresult/alltests17.txt",
    "./testresult/alltests18.txt",
    "./testresult/alltests19.txt",
    "./testresult/alltests20.txt",
    "./testresult/alltests21.txt",
    "./testresult/alltests22.txt",
    "./testresult/alltests23.txt",
};

const char *allFilesInSuite[] = {
    "./suit1/tester.fr",       // 1
    "./suit1/utilities.fth",   // 2
    "./suit1/errorreport.fth", // 3
    "./suit1/prelimtest.fth",  // 4
    //		"./suit1/ttester.fs" ,
    "./suit1/core.fr",             // 5
    "./suit1/coreplustest.fth",    // 6
    "./suit1/coreexttest.fth",     // 7
    "./suit1/blocktest.fth",       // 8
    "./suit1/doubletest.fth",      // 9
    "./suit1/exceptiontest.fth",   // 10
    "./suit1/facilitytest.fth",    // 11
    "./suit1/filetest.fth",        // 12
    "./suit1/localstest.fth",      // 13
    "./suit1/memorytest.fth",      // 14
    "./suit1/toolstest.fth",       // 15
    "./suit1/searchordertest.fth", // 16
    "./suit1/stringtest.fth",      // 17
    "./suit1/fp/fpzero-test.4th",  // 18
    "./suit1/fp/ak-fp-test.fth",   // 19
    "./suit1/fp/paranoia.4th",     // 20

    ""};
int errorInTest{}, errorInForth{};
std::string contentGlobal{};
void OKtest(cppforth::Forth *ptr)
{
    // EXPECT_TRUE(true);
    // std::cout << "OK";
}
void WrongStackTest(cppforth::Forth *ptr)
{
    errorInTest++;
    if (ptr->forth_depth() > 0)
    {
        auto offset = ptr->forth_tocell(0);
        ptr->forth_pop(1);
        std::string span((offset < 40) ? offset : 40, '-');
        std::cout << "Fort test error WrongStackSizeTest" << std::endl;
        std::cout << contentGlobal.substr(offset < 40 ? 0 : offset - 40, 70) << std::endl;
        std::cout << span << "^^" << std::endl;
    }

    // ADD_FAILURE() << "WrongStackTest";
    // std::cout << "Error";
}
void WrongValuesOnStackTest(cppforth::Forth *ptr)
{
    if (ptr->forth_depth() > 0)
    {
        auto offset = ptr->forth_tocell(0);
        ptr->forth_pop(1);
        std::string span((offset < 40) ? offset : 40, '-');
        std::cout << "Fort test error WrongValuesOnStackTest" << std::endl;
        std::cout << contentGlobal.substr(offset < 40 ? 0 : offset - 40, 70) << std::endl;
        std::cout << span << "^^" << std::endl;
    }
    // ADD_FAILURE() << "WrongValuesOnStackTest";
    errorInTest++;
    // std::cout << "Error";
}

class MyTestSuite : public testing::TestWithParam<int>
{
protected:
    void SetUp() override
    {
        // std::cout << "SetUp"<<std::endl;
    }
    void TearDown() override
    {
        // std::cout << "TearDown"<<std::endl;
    }
    static cppforth::Forth *shared_resource_;
    static int loadedFile;
    static std::vector<std::string> fileLines;
    static const char **testFileNamesList;
    static const char **outputFileNamesList;

    static void SetUpTestSuite()
    {
        testFileNamesList = allFilesInSuite;
        outputFileNamesList = allfilenames;
        int numberOfTestFiles = 5;
        int Start = 0;
        {
            std::string res{}, error{};
            errorInTest = 0;
            errorInForth = 0;
            contentGlobal.clear();
            try
            {
                shared_resource_ = new cppforth::Forth{};
                auto forth = shared_resource_;
                loadedFile = -1;
                fileLines.clear();

                // Execute string  f.e. from configuration file

                for (int i = Start; i < numberOfTestFiles; ++i)
                {
                    try
                    {
                        if (std::strlen(testFileNamesList[i]) == 0)
                            break;
                        if (i > 4 && i != (numberOfTestFiles - 1))
                            continue; // skip all cycles except tested

                        std::ifstream is;
                        contentGlobal.clear();
                        is.open(testFileNamesList[i], std::ios_base::in);
                        if (is.is_open() && !is.bad())
                        {
                            if ((i < (numberOfTestFiles - 1)) || numberOfTestFiles <= 5)
                            {
                                std::stringstream inStrStream{};
                                inStrStream << is.rdbuf();   // read the file
                                contentGlobal = inStrStream.str(); //
                                forth->ExecuteString(contentGlobal);
                                std::cout << "File "<<testFileNamesList[i]<<":1 loaded"<<std::endl;
                            }
                            else
                            {
                                if (i == (numberOfTestFiles - 1))
                                {
                                    forth->forth_setcfunction(OKtest, "GtestOK");
                                    forth->forth_setcfunction(WrongStackTest, "GtestFailStack");
                                    forth->forth_setcfunction(WrongValuesOnStackTest, "GtestFailValues");

                                    forth->ExecuteString(
                                        R"( 
				VARIABLE #GTESTERRORS 
				: }T							\ ( ... -- ) COMPARE STACK (EXPECTED) CONTENTS WITH SAVED
				0 #GTESTERRORS !
												\ (ACTUAL)CONTENTS.
				DEPTH ACTUAL-DEPTH @ = IF		 \ IF DEPTHS MATCH
				DEPTH ?DUP IF					\ IF THERE IS SOMETHING ON THE STACK
				0  DO							\ FOR EACH STACK ITEM
				ACTUAL-RESULTS I CELLS + @		\ COMPARE ACTUAL WITH EXPECTED
				= 0= IF 1 #GTESTERRORS ! >IN @ GtestFailValues  S" INCORRECT RESULT: " ERROR LEAVE THEN
				LOOP
				THEN
				ELSE							\ DEPTH MISMATCH
				1 #GTESTERRORS ! >IN @ GtestFailStack 
				S" WRONG NUMBER OF RESULTS: " ERROR
				THEN 
				#GTESTERRORS @ 0 = IF GtestOK THEN
				; )");
                std::cout << "Macros }T loaded"<<std::endl;
                                }
                                int count{};
                                while (std::getline(is, contentGlobal))
                                {
                                    ++count;
                                    try
                                    {
                                        if (contentGlobal.length() > 0)
                                        {
                                            auto saveError = errorInTest;
                                            if (count == 186 && (numberOfTestFiles) == 6)
                                            {
                                                count = count;
                                            }

                                            forth->ExecuteString(contentGlobal);
                                            if (errorInTest != saveError)
                                            {
                                                std::cout << "Test Forth error: " << testFileNamesList[i] << std::endl
                                                          << contentGlobal << std::endl;
                                                // clar stack after error
                                                forth->ExecuteString(": clearstack depth 0 > IF depth 0 do drop loop ELSE ; clearstack ");
                                            }
                                        }
                                    }
                                    catch (cppforth::Forth::AbortException &ex)
                                    {
                                        const char *aaa = ex.what();
                                        error = aaa;
                                        errorInForth++;
                                        // ADD_FAILURE() << "Uncaught exception Forth: " << testFileNamesList[i] << std::endl << ex.what();
                                        std::cout << "1.Uncaught exception Forth: " << testFileNamesList[i] << std::endl
                                                  << ex.what();
                                    }
                                    catch (std::exception &ex)
                                    {
                                        const char *aaa = ex.what();
                                        error = aaa;
                                        errorInForth++;
                                        // ADD_FAILURE() << "Uncaught exception : " << testFileNamesList[i] << std::endl << ex.what();
                                        std::cout << "1.Uncaught exception : " << testFileNamesList[i] << std::endl
                                                  << ex.what();
                                    }
                                    catch (...)
                                    {
                                        error = " 1.Unknown exception";
                                        errorInForth++;
                                        // ADD_FAILURE() << "Uncaught exception unknown type " << testFileNamesList[i] << std::endl;
                                        std::cout << "1.Uncaught exception unknown type " << testFileNamesList[i] << std::endl;
                                    }
                                }
                            }

                            is.close();
                        }

                        if (i == 0)
                        {
                            forth->forth_setcfunction(OKtest, "GtestOK");
                            forth->forth_setcfunction(WrongStackTest, "GtestFailStack");
                            forth->forth_setcfunction(WrongValuesOnStackTest, "GtestFailValues");

                            forth->ExecuteString(
                                R"( 
				VARIABLE #GTESTERRORS 
				: }T							\ ( ... -- ) COMPARE STACK (EXPECTED) CONTENTS WITH SAVED
				0 #GTESTERRORS !
												\ (ACTUAL)CONTENTS.
				DEPTH ACTUAL-DEPTH @ = IF		 \ IF DEPTHS MATCH
				DEPTH ?DUP IF					\ IF THERE IS SOMETHING ON THE STACK
				0  DO							\ FOR EACH STACK ITEM
				ACTUAL-RESULTS I CELLS + @		\ COMPARE ACTUAL WITH EXPECTED
				= 0= IF 1 #GTESTERRORS ! >IN @ GtestFailValues  S" INCORRECT RESULT: " ERROR LEAVE THEN
				LOOP
				THEN
				ELSE							\ DEPTH MISMATCH
				1 #GTESTERRORS ! >IN @ GtestFailStack 
				S" WRONG NUMBER OF RESULTS: " ERROR
				THEN 
				#GTESTERRORS @ 0 = IF GtestOK THEN
				; )");
                        }
                    }
                    catch (cppforth::Forth::AbortException &ex)
                    {
                        const char *aaa = ex.what();
                        error = aaa;
                        errorInForth++;
                        // ADD_FAILURE() << "Uncaught exception Forth: " << testFileNamesList[i] << std::endl << ex.what();
                        std::cout << "1.Uncaught exception Forth: " << testFileNamesList[i] << std::endl
                                  << ex.what();
                    }
                    catch (std::exception &ex)
                    {
                        const char *aaa = ex.what();
                        error = aaa;
                        errorInForth++;
                        // ADD_FAILURE() << "Uncaught exception : " << testFileNamesList[i] << std::endl << ex.what();
                        std::cout << "1.Uncaught exception : " << testFileNamesList[i] << std::endl
                                  << ex.what();
                    }
                    catch (...)
                    {
                        error = " 1.Unknown exception";
                        errorInForth++;
                        // ADD_FAILURE() << "Uncaught exception unknown type " << testFileNamesList[i] << std::endl;
                        std::cout << "1.Uncaught exception unknown type " << testFileNamesList[i] << std::endl;
                    }

                    res = forth->ExecutionOutput();
                    forth->ExecutionOutputReset();
                    std::ofstream aa;
                    aa.open(outputFileNamesList[i], std::ios_base::out);
                    aa << "C++ test suite message (" << testFileNamesList[i] << "):" << std::endl
                       << res << std::endl
                       << error;
                }
                // std::cout << res << std::endl;
            }
            catch (cppforth::Forth::AbortException &ex)
            {
                const char *aaa = ex.what();
                errorInForth++;
                // ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
                std::cout << "2.Uncaught exception Forth: " << ex.what();
            }
            catch (std::exception &ex)
            {
                const char *aaa = ex.what();
                errorInForth++;
                // ADD_FAILURE() << "Uncaught exception : " << ex.what();
                std::cout << "2.Uncaught exception : " << ex.what();
            }
            catch (...)
            {
                errorInForth++;
                // ADD_FAILURE() << "Uncaught exception unknown type ";
                std::cout << "2.Uncaught exception unknown type ";
            }
            if (errorInForth > 0 || errorInTest > 0)
            {
                //			ADD_FAILURE() << "Test error " << testFileNamesList[numberOfTestFiles-1] <<", Forth errors:"<<errorInForth<<", Tests errors:"<<errorInTest << std::endl;
            }
        }
    }

    static void TearDownTestSuite()
    {
        delete shared_resource_;
        shared_resource_ = nullptr;
    }
};
// init static members
cppforth::Forth *MyTestSuite::shared_resource_{};
int MyTestSuite::loadedFile = -1;
std::vector<std::string> MyTestSuite::fileLines{};
const char **MyTestSuite::testFileNamesList{};
const char **MyTestSuite::outputFileNamesList{};

TEST_P(MyTestSuite, MyTest)
{
    int param = GetParam();
    int fileNum = param / 10000;
    int line = param % 10000;
    auto saveError = errorInTest;
    auto saveErrorForth = errorInForth;
    const char *error = "";
    std::string content{};
    if (fileNum != loadedFile)
    {
        std::ifstream is;
        is.open(testFileNamesList[fileNum - 1], std::ios_base::in);
        if (is.is_open() && !is.bad())
        {
            int statusIF=0;
            int lineIF=-1;
            std::regex IF{std::string(".*\\[[Ii][Ff]\\].*")};
            std::regex THEN{".*\\[[Tt][Hh][Ee][Nn]\\].*"};
            std::smatch m1{};
            fileLines.clear();
            while (std::getline(is, content))
            {
                switch(statusIF){
                    case 0:
                    if(std::regex_search(content,m1,IF) ){
                        lineIF=fileLines.size();
                        statusIF=1;
                        
                    } 
                    fileLines.push_back(content);
                    break;
                    case 1:
                    if(std::regex_match(content,m1,THEN)){
                        statusIF=0;
                    }
                fileLines[lineIF].append(" ").append(content);    
                fileLines.push_back(" ");
                }
            }

            is.close();
        }
        loadedFile = fileNum;
    }
    content.clear();
    try
    {
        if (line < fileLines.size())
        {
            content = fileLines.at(line);
            if (content.length() > 0)
            {
                contentGlobal=content;
                shared_resource_->ExecuteString(content);
                if (errorInTest != saveError)
                {
                    //std::cout << "Test Forth error: " << testFileNamesList[loadedFile - 1] << std::endl
                    //          << content << std::endl;
                    // clar stack after error
                    shared_resource_->ExecuteString(": clearstack depth 0 > IF depth 0 do drop loop ELSE ; clearstack ");
                }
            }
        }
    }
    catch (cppforth::Forth::AbortException &ex)
    {
        const char *aaa = ex.what();
        error = aaa;
        errorInForth++;
        // ADD_FAILURE() << "Uncaught exception Forth: " << testFileNamesList[i] << std::endl << ex.what();
        std::cout << "1.Uncaught exception Forth: " << testFileNamesList[loadedFile-1] << std::endl
                  << ex.what();
    }
    catch (std::exception &ex)
    {
        const char *aaa = ex.what();
        error = aaa;
        errorInForth++;
        // ADD_FAILURE() << "Uncaught exception : " << testFileNamesList[i] << std::endl << ex.what();
        std::cout << "1.Uncaught exception : " << testFileNamesList[loadedFile-1] << std::endl
                  << ex.what();
    }
    catch (...)
    {
        error = " 1.Unknown exception";
        errorInForth++;
        // ADD_FAILURE() << "Uncaught exception unknown type " << testFileNamesList[i] << std::endl;
        std::cout << "1.Uncaught exception unknown type " << testFileNamesList[loadedFile-1] << std::endl;
    }
if (errorInForth != saveErrorForth || errorInTest != saveError)
            {
                			ADD_FAILURE() << "Test error " << testFileNamesList[loadedFile-1] <<":"<<(line+1)<<" , Forth errors:"<<errorInForth<<", Tests errors:"<<errorInTest << std::endl
                            <<content<<std::endl;
            }
    //std::cout << "Example Test Param: " << GetParam() << "" << std::endl;
}

//INSTANTIATE_TEST_SUITE_P(ForthTestSuit005Core,      MyTestSuite, testing::Range(50000, 51100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit006CorePlus,  MyTestSuite, testing::Range(60000, 61100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit007CoreExt,   MyTestSuite, testing::Range(70000, 71100), testing::PrintToStringParamName());

INSTANTIATE_TEST_SUITE_P(ForthTestSuit009DoubleTest, MyTestSuite, testing::Range(90000, 90438), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit010ExceptionTest, MyTestSuite, testing::Range(100000, 101100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit011facilitytest, MyTestSuite, testing::Range(110000, 111100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit012FileTest,  MyTestSuite, testing::Range(120000, 121100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit013LocalsTest, MyTestSuite, testing::Range(130000, 131100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit014MemoryTest, MyTestSuite, testing::Range(140000, 141100), testing::PrintToStringParamName());

//INSTANTIATE_TEST_SUITE_P(ForthTestSuit016searchordertesttest, MyTestSuite, testing::Range(160000, 161100), testing::PrintToStringParamName());

//INSTANTIATE_TEST_SUITE_P(ForthTestSuit018Float1Test, MyTestSuite, testing::Range(180000, 181100), testing::PrintToStringParamName());

//INSTANTIATE_TEST_SUITE_P(ForthTestSuit019Float2TestPart1, MyTestSuite, testing::Range(190018, 190286), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit019Float2TestPart2, MyTestSuite, testing::Range(190290, 191100), testing::PrintToStringParamName());

//INSTANTIATE_TEST_SUITE_P(ForthTestSuit019Float2TestPart1, MyTestSuite, testing::Range(190000, 191100), testing::PrintToStringParamName());
// include required
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit020Float3Test, MyTestSuite, testing::Range(200000, 201100), testing::PrintToStringParamName());


// segfault
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit008BlockTest, MyTestSuite, testing::Range(80000, 81100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit015toolstest, MyTestSuite, testing::Range(150000, 151100), testing::PrintToStringParamName());
//INSTANTIATE_TEST_SUITE_P(ForthTestSuit017stringtest, MyTestSuite, testing::Range(170000, 171100), testing::PrintToStringParamName());

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
