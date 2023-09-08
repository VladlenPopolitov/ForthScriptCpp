/*
  Demonstrate the usage of Forth class - implementing of simple Forth script (read line and execute).

  To build apllication use your favorite compiler, f.e. g++ :
   g++ SimpleForth.cpp  --std=c++11 -o SimpleForth

  // Forth is called by member ExecuteString :
  void ExecuteString(const std::string &commands)

  // Output buffer can be read by member  ExecutionOutput and cleared by ExecutionOutputReset
  std::string ExecutionOutput() const
  void ExecutionOutputReset()

  // If scripts reads from input buffer ( REFILL , ACCEPT ) , input buffer can be assigned and cleared by members
  void SetExecutionInputBuffer(const std::string &input)
  void ExecutionInputBufferReset()
  bool isExecutionInputBufferEmpty() const 
		
*/
#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
#define FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER 1

#include "../../ForthScriptCpp.h"
#include <fstream>


int main(int argc, const char** argv) {
	try {
		cppforth::Forth forth{};
		forth.SetExecutionInputBuffer("");
		forth.setInputSource(cppforth::Forth::FromStdCin);
		forth.setWriteTarget(cppforth::Forth::ToStdCout);

		std::string line{ " " }, res2{};
		for(int i=1;i<argc;++i){
			std::ifstream t(argv[i]);
                        std::string str((std::istreambuf_iterator<char>(t)),
                         std::istreambuf_iterator<char>());
			forth.ExecuteString(str);
			res2 = forth.ExecutionOutput();
			forth.ExecutionOutputReset();
			
			<< res2;
                }
		while (!forth.isFinished()){
			std::cout << std::endl << ">" ;
 			std::getline(std::cin, line);
			try {
				forth.ExecuteString(line);
				res2 = forth.ExecutionOutput();
				std::cout << res2;
			}
			catch (cppforth::Forth::AbortException &ex){
				const char *errorMessage = ex.what();
				std::cout << "Forth exception " << errorMessage << std::endl;
			}
			forth.ExecutionOutputReset();
			res2.clear();
		}
	}
	catch (cppforth::Forth::AbortException &ex){
		const char *errorMessage = ex.what();
         	std::cout << "Forth exception "<<errorMessage<<std::endl;
	}
	catch (std::exception &ex){
		const char *errorMessage = ex.what();
         	std::cout << "C++ exception "<<errorMessage<<std::endl;
	}
	catch (...){
         	std::cout << "Unknown C++ exception "<<std::endl;
	}
	return 0;
}
