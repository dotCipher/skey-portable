/* S/KEY v1.1b (skeysh.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *          Scott Chasin <chasin@crimelab.com>
 *
 * Modifications: 
 *          Cody Moore <cjmoore@cs.sunysb.edu>
 *
 * S/Key authentication shell
 */

// Config include
#include "config.h"

// Dynamic libs
#ifdef HAVE_STDIO
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB
#include <stdlib.h>
#endif
#ifdef HAVE_STRING
#include <string.h>
#endif
#ifdef HAVE_PWD
#include <pwd.h>
#endif

char userbuf[16] = "USER=";
char homebuf[128] = "HOME=";
char shellbuf[128] = "SHELL=";
char pathbuf[128] = "PATH=:/usr/ucb:/bin:/usr/bin";
char *cleanenv[] = { userbuf, homebuf, shellbuf, pathbuf, 0, 0 };
char *shell = "/bin/csh";

char **environ;
struct passwd *pwd;

/*
void setenv(char* ename, char* eval, char* buf){
  register char *cp, *dp;
  register char **ep = environ;
 
  //this assumes an environment variable "ename" already exists
   
   while(dp = *ep++){
       for(cp = ename; *cp == *dp && *cp; cp++, dp++)
            continue;
       if(*cp == 0 && (*dp == '=' || *dp == 0)){
            strcat(buf, eval);
            *--ep = buf;
            return;
       }
   }
}

char* getenv(char* ename){
  register char *cp, *dp;
  register char **ep = environ;
 
  while(dp = *ep++){
         for(cp = ename; *cp == *dp && *cp; cp++, dp++)
              continue;
         if(*cp == 0 && (*dp == '=' || *dp == 0))
              return (*--ep);
  }
  return ((char *)0);
}
*/

void main (int argc, char* argv[]){
  int stat, pid;
  char user [8];
 
  if((pwd = getpwuid(getuid())) == NULL){
      fprintf(stderr, "Who are you?\n");
      exit(1);
  }

  strcpy(user, pwd->pw_name);

  if((pwd = getpwnam(user)) == NULL){
      fprintf(stderr, "Unknown login: %s\n", user);
      exit(1);
  }
 
  stat = skey_haskey(user);
 
  if(stat == 1){
     fprintf(stderr,"keysh: no entry for user %s.\n", user);
     exit (-1);
  }

  if(stat == -1){
     fprintf(stderr, "keysh: could not open key file.\n");
     exit (-1);
  }
 
  if(skey_authenticate (user) == -1){
      printf ("Invalid response.\n");
      exit (-1);
  } 

  if(setgid(pwd->pw_gid) < 0){
      perror("keysh: setgid");
      exit(3);
  }

  if(initgroups(user, pwd->pw_gid)){
      fprintf(stderr, "keysh: initgroups failed\n");
      exit(4);
  }

  if(setuid(pwd->pw_uid) < 0){
      perror("keysh: setuid");
      exit(5);
  }

  cleanenv[4] = getenv("TERM");
  environ = cleanenv;

  // int setenv(const char *name, const char *value, int overwrite);
  /*
  setenv("USER", pwd->pw_name, userbuf);
  setenv("SHELL", shell, shellbuf);
  setenv("HOME", pwd->pw_dir, homebuf);
  */
  setenv("USER", pwd->pw_name, 0);
  setenv("SHELL", shell, 0);
  setenv("HOME", pwd->pw_dir, 0);

  if(chdir(pwd->pw_dir) < 0){
      fprintf(stderr, "No directory\n");
      exit(6);
  }

  execv(shell, argv);
  fprintf(stderr, "No shell\n");
  exit(7);
}
