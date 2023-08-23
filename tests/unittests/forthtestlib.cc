#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include <exception>
#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include <fstream>
// helper functions;
void RunAndCompare(const std::string &command, const std::string &result,bool trace=false){
    std::string res;
    try{
        cppforth::Forth forth{};
#ifdef _DEBUG
        forth.setTrace(trace);
#endif
        forth.ExecuteString(command);
        res = forth.ExecutionOutput();
    }
    catch (cppforth::Forth::AbortException &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
    }
    catch (std::exception &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception : " << ex.what();
    }
    catch (...){
        ADD_FAILURE() << "Uncaught exception unknown type " ;
    }
    if (res.compare(result)){
        ADD_FAILURE() << "RunAndCompare faulure: should be: " << result << ", got:"<<res;
    }

}
void RunAndPrint(const std::string &command, std::string &result,
    const std::string &filename){
    std::string res;
    try{
        cppforth::Forth forth{};
        forth.ExecuteString(command);
        result = forth.ExecutionOutput();
        std::ofstream aa;
        aa.open(filename, std::ios_base::out);
        aa << result;

        //std::cout << res << std::endl;
    }
    catch (cppforth::Forth::AbortException &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
    }
    catch (std::exception &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception : " << ex.what();
    }
    catch (...){
        ADD_FAILURE() << "Uncaught exception unknown type " ;
    }

}
void RunCommandAndPrint(const char *command,
    const char *filename[], int numberOfTestFiles){
    std::string res{}, content{}, error{};
    try{
        cppforth::Forth forth{};
        for (int i = numberOfTestFiles-1; i < numberOfTestFiles; ++i){
            try{

                forth.ExecuteString(command);
            }
            catch (cppforth::Forth::AbortException &ex){
                const char *aaa = ex.what();
                error = aaa;
            }
            catch (std::exception &ex){
                const char *aaa = ex.what();
                error = aaa;
            }
            catch (...){
                error = " Unknown exception";
            }

            res = forth.ExecutionOutput();
            forth.ExecutionOutputReset();
            std::ofstream aa;
            aa.open(filename[i], std::ios_base::out);
            aa << res << std::endl << error;;
        }
        //std::cout << res << std::endl;
    }
    catch (cppforth::Forth::AbortException &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
    }
    catch (std::exception &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception : " << ex.what();
    }
    catch (...){
        ADD_FAILURE() << "Uncaught exception unknown type " ;
    }


}
void RunAndPrint(const char *command[],
    const char *filename[], int numberOfTestFiles, int Start=0){
    std::string res{}, content{}, error{};
    try{
        cppforth::Forth forth{};
        for (int i = Start; i < numberOfTestFiles; ++i){
            try{
                std::ifstream is;
                is.open(command[i], std::ios_base::in);
                if (is.is_open() && !is.bad()){
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
            catch (cppforth::Forth::AbortException &ex){
                const char *aaa = ex.what();
                error = aaa;
            }
            catch (std::exception &ex){
                const char *aaa = ex.what();
                error = aaa;
            }
            catch (...){
                error = " Unknown exception";
            }

            res = forth.ExecutionOutput();
            forth.ExecutionOutputReset();
            std::ofstream aa;
            aa.open(filename[i], std::ios_base::out);
            aa << res << std::endl << error;;
        }
        //std::cout << res << std::endl;
    }
    catch (cppforth::Forth::AbortException &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception Forth: " << ex.what();
    }
    catch (std::exception &ex){
        const char *aaa = ex.what();
        ADD_FAILURE() << "Uncaught exception : " << ex.what();
    }
    catch (...){
        ADD_FAILURE() << "Uncaught exception unknown type " ;
    }


}


void RunAndPrintSuite(const char* command[],
    const char* filename[], int numberOfTestFiles, int Start = 0) {
    std::string res{}, content{}, error{};
    try {
        cppforth::Forth forth{};
        for (int i = Start; i < numberOfTestFiles; ++i) {
            try {
                if (std::strlen(command[i]) == 0) break;
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
            }
            catch (std::exception& ex) {
                const char* aaa = ex.what();
                error = aaa;
            }
            catch (...) {
                error = " Unknown exception";
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

