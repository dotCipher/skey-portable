/*
 * S/KEY v1.1b (skey.c)
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
 * Stand-alone program for computing responses to S/Key challenges.
 * Takes the iteration count and seed as command line args, prompts
 * for the user's key, and produces both word and hex format responses.
 *
 * Usage example:
 *	>skey 88 ka9q2
 *	Enter password:
 *	OMEN US HORN OMIT BACK AHOY
 *	>
 */

#include "md4.h"
#include "skey.h"
#include "debug.h"

int getopt ();
int optind;
char *optarg;

/* sevenbit ()
 *
 * Make sure line is all seven bits.
 */
void sevenbit(char* s){
   while(*s){
     *s = 0x7f & ( *s);
     s++;
   }
}

char* readpass(char* buf,int n){
#ifndef USE_ECHO
    set_term ();
    echo_off ();
#endif
    fgets (buf, n, stdin);
    rip (buf);
    printf ("\n\n");
    sevenbit (buf);
#ifndef USE_ECHO
    unset_term ();
#endif
    return buf;
}

void usage (char* s){
  printf("Usage: %s [OPTIONS [-p password ] [-n count] <sequence #>[/] <key> \n", s);
  printf("REQUIRED:\n");
  printf("   -p [PASS]  :  Password to use on One Time Function (OTF)\n");
  printf("   -n [COUNT] :  Use OTF \'COUNT\' number of times (ie. 2 = f(f(p)))\n");
  printf("DEBUGGING:\n");
  printf("   -d         :  Increases debugging level by one (can be used up to 3 times)\n");
  printf("                 Lvl 1 -> Shows output of entering ans exiting all functions.\n");
  printf("                 Lvl 2 -> Shows all return values from functions\n");
  printf("                 Lvl 3 -> Shows all arguments passed to functions\n");
  printf("   -l [FILE]  :  Set the logging device for debugging output (DEFAULT=stderr)\n");
  printf("MISC:\n");
  printf("   -h         :  Display help menu (you are looking at it) \n");
  printf("   -v         :  Print version number\n");
  return;
}

char* getVersion(){
  char rcs_vers[18] = "$Revision: 1.5 $";
  char* rcs_vers_cp,*version;
  
  /* Parse out version properly for display */
  rcs_vers_cp = strdup(rcs_vers);
  version = strtok(rcs_vers_cp," ");
  version = strtok(NULL," ");
  free(rcs_vers_cp);
  return version;
}

void main(int argc, char *argv[]){
  int n, cnt, i, dbgToFile = 0;
  int pass = 0;
  int dbgLevel = 0;
  char passwd[256], key[8], buf[33], *seed, *slash, *dbg, *dbg2, dbgFile[256];

  cnt = 1;

  while ((i = getopt (argc, argv, "vdhdn:p:l:")) != EOF){
    switch (i){
      case 'v':
        printf("S/Key Tool - v%s\n", getVersion());
        exit(0);
      case 'h':
        usage(argv[0]);
        exit(0);
      case 'd':
        dbgLevel++;
        break;
      case 'n':
        cnt = atoi(optarg);
        break;
      case 'p':
        strcpy(passwd, optarg);
        pass = 1;
        break;
      case 'l':
        strcpy(dbgFile, optarg);
        dbgToFile = 1;
        break;
    }
  }

  if(dbgToFile == 1) fprintf(stderr, "Sending debug output to file:%s\n", dbgFile);

  /* could be in the form <number>/<seed> */

  if(argc <= optind + 1){
    /* look for / in it */
    if(argc <= optind){
      usage(argv[0]);
      exit(1); 
    }

    _dbg_enter(_dbg_file,"strchr", 2, argv[optind], '/');
    slash = strchr(argv[optind], '/');
    _dbg_exit(_dbg_file,"strchr", slash);
    if(slash == NULL){
      usage(argv[0]);
      exit(1);
    }
    *slash++ = '\0';
    seed = slash;

    _dbg_enter(_dbg_file,"atoi", 1, argv[optind]);
    dbg = malloc(16);
    n = atoi(argv[optind]);
    snprintf(dbg, 16, "%d", n);
    _dbg_exit(_dbg_file,"atoi", dbg);
    free(dbg);

    if(n < 0){
      printf("%s not positive\n", argv[optind]);
      usage(argv[0]);
      exit(1);
    }
  }else{
    if(n < 0){
      printf("%s not positive\n", argv[optind]);
      usage(argv[0]);
      exit(1);
    }
    seed = argv[++optind];
  }

  /* Get user's secret password */
  if(!pass){
    printf("Enter secret password: ");
    readpass(passwd, sizeof(passwd));
  }
  _dbg_enter(_dbg_file,"rip", 1, passwd);
  rip(passwd);
  _dbg_exit(_dbg_file,"rip", passwd);

  /* Crunch seed and password into starting key */
  _dbg_enter(_dbg_file,"keycrunch", 3, key, seed, passwd);
  dbg = malloc(16);
  i = keycrunch(key, seed, passwd);
  snprintf(dbg, 16, "%d", i);
  _dbg_exit(_dbg_file,"keycrunch", dbg);
  free(dbg);
  
  if(i != 0){
    fprintf(stderr, "%s: key crunch failed\n", argv[0]);
    exit(1);
  }
  if(cnt == 1){
    while (n-- != 0){
      _dbg_enter(_dbg_file, "f", 1, key);
      f(key);
      _dbg_exit(_dbg_file, "f", key);
    }
    _dbg_enter(_dbg_file, "btoe", 2, buf, key);
    dbg = btoe(buf, key);
    _dbg_exit(_dbg_file, "btoe", dbg);
    printf ("%s\n", dbg);
#ifdef	HEXIN
    _dbg_enter(_dbg_file, "btoe", 2, buf, key);
    dbg = put8(buf, key);
    _dbg_exit(_dbg_file, "btoe", dbg);
    printf ("%s\n", dbg);
#endif
  }else{
    for (i = 0; i <= n - cnt; i++){
      _dbg_enter(_dbg_file, "f", 1, key);
      f(key);
      _dbg_exit(_dbg_file, "f", key);
    }
    for (; i <= n; i++){
      _dbg_enter(_dbg_file, "btoe", 2, buf, key);
      dbg = btoe(buf,key);
      _dbg_exit(_dbg_file, "btoe", dbg);
#ifdef	HEXIN
      _dbg_enter(_dbg_file, "put8", 2, buf, key);
      dbg2 = put8(buf, key);
      _dbg_exit(_dbg_file, "put8", dbg2)
      printf ("%d: %-29s  %s\n", i, dbg, dbg2);
#else
      printf ("%d: %-29s\n", i, dbg);
#endif
      _dbg_enter(_dbg_file, "f", 1, key);
      f(key);
      _dbg_exit(_dbg_file, "f", key);
    }
  }
  exit(0);
}
