# Portable S/Key Tool with debugging
Author: Cody Moore

## Description

Within this directory is a folder called 'skey' that contains
a set of programs that employ the S/Key One-Time-Function (OTF)
algorithm.  For more details on the S/Key OTF algorithm please
refer to the README inside of the 'skey' folder (./skey/README).

The details of the modifications of the S/Key tool follow:

- It now has built-in debugging functionality by using:
	#include "debug.h"
from within any .c file to access the debugging functions.
There is also a set of debugging macros that can be used to
more easily call all the debugging library functions,
and they are as follows:

Call on function enter:
	_dbg_enter(dbgfile, func, num, ...)
Argument descriptions:
	char* dbgfile = file name for logging (can be NULL for stderr)
	char* func = function name
	int num = number of arguments
	... = each argument

Also you can call on function exit:
	_dbg_exit(dbgfile, func, ret)
Argument descriptions:
	char* dbgfile = file name for logging (can be NULL for stderr)
	char* func = function name
	char* ret = char* representation of return value

NOTE: Use _dbg_file macro when calling _dbg_enter or _dbg_exit
but make sure to declare and initialize:
	int dbgToFile;
	char* dbgFile;
An example in main after #include "config.h" would be the following
code snippet to properly use the debugging functionality:
	int dbgToFile = 1;
	char* dbgFile = "/home/user/Log.txt";
	char* test = "This is a test";
	int ret = -1;
	_dbg_enter(_dbg_file, "printf", 1, test);
	ret = printf("%s\n", test);
	_dbg_exit(_dbg_file, "printf", ret);

- The bash script config-skey.sh must be ran before anything else.
The installion/build procedure is outlined below

## Installation

'cd' into the src directory and run the following command:

	./config-skey.sh -C <COMPILER> <INSTALLPATH>

Replacing <COMPILER> with your favorite compiler (ie. gcc) and
<INSTALLPATH> with an existing directory that you wish to install
skey into.

After running the above command, you should be able to run:

	make install

to do the installation procedure.  When it is complete, you should
then be able to run the programs from within their install directory.

If you chose to however, you can add the programs to your system path
and export it in your ~/.bashrc OR ~/.bash_profile
