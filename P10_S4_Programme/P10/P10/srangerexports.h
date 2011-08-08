// ****************** Definition of function of SRangerC_HL library

////////Initialize_HL///////////////////////////////////////////
//
//	This function opens a connection with the Signal Ranger board.
//	Activates the DSP reset,downloads the kernel "SRKernel.out", reads back the kernel
//	and verifies. If it compares, it executes the kernel relocalisation.
//  At this point, the kernel starts executing, and the function returns.
//
//	Input	- PCHAR BoardID : For only one board : Ranger0. 
//			  This is the value used to access the first connected board.
//			  Used Ranger1 for the second one and so on...
//
//	Output	- void ** hDev : Reference to the Windows driver used to communicate
//			  with the board.
//
//	Return	-  0 : no error
//			  -1 : can't find the board
//			  -2 : problem with extracting or finding the COFF file of kernel
//			  -3 : can't reset the DSP board
//			  -4 : can't load Kernel
//			  -5 : kernel does not compare
//			  -6 : WLeds generates a error
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT Initialize_HL(PCHAR BoardID,void ** hDev);

///////DriverClose_HL////////////////////////////////////////////
//
// This function closes the connection to the "Signal Ranger" board.
//
// Input : - void ** hDev : Reference to the Windows driver used to communicate with the board.
//			 (return by the function : Initialize_HL)
//
// Output : No output
//
// Return :  0 : No error
//          -1 : Can't close the board connection 
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT DriverClose_HL(void ** hDev);

///////K_Move_Data_HL////////////////////////////////////////////
//
// This function reads or writes an unlimited number of 16-bit words to/from the Program space,
// the Data space, or the I/O space of the DSP, using the DSP kernel.
// The direction of the transfer is determined by the state of BOOL RW :
// RW -> true (Read), then the contents of DSP RAM are transferred to the PC.
// Size represents the number of WORDS to transfer.
// RW -> false (Write), then the contents of ArrayIn are transfered to the DSP.
// DSPAddress represents the address in DSP RAM to which the transfer should occur
// (read or write).
// Note:  The kernel must me loaded and executing for this function to be functional.
//
// Input : - void ** hDev : Reference to the Windows driver used to communicate with the board.
//		   - BOOL RW : Transfer direction is determined by the state of this BOOL
//					   true : Read  False : write. 
//		   - unsigned short Nb_Words  : The number of 16-bit words to read or write from DSP memory.
//		   - unsigned short * Buffer : Pointer on the array to read or write.
//		   - signed long DSPAddress : The address (in the DSP memory map) of the first word to
//							  transfer (read or write). Subsequent words occur at following
//						      addresses. (-1 : if a symbol is used)
//		   - int MemSpace : Memory Space concerned : 0 : Program 1 : Data 2 : I/O
//		   - char * Symbol : This is the Symbol (found in SymbolTable) of the starting address for
//					  the vector to write or read. (The DSP address should be set to -1)
//		   - struct HSymTbl SymTable : Structure of information about Symboltable structure(address
//						 	  and length). Return by Extract_out_HL or Load_User_HL functions.
//
// Output :- unsigned short * : Array read (only for Reading mode)
//
// Return :  0 : No error
//			-1 : Can't find symbol in symboltable
//          -2 : Can't read or write in DSP memory 
///////////////////////////////////////////////////	

extern "C" int PASCAL EXPORT K_Move_Data_HL(void ** hDev,BOOL RW,unsigned short Nb_Words,
											unsigned short * Buffer,signed long DSPAddress,
											int MemSpace,char * Symbol,struct HSymTbl SymTable);

////////LoadUser_HL///////////////////////////////////////////
//
// This function loads a user's DSP code into DSP memory.
// Only COFF2 and COFF1 formats are supported.
// The kernel (SRKernel.out) has to be loaded prior to the execution of this function.
//
// Input	- void ** hDev : Reference to the Windows driver used to communicate with the board.
//			 (return by the function : Initialize_HL)
//			- char * FilePath: Pointer to a char that contain the file path.
//			
// Output : - struct HSymTbl * pSymTable : Pointer to a structure of information about Symboltable
//			  structure (address and length).
//
// Return :   0 : No error
//	 		 -1	: Cannot open file
//	 		 -2	: This is not a valid COFF file
//			 -3	: This COFF file is not supported
//		     -4 : can't write into DSP memory
//			 -5 : one section does not compare
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT LoadUser_HL(void ** hDev,char * FilePath,struct HSymTbl * pSymTable);

/////////ExecUser_HL//////////////////////////////////////////
//
// This function launches execution of the DSP code from an entry point address,
// passed in argument. If DSPadr is -1 : the function searches in SymbolTable
// for the address corresponding to the label. This way, the entry point of the program
// or function can be defined symbolically by the label. If the symbol is not found, an
// error is generated.If DSPadr is not -1, the value passed is used as the entry point.
// The kernel (SRKernel.out) has to be loaded onto the DSP prior to the execution of
// this function.
//
// Input	- void ** hDev: Reference to the Windows driver used to communicate with the board.
//			 (return by the function : Initialize_HL)
//			- signed short DSPAddress : The starting point of the user DSP function.
//							   (-1 to use the Symbol Table).
//			- char * Symbol: Pointer to the symbol (found in SymbolTable) of the starting point
//		 			   of user's function in assembler code.
//		    - struct HSymTbl SymTable : Structure of information about Symboltable structure(address
//						 	  and length). Return by Extract_out_HL or Load_User_HL functions.
//			
// Output : - no output
//
// Return :  0 : No error
//          -1 : Symbol not find
//			-2 : Can't execute function 
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT ExecUser_HL(void ** hDev,signed long DSPAddress,char * Symbol,
										 struct HSymTbl SymTable);

///////Load_kernel_HL////////////////////////////////////////////
//
//	This function activates the DSP reset,downloads the kernel "SRKernel.out", 
//	reads back the kernel and verifies. If it compares, it executes the kernel
//	relocalisation.
//  At this point, the kernel starts executing, and the function returns.
//	
//	Input	- void ** hDev : Reference to the Windows driver used to communicate
//						with the board. Return by function Initialize_HL.
//
//	Output	- No output
//
//	Return	-  0 : no error
//			  -1 : problem with extracting or finding the COFF file of kernel
//			  -2 : can't reset the DSP board
//			  -3 : can't load Kernel
//			  -4 : kernel does not compare
//			  -5 : WLeds generates a error
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT Load_kernel_HL(void ** hDev);

//////Extract_out_HL/////////////////////////////////////////////
//
//  This function extracts the data and the symbol table from the COFF file of the DSP
//  executable. Only the COFF1 and COFF2 format are supported.
//
//  Input   - char * FilePath : Pointer to a char that contain the file path.
//
//  Output  - struct HLoadData pLoadData : Pointer to a structure of information about
//			  LoadData structure (address and length).
//			- struct HSymTbl pSymTable : Pointer to a structure of information about Symboltable
//			  structure (address and length).
//
//  Return    0	:	no error
//	 		 -1	:	Cannot open file
//	 		 -2	:	This is not a valid COFF file
//			 -3	:	This COFF file is not supported
///////////////////////////////////////////////////

extern "C" int PASCAL EXPORT Extract_out_HL(char * FilePath,struct HLoadData * pLoadData,
											struct HSymTbl * pSymTable);


// ****************** Definition of structure used by SRangerC_HL library

// Pointer to a structure of information about Symboltable
// structure (address and length).

struct	HSymTbl
				{
				unsigned long		pSymTbl;
				unsigned long		length;
				};

// Pointer to a structure of information about
// LoadData structure (address and length).

struct	HLoadData
				{
				unsigned long		pLoadData;
				unsigned long		length;
				};

// Load Data structure

struct	LoadData
				{
				unsigned long		beginaddress;
				unsigned short		*data;
				unsigned short		memType;
				unsigned long		length;
				};	

// Symbol Table structure

struct	SymTblReal
				{
				char				*symbol;
				unsigned long		address;	
				signed short		SecNo;
				};		

