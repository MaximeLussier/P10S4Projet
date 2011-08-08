/*****************************************************************************
*																    Includes *
******************************************************************************/

#include "crane.h"

/*****************************************************************************
*																   Constants *
******************************************************************************/

#ifndef DLLIMPORT
#define DLLIMPORT __declspec(dllimport)
#endif


// Error codes
#define ERR_OPEN_FILE	-1			// Open file failed
#define ERR_CLOSE_FILE	-2			// Close file failed
#define ERR_WRITE_FILE	-3			// Write file failed
#define ERR_PARSE_FILE	-4			// Parse file failed

#define ERR_NULL_PTR	-5			// Null pointer exception
#define ERR_THREAD		-6

#define ERR_READ_DSP	-7			// DSP read operation failed
#define ERR_WRITE_DSP	-8			// DSP write operation failed
#define ERR_START_DSP	-9			// DSP launch failed
#define ERR_CLOSE_DSP	-10			// DSP close failed
#define ERR_OPEN_DSP	-11			// DSP open failed
#define ERR_SYNC_DSP	-12			// DSP synchronization lost

#define ERR_TYPE		-13			// Controller type invalid or not specified
#define ERR_TF			-14			// Transfert function null
#define ERR_PID			-15			// PID coefficient null

#define ERR_RUNNING		-16			// Crane is already running
#define ERR_TRAJ		-17			// Trajectory is not loaded
#define ERR_TRAJ_FLAG	-18			// Run flag is not valid
#define ERR_CTL_MODE	-19			// Control mode is not valid
#define ERR_TRAJ_MODE	-20			// Trajectory mode is not valid

// Commands, control mode, trajectory mode
#define STOP 			1			// Stop crane
#define LOOP			0			// Restart crane after execution of the trajectory
#define HOLD			2			// Hold last position after execution of the trajectory
#define RESET			3			// Reset command
#define OPEN 			2			// Opend-loop run
#define CLOSE 			7			// Closed-loop run

// Controller type
#define PID				0			// Proportionnal, integral and derivative controller
#define TF				1			// Lead-lag controller

// Run flags
#define RESTART			0			// Reset the trajectory to the beginning before run
#define CONTINUE		1			// Restart the trajectory at same point


/*****************************************************************************
*																  Structures *
******************************************************************************/

struct CompParamdef {
	int	  typeXBeta;				// x-beta controller type
	float numXBeta[3];				// x-beta controller transfert function numerator 
	float denXBeta[3];				// x-beta controller transfert function denominator
	float pidXBeta[3];				// x-beta PID controller parameters
	float kb;						// xb internal feedback gain

	int	  typeYAlpha;				// y-alph controller type
	float numYAlpha[3];				// y-alpha transfert function numerator
	float denYAlpha[3];				// y-alpha transfert function denominator
	float pidYAlpha[3];				// y-alpha PID parameters
	float ka;						// ka internal feedback gain

	int	  typeR;					// r beta controller type
	float numR[3];					// r controller transfert function numrator
	float denR[3];					// r controller transfert function denominator
	float pidR[3];					// r PID controller parameters
};


/*****************************************************************************
*																     Classes *
******************************************************************************/

class DLLIMPORT Crane {
	
	public:
		
		//Functions
		 Crane();															// Create and Start thread
		 ~Crane();															// Stop thread if necessary
		
		 int					Run(int,int);								// Run crane
		 int					Stop(void);									// Stop crane
		 void					Reset(void);								// Reset crane
		 int					GetCommand(void);							// Get host command
		 void					Wait(int);									// Wait until the system reach a state
		 int 					GetErr(void);								// Get last error code
		 void					ClearErr(void);								// Clear last error code

		 int					LoadTrajFile(char *);						// Load trajectory from binary file
		 int					SetTrajBuffer(float *[3], long);			// Set trajectory buffer pointer
		 int 					GetTrajBuffer(float *[3], long *);			// Get trajectroy buffer pointer
		 int					SetTrajMode(int);							// Set trajectory mode
		 int					GetTrajMode(void);							// Get trajectory mode

		 int					LoadCompFile(char *);						// Load compensator parameters
		 int					SetComp(struct CompParamdef comp);			// Set structure compensateur
		 struct CompParamdef 	GetComp(void);								// Get structure compensateur

		 int					OpenSensorFile(char *);						// Save sensor buffers to binary file
		 int					CloseSensorFile(void);						// Get sensor buffers pointer
		 int					GetSensor(float [5]);						// Get current sensor values
		 int					GetScale(float [5]);						// Get position and angle scale factors
		 int					SetScale(float [5]);						// Set position and angle scale factors

	private:
		
		// Parameters
		void					*refnum;									// DSP reference number
		bool					stopThread;									// Stop thread flag	
		int						hostCommand;								// Host command
		int						trajMode;									// Trajectory mode flag	
		int 					err;																		
		
		long					hostBufferCPtr;								// Host buffer current pointers
		long					hostBufferEPtr;								// Host buffer end pointers
		
		float					*hostBufferPtr[3];							// Trajectories and sensor buffer pointer
		
		struct DspBufferAddDef	dspBufferAdd;								// DSP adress structure
		struct HSymTbl			sym;										// DSP symbol table
		struct CompParamdef		CompParam;									// Controller parameters

		DWORD					executionTime;								// Thread execution time
		FILE					*sensor_fd;									// Sensor file descriptor
		CWinThread				*thread;									// Thread pointer
		CCriticalSection		cs;
		
		// Functions
		int						GetDspCommand(int *);						// Get DSP command
		int						SetDspCommand(int);							// Get DSP command

		int						OpenBoard(void);							// Open DSP driver
		int						CloseDriver(void);							// Close DSP driver
		int						LoadUserCode(void);							// Load DSP user code

		int						GetDspBufferAdd(void);						// Get DSP buffers adresses
		int						GetDspBufferStatus(int *, int *);			// Get DSP buffers status
		int						SetDspBufferStatus(int, int);				// Set DSP buffers status
		int						WriteDspBuffer(int);						// Load DSP buffers
		int						ReadDspBuffer(int, int);					// Read DSP buffers
		int						GetDspBufferPrt(long *, long *, long *);	// Set DSP buffers pointer
		int						SetDspBufferPrt(long, long);				// Set DSP buffers pointer
		int						GetDspDebug(float *);						// Get DSP debug flag
	
		bool					StopThread(DWORD dwTimeoutMs = INFINITE);	// Thread stopper
		UINT					ExecuteThread(void);						// Thread function
		static UINT				WorkerThreadLauncher(LPVOID pvParam);		// Thread launcher
		
};
