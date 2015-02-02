#ifndef _DEBUG_LIB
#define _DEBUG_LIB

void debug(char* fName, int level, char* msg, char* msg2);

// Debug wrapper
void dbgL1_Enter(char* fName, char* funcName);

// Debug wrapper
void dbgL1_Exit(char* fName, char* funcName);

// Debug wrapper
void dbgL2_Exit(char* fName, char* arg);

// Debug wrapper
void dbgL3_Enter(char* fName, int num, char* arg, ...);

#endif