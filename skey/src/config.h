///////////////////////////////////////////////////
/*    AUTO-GENERATED CONFIG.H FOR USE SKEY.H     */
/*                                               */
/*             DO NOT ALTER!!!!!!!!!!!!!         */
/* Use config-skey.sh to modify this file ONLY!  */
///////////////////////////////////////////////////

#define HAVE_STDIO 1
#define HAVE_STDLIB 1
#undef HAVE_SYSTYPES
#define HAVE_STRING 1
#define HAVE_FCNTL 1
#define HAVE_SGTTY 1
#define HAVE_PWD 1
#undef HAVE_SYSRESOURCE
#define HAVE_TIME 1
#define HAVE_UNISTD 1
#define HAVE_CRYPT 1
#undef HAVE_SYSTEMINFO
#define HAVE_SHADOW 1
#undef HAVE_SYSPARAM
#undef HAVE_SYSQUOTA
#undef HAVE_SYSSTAT
#undef HAVE_SYSTIMEB
#undef HAVE_SYSTIME
#define HAVE_ERRNO 1
#define HAVE_SIGNAL 1
#define HAVE_TERMIO 1
#define HAVE_TERMIOS 1
#define IS_LITTLE_ENDIAN 1

//BEGIN_DEBUG_MACROS
#define _dbg_enter(dbgfile, func, num, ...)\
 if(dbgLevel >= 1) dbgL1_Enter(dbgfile, func);\
 if(dbgLevel >= 3) dbgL3_Enter(dbgfile, num, __VA_ARGS__);
#define _dbg_exit(dbgfile, func, ret)\
 if(dbgLevel >= 1) dbgL1_Exit(dbgfile, func);\
 if(dbgLevel >= 2) dbgL2_Exit(dbgfile, ret);
#define _dbg_file (dbgToFile==1) ? dbgFile : NULL
//END_DEBUG_MACROS
