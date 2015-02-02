#!/bin/bash

# Globals
_CONFIG_HEADER="config.h"

createConfigHeaderOrNop(){
	# Create header if none exists
	set -o noclobber
	{ > $_CONFIG_HEADER ; } &> /dev/null
}

# Writes out all the needed debug macros
writeDebugMacros(){
	# Build comment strings
	BEGIN_STR="//BEGIN_DEBUG_MACROS"
	END_STR="//END_DEBUG_MACROS"
	# Build macro strings
	L1="#define _dbg_enter(dbgfile, func, num, ...)\\"
	L2=" if(dbgLevel >= 1) dbgL1_Enter(dbgfile, func);\\"
	L3=" if(dbgLevel >= 3) dbgL3_Enter(dbgfile, num, __VA_ARGS__);"
	L4="#define _dbg_exit(dbgfile, func, ret)\\"
	L5=" if(dbgLevel >= 1) dbgL1_Exit(dbgfile, func);\\"
	L6=" if(dbgLevel >= 2) dbgL2_Exit(dbgfile, ret);"
	L7="#define _dbg_file (dbgToFile==1) ? dbgFile : NULL"
	# Create or nop
	createConfigHeaderOrNop
	# Run checks
	if grep -Fxq "$BEGIN_STR" $_CONFIG_HEADER; then
		echo "Debug macros already written, skipping..."
	else
		echo -e "$BEGIN_STR" >> $_CONFIG_HEADER
		echo -e "$L1" >> $_CONFIG_HEADER
		echo -e "$L2" >> $_CONFIG_HEADER
		echo -e "$L3" >> $_CONFIG_HEADER
		echo -e "$L4" >> $_CONFIG_HEADER
		echo -e "$L5" >> $_CONFIG_HEADER
		echo -e "$L6" >> $_CONFIG_HEADER
		echo -e "$L7" >> $_CONFIG_HEADER
		echo -e "$END_STR" >> $_CONFIG_HEADER
	fi
	echo
}

# Used for checking library and appending to config.h
checkAndInclude(){
	# Grab path and definition label
	HEADER_PATH=$1
	HEADER_DEF=$2
	# Create or nop
	createConfigHeaderOrNop
	# Build strings
	DEFINE_STR="#define $HEADER_DEF 1"
	UNDEFINE_STR="#undef $HEADER_DEF"
	# Output
	#echo "Testing for $HEADER_PATH"
	# Check if strings exist
	if grep -Fxq "$DEFINE_STR" $_CONFIG_HEADER; then
		echo "Found $DEFINE_STR in $_CONFIG_HEADER"
	elif grep -Fxq "$UNDEFINE_STR" $_CONFIG_HEADER; then
		echo "Found $UNDEFINE_STR in $_CONFIG_HEADER"
	else
	    if test -f $HEADER_PATH; then
	    	#echo "Including $HEADER_PATH..."
			echo -e "$DEFINE_STR" >> $_CONFIG_HEADER
		else
			#echo "$HEADER_PATH not found, excluding..."
			echo -e "$UNDEFINE_STR" >> $_CONFIG_HEADER
		fi
	fi
}

# Writes pre-generated comment block for header
writeHeaderComments(){
	# Create header or nop
	createConfigHeaderOrNop
	# Build header comment strings
	LINE="///////////////////////////////////////////////////"
	TITLE="AUTO-GENERATED CONFIG.H FOR USE SKEY.H"
	C="$LINE\n"
	C="$C/*    $TITLE     */\n"
	C="$C/*                                               */\n"
	C="$C/*             DO NOT ALTER!!!!!!!!!!!!!         */\n"
	C="$C/* Use config-skey.sh to modify this file ONLY!  */\n"
	C="$C$LINE\n"
	# Check if header block was written
	if grep -Fxq "$LINE" $_CONFIG_HEADER; then
		echo "Comment block written, skipping..."
	else
		echo -e "$C" >> $_CONFIG_HEADER
	fi
	echo
}

# Writes definition based on checking of endian-ness
writeEndian(){
	LITTLE_ENDIAN=$(echo -n I | hexdump -o | awk '{ print substr($2,6,1); exit}')
	DEF_STR="#define IS_LITTLE_ENDIAN 1\n"
	UNDEF_STR="#undef IS_LITTLE_ENDIAN\n"
	# Check if definition exists
	if grep -Fxq "$DEF_STR" $_CONFIG_HEADER; then
		echo "Endian-ness already defined, skipping.."
	elif grep -Fxq "$UNDEF_STR" $_CONFIG_HEADER; then
		echo "Endian-ness already defined, skipping..."
	else
		if [ "$LITTLE_ENDIAN" -eq 1 ]; then
			echo -e "$DEF_STR" >> $_CONFIG_HEADER
		else
			echo -e "$UNDEF_STR" >> $_CONFIG_HEADER
		fi
	fi
}

# Dynamically builds config.h for all code includes
headerConfig(){
	# Lib directory
	_LIB_DIR="/usr/include"
	# All headers
	HEADERS=("$_LIB_DIR/stdio.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/stdlib.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/types.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/string.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/fcntl.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sgtty.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/pwd.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/resource.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/time.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/unistd.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/crypt.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/systeminfo.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/shadow.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/param.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/quota.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/stat.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/timeb.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/sys/time.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/errno.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/signal.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/termio.h")
	HEADERS=("${HEADERS[@]}" "$_LIB_DIR/termios.h")
	# All definitions in same order
	DEFS=("HAVE_STDIO")
	DEFS=("${DEFS[@]}" "HAVE_STDLIB")
	DEFS=("${DEFS[@]}" "HAVE_SYSTYPES")
	DEFS=("${DEFS[@]}" "HAVE_STRING")
	DEFS=("${DEFS[@]}" "HAVE_FCNTL")
	DEFS=("${DEFS[@]}" "HAVE_SGTTY")
	DEFS=("${DEFS[@]}" "HAVE_PWD")
	DEFS=("${DEFS[@]}" "HAVE_SYSRESOURCE")
	DEFS=("${DEFS[@]}" "HAVE_TIME")
	DEFS=("${DEFS[@]}" "HAVE_UNISTD")
	DEFS=("${DEFS[@]}" "HAVE_CRYPT")
	DEFS=("${DEFS[@]}" "HAVE_SYSTEMINFO")
	DEFS=("${DEFS[@]}" "HAVE_SHADOW")
	DEFS=("${DEFS[@]}" "HAVE_SYSPARAM")
	DEFS=("${DEFS[@]}" "HAVE_SYSQUOTA")
	DEFS=("${DEFS[@]}" "HAVE_SYSSTAT")
	DEFS=("${DEFS[@]}" "HAVE_SYSTIMEB")
	DEFS=("${DEFS[@]}" "HAVE_SYSTIME")
	DEFS=("${DEFS[@]}" "HAVE_ERRNO")
	DEFS=("${DEFS[@]}" "HAVE_SIGNAL")
	DEFS=("${DEFS[@]}" "HAVE_TERMIO")
	DEFS=("${DEFS[@]}" "HAVE_TERMIOS")
	# Write headers comments
	writeHeaderComments
	# Iterate through all headers to include
	numDefs=${#DEFS[@]}
	numHeaders=${#HEADERS[@]}
	# NOTE: numDefs MUST EQUAL numHeaders
	for (( i=0; i<${numHeaders}; i++ )); do
		checkAndInclude ${HEADERS[$i]} ${DEFS[$i]}
	done
	# Write endian-ness
	writeEndian
	# Write out debug macros
	writeDebugMacros
	# Output complete
	echo "config.h configuration complete!"
}

# Uses given compiler or autodetects compiler
setCompiler(){
	# Get the first arg as COMPILER
	COMPILER=$1
	if [ -z "$COMPILER" ]; then
		# Check for GCC
		GCCVAR=$(command -v gcc)
		if [ -z "$GCCVAR" ]; then
			# Check for CC
			CCVAR=$(command -v cc)
			if [ -z "$CCVAR" ]; then
				# No compiler found
				echo "ERROR: No compiler found, cannot continue."
				exit 1
			else
				sed -i "s~@COMPILER@~$CCVAR~g" Makefile
			fi
		else
			sed -i "s~@COMPILER@~$GCCVAR~g" Makefile
		fi
	else
		sed -i "s~@COMPILER@~$COMPILER~g" Makefile
	fi
}

# Sets destination directory
setDestDir(){
	DEST_DIR=$1
	if [ -d "$DEST_DIR" ]; then
		sed -i "s~@DESTDIR@~$DEST_DIR~g" Makefile
	else
		echo "ERROR: Invalid installation directory, cannot continue."
		exit 1
	fi
}

# Sets C Flags based on compiler
setCFlags(){
	GCC_FLAGS="-Wall -Werror -DUSE_ECHO -lcrypt"
	CC_FLAGS=""
	COMPILER=$1
	if [ -n "$COMPILER" ]; then
		if [[ "$COMPILER" =~ "gcc" ]]; then
			sed -i "s~@CFLAGS@~$GCC_FLAGS~g" Makefile
		else
			if [[ "$COMPILER" =~ "cc" ]]; then
				sed -i "s~@CFLAGS@~$CC_FLAGS~g" Makefile
			else
				sed -i "s~@CFLAGS@~~g" Makefile
				echo "WARNING: No compiler flags set, compiler unknown."
			fi
		fi
	else
		if $(command -v gcc | grep -q 'gcc'); then
			sed -i "s~@CFLAGS@~$GCC_FLAGS~g" Makefile
		else
			if $(command -v cc | grep -q 'cc'); then
				sed -i "s~@CFLAGS@~$CC_FLAGS~g" Makefile
			else 
				echo "ERROR: No compiler found, cannot continue."
				exit 1
			fi
		fi
	fi
}

# Sets Makefile to use install if present
setInstallProgram(){
	INSTALL=$(command -v install)
	if [ -z "$INSTALL" ]; then
		sed -i "s~@INSTALLPROGRAM@~cp~g" Makefile
	else
		sed -i "s~@INSTALLPROGRAM@~install~g" Makefile
	fi
}

# Create Makefile from finished template
createMakefile(){
	# Delete any previous Makefile
	rm -f Makefile 2>/dev/null
	# Use copy for build
	cp Makefile.tmpl Makefile
}

# Builds Makefile from template
makefileConfig(){
	# Init vars
	INSTALL_PATH=$1
	COMPILER=$2
	# Create new Makefile
	createMakefile
	# Configure Makefile
	setCompiler $COMPILER
	setDestDir $INSTALL_PATH
	setCFlags $COMPILER
	setInstallProgram
	# Output
	echo "Makefile configuration complete!"
}

# Prints usage string then exits with return value 1
usage(){
	echo $0 " [-C arg] INSTPATH"
	echo "  -C [arg]	:  Use 'arg' as default compiler"
	echo "  INSTPATH	:  Install path to use"
	exit 1
}

########################################################
## MAIN
########################################################
# Parse out options
ARGNUM=$#
shift $((OPTIND - 1))
while getopts ":C:" opt; do
	case $opt in
		C)
			echo "Compiler set to: $OPTARG" >&2
			C_OPT=$OPTARG
			;;
		\?)
			echo "Invalid option: -$OPTARG" >&2
			usage
			;;
		:)
			echo "Option -$OPTARG requires an argument." >&2
			usage
			;;
	esac
done

# Get INSTPATH
if [ "$ARGNUM" -eq 1 ]; then
	INSTPATH=$1
	echo "Install path set to: $INSTPATH"
elif [ "$ARGNUM" -eq 3 ]; then
	if [ "$2" == "-C" ]; then
		INSTPATH=$1
		C_OPT=$3
	else
		INSTPATH=$3
	fi
	echo "Install path set to: $INSTPATH"
else
	echo "Invalid usage"
	usage
fi

# Sanitize Install path
if [[ -d $INSTPATH ]]; then
	# Build config.h
	headerConfig
	echo
	# Build Makefile
	makefileConfig $INSTPATH $C_OPT
elif [[ -f $INSTPATH ]]; then
	echo "Install path cannot be an existing file."
else
	echo "Invalid path, or directory does not exist."
fi