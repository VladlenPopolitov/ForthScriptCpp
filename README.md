# ForthScriptCpp

ForthScriptCpp: A Forth implementation as C++ Class by Vladlen Popolitov 

Use CMake to build examples and tests. Forth class is implemented as include only file.

CORE, CORE-EXT, DOUBLE, EXCEPTION, FILE, FLOAT, MEMORY, STRING, STRING-EXT are fully impelemented, all tests for these words are passed.
==============================================
The list of all FORTH-2012 words with the impelementation status is in [words_status.txt](words_status.txt). 

To check the Forth implementation run example SimpleForth: program read from keyboard input and execute everythng as Forth words:
```
git clone https://github.com/VladlenPopolitov/ForthScriptCpp
cd ForthScriptCpp
mkdir build
cd build
cmake ..
make
cd examples/SimpleForth
./SimpleForth
```

C++ Class cppforth::Forth (https://github.com/VladlenPopolitov/ForthScriptCpp) 
is inspired by Kristopher Johnson's cxxforth (https://github.com/kristopherjohnson/cxxforth). 

Though the most lines of code are changed to implements C++ members and virtual 
forth memory, many method names and comments are unchanged. cppforth::Forth class
can be used in C++ program as embeded script language. Forth accesses only virtual memory
and cannot harm anything in C++ address space. Forth address space, stacks and definitions
dictionary are implemented as std::vector.

This free software released into public domain and can be used without any restrictions.

 Create forth object:
 Include file ForthScriptCpp.h into your C++ program. 

```
#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
#define FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER 1

#include "ForthScriptCpp.h"
```

 Create object forthcpp::Forth .

```
  cppforth::Forth ForthExample{};
```

 Forth is called by member `void ExecuteString(const std::string &commands)` :

```
   ForthExample.ExecuteString("1 2 + .");
```

 Output buffer can be read by member  ExecutionOutput `std::string ExecutionOutput() const` and cleared by ExecutionOutputReset `void ExecutionOutputReset()`
```
  ForthExample.ExecuteString("1 2 + .");
  result=ForthExample.ExecutionOutput();   // result will be "3"
```

 If scripts reads from input buffer ( ACCEPT ) , input buffer can be assigned and cleared by members

  `void SetExecutionInputBuffer(const std::string &input)`

  `void ExecutionInputBufferReset()`

  `bool isExecutionInputBufferEmpty() const `

 Embeding of Forth as Script language to your C++ program.
========================================================== 

  Function forth_setcfunction registers the function "functionName" and assign the Forth word "name"

```
  Forth::forth_setcfunction(void(*functionName)(Forth* ptr) , const std::string &name)
```

  Registered function receives a pointer to Forth object and can access Forth environment 
  calling Forth member functions :
```
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

```

Example:

```
#define FORTHSCRIPTCPP_ENABLE_FILE   1
#define FORTHSCRIPTCPP_ENABLE_MEMORY  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT  1
#define FORTHSCRIPTCPP_ENABLE_FLOAT_EXT 1
#define FORTHSCRIPTCPP_ENABLE_INTERNALDEBUGGER 1
#include "ForthScriptCpp.h"

static int Width = 0;
void SetWidth(cppforth::Forth* ptr){
	if (ptr->forth_depth() > 0){
		Width=ptr->forth_tocell(0);
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

int main(int argc, const char** argv) {
	try {
		cppforth::Forth forth{};
		forth.SetExecutionInputBuffer(" ");
	 // Register functions as Forth words		
		forth.forth_setcfunction(SetWidth, "SetWidth");
		forth.forth_setcfunction(GetWidth, "GetWidth");
  // Execute string  f.e. from configuration file
	 forth.ExecuteString(" 100 SetWidth ");
		std::string res = forth.ExecutionOutput();
		forth.ExecutionOutputReset();
		std::cout <<" Width "<< Width << std::endl;
	} catch (cppforth::Forth::AbortException &ex){
		const char *errorMessage = ex.what();
	} catch (std::exception &ex){
		const char *errorMessage = ex.what();
	} catch (...){
		;
	}
	return 0;
}
```
