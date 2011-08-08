/*****************************************************************************
*																    Includes *
******************************************************************************/

#include "stdafx.h"
#include "srangerexports.h"

/*****************************************************************************
*																   Constants *
******************************************************************************/

// Buffers
#define BUFFER_SIZE 	(1024+256+128)	// Buffer size
#define READY 			1				// Buffer ready
#define EMPTY 			2				// Buffer empty
#define BUSY 			3				// Buffer busy
#define FULL 			4				// Buffer full



/*****************************************************************************
*																  Structures *
******************************************************************************/

struct DspBufferAddDef {
	long b1s;
	long b2s;
	long bcp;
	long b1ep;
	long b2ep;

	long trajxb1;
	long trajxb2;
	long trajyb1;
	long trajyb2;
	long trajzb1;
	long trajzb2;
	long xb1;
	long xb2;
	long yb1;
	long yb2;
	long zb1;
	long zb2;
	long ab1;
	long ab2;
	long bb1; 
	long bb2; 
	
	long x;
	long y;
	long z;
	long a;
	long b;
	long scalex;
	long scaley;
	long scalez;
	long scalea;
	long scaleb;

	long ka;
	long kb;
	long typex;
	long cx;
	long ax;
	long kx;

	long typey;
	long cy;
	long ay;
	long ky;
	
	long typez;
	long cz;
	long az;
	long kz;

	long pos;
};