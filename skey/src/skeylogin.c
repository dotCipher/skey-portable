/* S/KEY v1.1b (skeylogin.c)
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
 * S/KEY verification check, lookups, and authentication.
 */

// Config include
#include "config.h"

// Dynamic includes
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#ifdef HAVE_SYSPARAM
#include <sys/param.h>
#endif
#ifdef HAVE_SYSQUOTA
#include <sys/quota.h>
#endif
#ifdef HAVE_UNISTD
#include <unistd.h>
#endif
#ifdef HAVE_TIME
#include <time.h>
#endif
#ifdef HAVE_SYSTIMEB
#include <sys/timeb.h>
#endif
#ifdef HAVE_ERRNO
#include <errno.h>
#endif

#include "skey.h"

#define	KEYFILE	"/etc/skeykeys"

char *skipspace();
int skeylookup(struct skey *mp,char *name);

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

/* Issue a skey challenge for user 'name'. If successful,
 * fill in the caller's skey structure and return 0. If unsuccessful
 * (e.g., if name is unknown) return -1.
 *
 * The file read/write pointer is left at the start of the
 * record.
 */
int getskeyprompt(struct skey* mp, char* name, char* prompt){
	int rval;

	sevenbit(name);
	rval = skeylookup(mp,name);
	strcpy(prompt,"s/key 55 latour1\n");
	switch(rval){
	case -1:	/* File error */
		return -1;
	case 0:		/* Lookup succeeded, return challenge */
		sprintf(prompt,"s/key %d %s\n",mp->n - 1,mp->seed);
		return 0;
	case 1:		/* User not found */
		fclose(mp->keyfile);
		return -1;
	}
	return -1;	/* Can't happen */
}	
/* Return  a skey challenge string for user 'name'. If successful,
 * fill in the caller's skey structure and return 0. If unsuccessful
 * (e.g., if name is unknown) return -1.
 *
 * The file read/write pointer is left at the start of the
 * record.
 */
int skeychallenge(struct skey* mp, char* name, char* ss){
	int rval;

	rval = skeylookup(mp,name);
	switch(rval){
	case -1:	/* File error */
		return -1;
	case 0:		/* Lookup succeeded, issue challenge */
                sprintf(ss, "s/key %d %s",mp->n - 1,mp->seed);
		return 0;
	case 1:		/* User not found */
		fclose(mp->keyfile);
		return -1;
	}
	return -1;	/* Can't happen */
}	

/* Find an entry in the One-time Password database.
 * Return codes:
 * -1: error in opening database
 *  0: entry found, file R/W pointer positioned at beginning of record
 *  1: entry not found, file R/W pointer positioned at EOF
 */
int skeylookup(struct skey* mp, char* name){
	int found;
	int len;
	long recstart;
	char *cp;
	struct stat statbuf;

	/* See if the KEYFILE exists, and create it if not */
	if(stat(KEYFILE,&statbuf) == -1 && errno == ENOENT){
		mp->keyfile = fopen(KEYFILE,"w+");
	} else {
		/* Otherwise open normally for update */
		mp->keyfile = fopen(KEYFILE,"r+");
	}
	if(mp->keyfile == NULL)
		return -1;

	/* Look up user name in database */
	len = strlen(name);
	if(len > 8) len = 8;		/*  Added 8/2/91  -  nmh */
	found = 0;
	while(!feof(mp->keyfile)){
		recstart = ftell(mp->keyfile);
		mp->recstart = recstart;
		if(fgets(mp->buf,sizeof(mp->buf),mp->keyfile) != mp->buf){
			break;
		}
		rip(mp->buf);
		if(mp->buf[0] == '#')
			continue;	/* Comment */
		if((mp->logname = strtok(mp->buf," \t")) == NULL)
			continue;
		if((cp = strtok(NULL," \t")) == NULL)
			continue;
		mp->n = atoi(cp);
		if((mp->seed = strtok(NULL," \t")) == NULL)
			continue;
		if((mp->val = strtok(NULL," \t")) == NULL)
			continue;
		if(strlen(mp->logname) == len
		 && strncmp(mp->logname,name,len) == 0){
			found = 1;
			break;
		}
	}
	if(found){
		fseek(mp->keyfile,recstart,0);
		return 0;
	} else
		return 1;
}

/* Convert hex digit to binary integer */
int htoi(register char c){
	if('0' <= c && c <= '9')
		return c - '0';
	if('a' <= c && c <= 'f')
		return 10 + c - 'a';
	if('A' <= c && c <= 'F')
		return 10 + c - 'A';
	return -1;
}

/* Convert 8-byte hex-ascii string to binary array
 * Returns 0 on success, -1 on error
 */
int atob8(register char* out, register char* in){
	register int i;
	register int val;

	if (in == NULL || out == NULL)
		return -1;

	for(i=0;i<8;i++){
		if((in = skipspace(in)) == NULL)
			return -1;
		if((val = htoi(*in++)) == -1)
			return -1;
		*out = val << 4;

		if((in = skipspace(in)) == NULL)
			return -1;
		if((val = htoi(*in++)) == -1)
			return -1;
		*out++ |= val;
	}
	return 0;
}

/* Convert 8-byte binary array to hex-ascii string */
int btoa8(register char* out, register char* in){
	register int i;

	if(in == NULL || out == NULL)
		return -1;

	for(i=0;i<8;i++){
		sprintf(out,"%02x",*in++ & 0xff);
		out += 2;
	}
	return 0;
}

/* Verify response to a s/key challenge.
 *
 * Return codes:
 * -1: Error of some sort; database unchanged
 *  0:  Verify successful, database updated
 *  1:  Verify failed, database unchanged
 *
 * The database file is always closed by this call.
 */
int skeyverify(struct skey* mp, char* response){
	char key[8];
	char fkey[8];
	char filekey[8];
	time_t now;
	struct tm *tm;
	char tbuf[27];

	time(&now);
	tm = localtime(&now);
	strftime(tbuf, sizeof(tbuf), " %b %d,%Y %T", tm);

	if(response == NULL){
		fclose(mp->keyfile);
		return -1;
	}
	rip(response);

	/* Convert response to binary */
	if(etob(key, response) != 1 && atob8(key, response) != 0){
		/* Neither english words or ascii hex */
		fclose(mp->keyfile);
		return -1;
	}

	/* Compute fkey = f(key) */
	memcpy(fkey,key,sizeof(key));
        fflush (stdout);

	f(fkey);
	/* in order to make the window of update as short as possible
           we must do the comparison here and if OK write it back
           other wise the same password can be used twice to get in
  	   to the system
	*/

	setpriority(PRIO_PROCESS, 0, -4);

	/* reread the file record NOW*/

	fseek(mp->keyfile,mp->recstart,0);
	if(fgets(mp->buf,sizeof(mp->buf),mp->keyfile) != mp->buf){
		setpriority(PRIO_PROCESS, 0, 0);
		fclose(mp->keyfile);
		return -1;
	}
	rip(mp->buf);
	mp->logname = strtok(mp->buf," \t");
	mp->seed = strtok(NULL," \t");
	mp->val = strtok(NULL," \t");
	/* And convert file value to hex for comparison */
	atob8(filekey,mp->val);

	/* Do actual comparison */
	fflush (stdout);

	if(memcmp(filekey,fkey,8) != 0){
		/* Wrong response */
		setpriority(PRIO_PROCESS, 0, 0);
		fclose(mp->keyfile);
		return 1;
	}

	/* Update key in database by overwriting entire record. Note
	 * that we must write exactly the same number of bytes as in
	 * the original record (note fixed width field for N)
	 */
	btoa8(mp->val,key);
	mp->n--;
	fseek(mp->keyfile,mp->recstart,0);
	fprintf(mp->keyfile,"%s %04d %-16s %s %-21s\n",mp->logname,mp->n,mp->seed,
	 mp->val, tbuf);

	fclose(mp->keyfile);
	
	setpriority(PRIO_PROCESS, 0, 0);
	return 0;
}

char* skipspace(register char* cp){
	while(*cp == ' ' || *cp == '\t')
		cp++;

	if(*cp == '\0')
		return NULL;
	else
		return cp;
}

/*
 * skey_haskey ()
 *
 * Returns: 1 user doesnt exist, -1 fle error, 0 user exists.
 *
 */
int skey_haskey(char* username){
  struct skey skey;
  return (skeylookup (&skey, username));
}
 
/*
 * skey_keyinfo ()
 *
 * Returns the current sequence number and
 * seed for the passed user.
 *
 */
int skey_keyinfo(char* str, char* username){
  int i;
  struct skey skey;
 
  i = skeychallenge (&skey, username, str);
 
 /*
  if (i == -2)
     return 0;
 
  return 1;
  */
  return i;
}
 
/*
 * skey_passcheck ()
 *
 * Check to see if answer is the correct one to the current
 * challenge.
 *
 * Returns: 0 success, -1 failure
 *
 */
int skey_passcheck(char* username, char* passwd){
  int i;
  struct skey skey;
 
  i = skeylookup (&skey, username);
 
  if (i == -1 || i == 1)
      return -1;
 
  if (skeyverify (&skey, passwd) == 0)
      return skey.n;
 
  return -1;
}

/*
 * skey_authenticate ()
 *
 * Used when calling program will allow input of the user's
 * response to the challenge.
 *
 * Returns: 0 success, -1 failure
 *
 */
int skey_authenticate(char* username){
  int i;
  char pbuf [256], skeyprompt [50];
  struct skey skey;
 
  /* Attempt a S/Key challenge */
  i = skeychallenge (&skey, username, skeyprompt);
 
  if (i == -2)
    return 0;
 
  printf ("[%s]\n", skeyprompt);
  fflush (stdout);
 
  printf ("Response: ");
  readpass (pbuf, sizeof (pbuf));
  rip (pbuf);
 
  /* Is it a valid response? */
  if (i == 0 && skeyverify (&skey, pbuf) == 0){
    if (skey.n < 5){
      printf ("\nWarning! Key initialization needed soon.  ");
      printf ("(%d logins left)\n", skey.n);
    }
    return 0;
  }
  return -1;
}

