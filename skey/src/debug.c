#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

void debug(char* fName, int level, char* msg, char* msg2){
  if(fName == NULL){
    fprintf(stderr, "[DBG_LVL:%i]:%s:%s\n", level, msg, msg2);
  } else {
    FILE *fp = fopen(fName, "ab+");
    if (fp == NULL){
      printf("[DEBUG_ERROR]:Could not open file\n");
      exit(1);
    }
    fprintf(fp, "[DBG_LVL:%i]:%s:%s\n", level, msg, msg2);
    fclose(fp);
  }
  return;
}

// Debug wrapper
void dbgL1_Enter(char* fName, char* funcName){
  debug(fName, 1, "Entering function", funcName);
  return;
}

// Debug wrapper
void dbgL1_Exit(char* fName, char* funcName){
  debug(fName, 1, "Exiting function", funcName);
  return;
}

// Debug wrapper
void dbgL2_Exit(char* fName, char* arg){
  debug(fName, 2, "Return value", arg);
  return;
}

// Debug wrapper
void dbgL3_Enter(char* fName, int num, char* arg, ...){
  va_list varlist;
  char* str;
  str=arg;
  va_start(varlist,arg);
  while(num > 0){
    debug(fName, 3, "Argument", str);
    str=va_arg(varlist,char*);
    num--;
  }
  va_end(varlist);
  return;
}