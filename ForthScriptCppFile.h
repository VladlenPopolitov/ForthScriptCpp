#ifndef FORTHSCRIPTCPP_FILE_INCLUDED
#define FORTHSCRIPTCPP_FILE_INCLUDED


/****

File Access Words
-----------------

One of my goals is to make ForthScriptCPP useful for writing simple shell-like
scripts and utilities, and so being able to read and write files and execute
Forth scripts are necessities.  So I am providing a subset of the [File-Access
and File-Access extension wordsets][dpansFileAccess] from the standards.

[dpansFileAccess]: http://forth.sourceforge.net/std/dpans/dpans11.htm "File Access words"

As with our user input, we'll use C++ iostreams to implement the file access
words.  This means that a Forth _fileid_ is going to be a pointer to a
`std::fstream` instance.

On some platforms, the C++ iostreams library may be unavailable or incomplete,
or the overhead of linking in these words may be too great.  In that case,
define the macro `FORTHSCRIPTCPP_DISABLE_FILE_ACCESS` to disable compilation of these
words.

Words related to file position and size have undefined results if the size of a
file is greater than the maximum value that can be stored in a cell.

****/



//#define FILEID(x) reinterpret_cast<std::fstream*>(x)
struct ForthFile {
	Cell FILE;
	std::ios_base::openmode fileAccessMode;
	std::string fileName;
	std::shared_ptr<std::fstream> fileObject;

};
std::vector<ForthFile> OpenFiles{};
Cell fileHandle{};

std::shared_ptr<std::fstream> GetFileHandle(Cell handler, const std::string &function, errorCodes ec=errorAbort){
	for (auto it = OpenFiles.begin(); it != OpenFiles.end(); ++it){
		if ((*it).FILE == handler && (*it).fileObject->is_open()){
			return (*it).fileObject;
		}
	}
	throwMessage(function + ": wrong file handler access",ec);
	throw;
}

void  MoveFileHandle(Cell handler,std::shared_ptr<std::fstream>& fileHandle_,  const std::string& function, errorCodes ec = errorAbort) {
	for (auto it = OpenFiles.begin(); it != OpenFiles.end(); ++it) {
		if ((*it).FILE == handler) {
			std::swap((*it).fileObject, fileHandle_);
			return;
		}
	}
	throwMessage(function + ": wrong file handler access", ec);
	throw;
}

/*
DCell GetFileSize(Cell handler, const std::string& function, errorCodes ec = errorAbort) {
	for (auto it = OpenFiles.begin(); it != OpenFiles.end(); ++it) {
		if ((*it).FILE == handler && (*it).fileObject->is_open()) {
			return (*it).fileSize;
		}
	}
	throwMessage(function + ": wrong file handler access", ec);
	throw;
}
*/
std::ios_base::openmode GetFileAccessMode(Cell handler, const std::string& function, errorCodes ec = errorAbort) {
	for (auto it = OpenFiles.begin(); it != OpenFiles.end(); ++it) {
		if ((*it).FILE == handler && (*it).fileObject->is_open()) {
			return (*it).fileAccessMode;
		}
	}
	throwMessage(function + ": wrong file handler access", ec);
	throw;
}


std::string GetFileName(Cell handler, const std::string& function, errorCodes ec = errorAbort) {
	for (auto it = OpenFiles.begin(); it != OpenFiles.end(); ++it) {
		if ((*it).FILE == handler && (*it).fileObject->is_open()) {
			return (*it).fileName;
		}
	}
	throwMessage(function + ": wrong file handler access", ec);
	throw;
}


void OpenFilesSqeeze(){
	for (int it = static_cast<int>(OpenFiles.size()) - 1; it >= 0; --it){
		if (!(OpenFiles[it].fileObject->is_open())){
			if (it < (static_cast<int>(OpenFiles.size()) - 1)){
				std::swap(OpenFiles[it], OpenFiles[OpenFiles.size() - 1]);
			}
			OpenFiles.resize(OpenFiles.size() - 1);
		}
	}
}

// R/O ( -- fam )
void readOnly() {
	REQUIRE_DSTACK_AVAILABLE(1, "R/O");
	push(static_cast<Cell>(std::ios_base::in | std::ios_base::binary));
}

// R/W ( -- fam )
void readWrite() {
	REQUIRE_DSTACK_AVAILABLE(1, "R/W");
	push(static_cast<Cell>(std::ios_base::in | std::ios_base::out | std::ios_base::binary));
}

// W/O ( -- fam )
void writeOnly() {
	REQUIRE_DSTACK_AVAILABLE(1, "W/O");
	push(static_cast<Cell>(std::ios_base::out | std::ios_base::binary));
}

// BIN ( fam1 -- fam2 )
void bin() {
	REQUIRE_DSTACK_DEPTH(1, "BIN");
	dStack.setTop(dStack.getTop() | static_cast<Cell>(std::ios_base::binary));
}

// CREATE-FILE ( c-addr u fam -- fileid ior )
void createFile() {
	REQUIRE_DSTACK_DEPTH(3, "CREATE-FILE");

	auto caddr = dStack.getTop(2);
	auto length = SIZE_T(dStack.getTop(1));
	auto fam = static_cast<std::ios_base::openmode>(dStack.getTop()); pop();

	std::string filename{};
	moveFromDataSpace(filename, caddr, length);
	try{
		std::shared_ptr<std::fstream> f{ new std::fstream(filename, fam | std::ios_base::trunc) };
		if (f->is_open()) {
			//auto filesize = std::filesystem::file_size(filename);
			ForthFile createdFile{ ++fileHandle, fam,  filename, f };
			OpenFiles.push_back(createdFile);
			dStack.setTop(1, fileHandle);
			dStack.setTop(0);
		}
		else {
			dStack.setTop(1, 0);
			dStack.setTop(Cell(errorCreateFile));
		}
	}
	catch (std::exception &) {
		dStack.setTop(1, 0);
		dStack.setTop(Cell(errorCreateFile));
	}
}

// OPEN-FILE ( c-addr u fam -- fileid ior )
void openFile() {
	REQUIRE_DSTACK_DEPTH(3, "OPEN-FILE");

	auto caddr = dStack.getTop(2);
	auto length = SIZE_T(dStack.getTop(1));
	auto fam = static_cast<std::ios_base::openmode>(dStack.getTop()); pop();

	std::string filename{};
	moveFromDataSpace(filename, caddr, length);
	try{
		std::shared_ptr<std::fstream> f{ new std::fstream(filename, fam) };
		if (f->is_open()) {
			//auto filesize = std::filesystem::file_size(filename);
			ForthFile createdFile{  ++fileHandle, fam,  filename, f };
			OpenFiles.push_back(createdFile);
			dStack.setTop(1, fileHandle);
			dStack.setTop(0);
		}
		else {
			dStack.setTop(1, 0);
			dStack.setTop(Cell(errorOpenFile));
		}
	}
	catch (std::exception &) {
		dStack.setTop(1, 0);
		dStack.setTop(Cell(errorOpenFile));
	}
}

// RESIZE-FILE ( ud fileid -- ior )
void resizeFile() {
	REQUIRE_DSTACK_DEPTH(3, "RESIZE-FILE");
	auto h = (dStack.getTop()); pop();
	auto f = GetFileHandle(h, "RESIZE-FILE", errorResizeFile);
	DCell ud(dStack.getTop(1), dStack.getTop()); dStack.pop();

	auto fam = GetFileAccessMode(h, "RESIZE-FILE", errorResizeFile);
	std::string filename=GetFileName(h, "RESIZE-FILE", errorResizeFile);
	f->clear();
	f->close();
	std::filesystem::resize_file(filename,ud.data_.Dcells);
	try {
		std::shared_ptr<std::fstream> fnew{ new std::fstream(filename, fam  ) };
		if (fnew->is_open()) {
			//auto filesize = std::filesystem::file_size(filename);
			MoveFileHandle(h,  fnew,  "RESIZE-FILE", errorResizeFile);
			dStack.setTop(0);
		}
		else {
			dStack.setTop(Cell(errorResizeFile));
		}
	}
	catch (std::exception&) {
		dStack.setTop(Cell(errorResizeFile));
	}
}


// READ-FILE ( c-addr u1 fileid -- u2 ior )
void readFile() {
	REQUIRE_DSTACK_DEPTH(3, "READ-FILE");
	auto h = (dStack.getTop()); pop();
	auto f = GetFileHandle(h, "READ-FILE", errorReadFile);
	auto length = SIZE_T(dStack.getTop());
	auto caddr = (dStack.getTop(1));
	std::vector<char> readBuffer(length);
	f->clear();
	f->read(&readBuffer[0], static_cast<std::streamsize>(length));
	auto status = f->rdstate();
	auto realLength = static_cast<Cell>(std::strlen(&readBuffer[0]));
	auto realLengthF = CELL(f->gcount());
	moveIntoDataSpace(caddr, &readBuffer[0], realLengthF);
	dStack.setTop(f->bad() ? Cell(errorReadFile) : 0);
	dStack.setTop(1, realLengthF);
}

/// READ-LINE ( c-addr u1 fileid -- u2 flag ior )
void readLine() {
	REQUIRE_DSTACK_DEPTH(3, "READ-LINE");
	auto h = (dStack.getTop()); 
	auto f = GetFileHandle(h, "READ-LINE", errorReadLine);
	auto length = SIZE_T(dStack.getTop(1));
	auto caddr = (dStack.getTop(2));
	Cell ior = 0;
	Cell flag = True;
	Cell u2 = 0;
	if (length > 0) {
		std::vector<char> readBuffer(length+2);
		if(f->rdstate() == std::ios::failbit) {
			f->clear(); // EOL is not read. Need to clear failbit flag and continue reading
		}
		f->clear();
		f->getline(&readBuffer[0], static_cast<std::streamsize>(length+1)); // add termination 0 to char counter
		auto realLength = static_cast<Cell>(std::strlen(&readBuffer[0]));
		if (realLength == (length ) && f->rdstate() == std::ios::failbit) {
			f->clear(); // EOL is not read. Need to clear failbit flag and continue reading
		}
		length = realLength;
		auto infoLength = f->gcount();
		moveIntoDataSpace(caddr, &readBuffer[0], length);
		u2 = static_cast<Cell>(length);
		if (f->rdstate() & std::ios::eofbit && u2==0) {
			flag = False; // EOF
		}
		else {
			flag = True;
		}
		ior = f->bad() ? Cell(errorReadLine) : 0;
	}
	
	dStack.setTop(0, ior);
	dStack.setTop(1, flag);
	dStack.setTop(2, u2);
}

// READ-CHAR ( fileid -- char ior )
//
// Not a standard word.
//
// Reads a single character from the specified file.
// On success, ior is 0 and char is the character read.
// On failure, ior is non-zero and char is undefined.
void readChar() {
	REQUIRE_DSTACK_DEPTH(1, "READ-CHAR");
	REQUIRE_DSTACK_AVAILABLE(1, "READ-CHAR");
	auto h = (dStack.getTop());
	auto f = GetFileHandle(h, "READ-CHAR", errorReadFile);
	f->clear();
	auto ch = static_cast<unsigned char>(f->get());
	auto status = f->rdstate();
	dStack.setTop(static_cast<Cell>(ch));
	if (f->bad()) push(static_cast<Cell>(errorReadFile)); else push(0);
}

// WRITE-FILE ( c-addr u fileid -- ior )
void writeFile() {
	REQUIRE_DSTACK_DEPTH(3, "WRITE-FILE");
	auto h = (dStack.getTop()); pop();
	auto f = GetFileHandle(h, "WRITE-FILE", errorWriteFile);
	auto length = SIZE_T(dStack.getTop()); pop();
	auto caddr = dStack.getTop();
	std::string fileData{};
	moveFromDataSpace(fileData, caddr, length);
	f->clear();
	f->write(fileData.c_str(), static_cast<std::streamsize>(length));
	auto status = f->rdstate();
	dStack.setTop(f->bad() ? Cell(errorWriteFile) : 0);
}

// WRITE-LINE ( c-addr u fileid -- ior )
void writeLine() {
	REQUIRE_DSTACK_DEPTH(3, "WRITE-LINE");
	auto h = (dStack.getTop()); pop();
	auto f = GetFileHandle(h, "WRITE-LINE", errorWriteLine);
	auto length = SIZE_T(dStack.getTop()); pop();
	auto caddr = (dStack.getTop());
	std::string fileData{};
	moveFromDataSpace(fileData, caddr, length);
	f->clear();
	f->write(fileData.c_str(), static_cast<std::streamsize>(length));
	(*f) << std::endl;
	auto status = f->rdstate();
	auto written = f->gcount();
	Cell ior = f->bad() ? Cell(errorWriteLine) : 0;
	dStack.setTop(ior);
}

// WRITE-CHAR ( char fileid -- ior )
//
// Not a standard word.
//
// Writes a single character to the specified file.
void writeChar() {
	REQUIRE_DSTACK_DEPTH(2, "WRITE-CHAR");
	auto h = (dStack.getTop()); pop();
	auto f = GetFileHandle(h, "WRITE-CHAR", errorWriteFile);
	auto ch = static_cast<char>(dStack.getTop());
	f->clear();
	f->put(ch);
	auto status = f->rdstate();
	dStack.setTop(f->bad() ? Cell(errorWriteFile) : 0);
}

// FILE-POSITION ( fileid -- ud ior )
void filePosition() {
	REQUIRE_DSTACK_DEPTH(1, "FILE-POSITION");
	auto h = (dStack.getTop()); 
	auto f = GetFileHandle(h, "FILE-POSITION", errorFileSize);
	auto status = f->rdstate();
	if (status != 0) f->clear();
	f->clear();
	DCell position(f->tellg());
	status = f->rdstate();
	dStack.setTop(0, position.data_.Cells.lo);
	dStack.push(position.data_.Cells.hi);
	dStack.push(f->bad() ? Cell(errorFilePosition) : 0);
}

// FILE-SIZE ( fileid -- ud ior )
void fileSize() {
	REQUIRE_DSTACK_DEPTH(1, "FILE-SIZE");
	auto h = (dStack.getTop());
	auto filename = GetFileName(h, "FILE-SIZE", errorFilePosition);
	auto f = GetFileHandle(h, "FILE-SIZE", errorFilePosition);
	f->clear();
	f->flush();
	auto filesize = std::filesystem::file_size(filename);
	DCell position(filesize);
	DCell currentPosition(f->tellg());
	if (currentPosition.data_.SDcells > position.data_.SDcells) {
		// if OS was not updated filesize of open file, we take biggest value
		position = currentPosition;
	}
	dStack.setTop(0, position.data_.Cells.lo);
	dStack.push(position.data_.Cells.hi);
	dStack.push(f->bad() ? Cell(errorFilePosition) : 0);
}

// REPOSITION-FILE ( ud fileid -- ior )
void fileReposition() {
	REQUIRE_DSTACK_DEPTH(3, "REPOSITION-FILE");
	auto h = (dStack.getTop()); dStack.pop();
	DCell ud(dStack.getTop(1), dStack.getTop()); dStack.pop();
	auto f = GetFileHandle(h, "REPOSITION-FILE", errorFilePosition);
	f->clear();
	f->seekg(ud.data_.Dcells , std::ios_base::beg);
	auto status = f->rdstate();
	dStack.setTop(f->bad() ? Cell(errorFilePosition) : 0);
}

// FILE-STATUS (c-addr u -- x ior)
void fileStatus() {
	REQUIRE_DSTACK_DEPTH(2, "OPEN-FILE");

	auto caddr = dStack.getTop(1);
	auto length = SIZE_T(dStack.getTop());
	
	std::string filename{};
	moveFromDataSpace(filename, caddr, length);
	auto exists = std::filesystem::exists(filename);
	dStack.setTop(1, 0);
	dStack.setTop(exists? 0: errorFileStatus );
}


// FLUSH-FILE ( fileid -- ior )
void flushFile() {
	REQUIRE_DSTACK_DEPTH(1, "FLUSH-FILE");
	auto h = (dStack.getTop());
	auto f = GetFileHandle(h, "FLUSH-FILE");
	f->clear();
	f->flush();
	auto status = f->rdstate();
	dStack.setTop(f->bad() ? Cell(-1) : 0);
}

// CLOSE-FILE ( fileid -- ior )
void closeFile() {
	REQUIRE_DSTACK_DEPTH(1, "CLOSE-FILE");
	auto h = (dStack.getTop());
	auto f = GetFileHandle(h, "CLOSE-FILE", errorCloseFile);
	f->clear();
	f->close();
	dStack.setTop(0);
	OpenFilesSqeeze(); // squeeze OpenFile vector
}

// DELETE-FILE ( c-addr u -- ior )
void deleteFile() {
	REQUIRE_DSTACK_DEPTH(2, "DELETE-FILE");

	auto caddr = (dStack.getTop(1));
	auto length = SIZE_T(dStack.getTop()); pop();

	std::string filename{};
	moveFromDataSpace(filename, caddr, length);
	auto result = std::remove(filename.c_str());
	dStack.setTop(static_cast<Cell>(result?errorDeleteFile:0));
}

// RENAME-FILE ( c-addr1 u1 c-addr2 u2 -- ior )
void renameFile() {
	REQUIRE_DSTACK_DEPTH(4, "RENAME-FILE");
	auto lengthNew = SIZE_T(dStack.getTop()); pop();
	auto caddrNew = (dStack.getTop()); pop();
	auto lengthOld = SIZE_T(dStack.getTop()); pop();
	auto caddrOld = (dStack.getTop());
	std::string oldName{};
	moveFromDataSpace(oldName, caddrOld, lengthOld);
	std::string newName{};
	moveFromDataSpace(newName, caddrNew, lengthNew);
	auto result = std::rename(oldName.c_str(), newName.c_str());
	dStack.setTop(static_cast<Cell>(result?errorRenameFile:0));
}

// INCLUDE-FILE ( i*x fileid -- j*x )
void includeFile() {
	REQUIRE_DSTACK_DEPTH(1, "INCLUDE-FILE");

	auto h = (dStack.getTop()); // do not delete handler from stack to close file after read
	auto f = GetFileHandle(h, "INCLUDE-FILE",errorReadFile);
	/*
	std::string line;
	while (std::getline(*f, line)) {
		auto addr = PutStringToEndOfDataSpace(line);
		push(CELL(addr));
		push(static_cast<Cell>(line.length()));
		evaluate(); //@bug - should move evaluate outside of this procedure
	}
	*/
	std::string fileContent{};
	std::stringstream inStrStream{};
	inStrStream << (*f).rdbuf();   // read the file
	fileContent = inStrStream.str(); //
	SaveInput();
	setSourceId(h);
	setDataCell(VarOffsetBlkAddress, Cell(0)); // set BLK=0
	SetInput(fileContent);
	InterpretState = InterpretSource;
	// must close file according to specification
	closeFile();
	pop(); // discard error code
}



#endif
