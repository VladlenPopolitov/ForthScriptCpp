/*
  Demonstrate the embeding of Forth as script language.

  To build apllication use your favorite compiler, f.e. g++ :
   g++ EmbededScript.cpp  --std=c++11 -o EmbededScript

  Forth::forth_setcfunction(void(*functionName)(Forth* ptr) , const std::string &name) 
  registers the function "functionName" and assign the Forth word "name"

  Registered function receives a pointer to Forth object and can access Forth environment 
  calling Forth member functions :

  // push value to stack
  void forth_push(Cell value)
  // return the depth of the stack
  size_t forth_depth()
  // return true, if stack is available for 'num' elements
  bool forth_available(size_t num) 
  // pop num elements from stack
  void forth_pop(size_t num)
  // read element from stack
  Cell forth_tocell(size_t index)
  // read from Forth memory to std::vector . Vector is cleared before reading
  void forth_readmemory(CAddr address, Cell size, std::vector<char> &buffer)
  // write to Forth memory from std::vector
  void forth_writememory(CAddr address, Cell size, std::vector<char> &buffer)

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
#define FORTHSCRIPTCPP_ENABLE_FLOAT 1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
//#define FORTHSCRIPTCPP_DISABLE_OUTPUT 1
#include "../../ForthScriptCpp.h"
#include <fstream>



static int Width = 0, Height = 0;
void SetWidth(cppforth::Forth* ptr){
	if (ptr->forth_depth() > 0){
		Width=ptr->forth_tocell(0);
	}
}
void SetHeight(cppforth::Forth* ptr){
	if (ptr->forth_depth() > 0){
		Height = ptr->forth_tocell(0);
	}
}
void GetWidth(cppforth::Forth* ptr){
	if (ptr->forth_available(1)){
		 ptr->forth_push(Width);
	}
	else {
		throw std::runtime_error("GetWidth does not have space in Forth stack");
	}
}
void GetHeight(cppforth::Forth* ptr){
	if (ptr->forth_available(1)){
		ptr->forth_push(Height);
	}
	else {
		throw std::runtime_error("GetHeight does not have space in Forth stack");
	}
}

int main(int argc, const char** argv) {
	try {
		cppforth::Forth forth{};
		forth.SetExecutionInputBuffer(" ");
	// Register functions as Forth words		
		forth.forth_setcfunction(SetWidth, "SetWidth");
		forth.forth_setcfunction(GetWidth, "GetWidth");
		forth.forth_setcfunction(SetHeight, "SetHeight");
		forth.forth_setcfunction(GetHeight, "GetHeight");
		forth.ExecuteString(" : makeaction 100 100 + dup * drop ; ");
	// Execute string  f.e. from configuration file
		for(int i = 0; i < 1000; ++i) {
			forth.ExecuteString("");
			std::string res = forth.ExecutionOutput();
			forth.ExecutionOutputReset();
		}
		std::cout <<" Width "<< Width << ", height " << Height << std::endl;
//		std::cout <<" Cell "<< sizeof(cppforth::Forth::Cell) << ", DCell " << sizeof(cppforth::Forth::DCell) << std::endl;
	}
	catch (cppforth::Forth::AbortException &ex){
		const char *errorMessage = ex.what();
	}
	catch (std::exception &ex){
		const char *errorMessage = ex.what();
	}
	catch (...){
		;
	}
	return 0;
}


