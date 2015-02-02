/* S/KEY v1.1b (skeysubr.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *
 * Modifications: 
 *          Scott Chasin <chasin@crimelab.com>
 *			Cody Moore <cjmoore@cs.sunysb.edu>
 *
 * S/KEY misc routines.
 */

#include "config.h"

#ifdef HAVE_STDIO
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB
#include <stdlib.h>
#endif
#ifdef HAVE_STRING
#include <string.h>
#endif
#ifdef HAVE_SIGNAL
#include <signal.h>
#endif
#include <sys/ioctl.h>



struct sgttyb newtty;
struct sgttyb oldtty;
struct tchars chars;

#define SIGTYPE void

SIGTYPE trapped();

#include "md4.h"
#include "skey.h"

/* Crunch a key:
 * concatenate the seed and the password, run through MD4 and
 * collapse to 64 bits. This is defined as the user's starting key.
 */
int keycrunch(char* result, char* seed, char* passwd){
	/* result = 8-byte result */
	/* seed = Seed, any length */
	/* passwd = Password, any length */
	char *buf;
	MDstruct md;
	unsigned int buflen;
#ifndef	IS_LITTLE_ENDIAN
	int i;
	register long tmp;
#endif
	
	buflen = strlen(seed) + strlen(passwd);
	if ((buf = (char *)malloc(buflen+1)) == NULL)
		return -1;
	strcpy(buf,seed);
	strcat(buf,passwd);

	/* Crunch the key through MD4 */
	sevenbit(buf);
	MDbegin(&md);
	MDupdate(&md,(unsigned char *)buf,8*buflen);

	free(buf);

	/* Fold result from 128 to 64 bits */
	md.buffer[0] ^= md.buffer[2];
	md.buffer[1] ^= md.buffer[3];

#ifdef	IS_LITTLE_ENDIAN
	/* Only works on byte-addressed little-endian machines!! */
	memcpy(result,(char *)md.buffer,8);
#else
	/* Default (but slow) code that will convert to
	 * little-endian byte ordering on any machine
	 */
	for (i=0; i<2; i++) {
		tmp = md.buffer[i];
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
		tmp >>= 8;
		*result++ = tmp;
	}
#endif

	return 0;
}

/* The one-way function f(). Takes 8 bytes and returns 8 bytes in place */
void f(char* x){
	MDstruct md;
#ifndef	IS_LITTLE_ENDIAN
	register long tmp;
#endif

	MDbegin(&md);
	MDupdate(&md,(unsigned char *)x,64);

	/* Fold 128 to 64 bits */
	md.buffer[0] ^= md.buffer[2];
	md.buffer[1] ^= md.buffer[3];

#ifdef	IS_LITTLE_ENDIAN
	/* Only works on byte-addressed little-endian machines!! */
	memcpy(x,(char *)md.buffer,8);

#else
	/* Default (but slow) code that will convert to
	 * little-endian byte ordering on any machine
	 */
	tmp = md.buffer[0];
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;

	tmp = md.buffer[1];
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x++ = tmp;
	tmp >>= 8;
	*x = tmp;
#endif
}

/* Strip trailing cr/lf from a line of text */
void rip (char* buf){
	char *cp;

	if((cp = strchr(buf,'\r')) != NULL)
		*cp = '\0';

	if((cp = strchr(buf,'\n')) != NULL)
		*cp = '\0';
}

void set_term(){
    gtty (fileno(stdin), &newtty);
    gtty (fileno(stdin), &oldtty);
 
    signal (SIGINT, trapped);
}

void echo_off(){
#ifdef HAVE_TERMIOS
    newtty.c_lflag &= ~(ICANON | ECHO | ECHONL);
#else
    newtty.sg_flags |= CBREAK;
    newtty.sg_flags &= ~ECHO;
#endif

#ifdef HAVE_TERMIOS
    newtty.c_cc[VMIN] = 1;
    newtty.c_cc[VTIME] = 0;
    newtty.c_cc[VINTR] = 3;
#else
    ioctl(fileno(stdin), TIOCGETC, &chars);
    chars.t_intrc = 3;
    ioctl(fileno(stdin), TIOCSETC, &chars);
#endif

    stty (fileno (stdin), &newtty);
}

void unset_term(){
    stty (fileno (stdin), &oldtty);
 
#ifndef HAVE_TERMIOS
    ioctl(fileno(stdin), TIOCSETC, &chars);
#endif
}

void trapped(){
  signal (SIGINT, trapped);
  printf ("^C\n");
  unset_term ();
  exit (-1);
 }

//#endif

/* removebackspaced over charaters from the string */
void backspace(char* buf){
	char bs = 0x8;
	char *cp = buf;
	char *out = buf;

	while(*cp){
		if( *cp == bs ) {
			if(out == buf){
				cp++;
				continue;
			} else {
			  cp++;
			  out--;
			}
		} else {
			*out++ = *cp++;
		}

	}
	*out = '\0';
	
}
