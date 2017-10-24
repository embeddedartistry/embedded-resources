/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Posix rand_r function added May 1999 by Wes Peters <wes@softweyr.com>.
 */

#include <stdlib.h>

static unsigned long next = 1;
static int do_rand(unsigned long* ctx)
{
#ifdef USE_WEAK_SEEDING
	/*
	 * Historic implementation compatibility.
	 * The random sequences do not vary much with the seed,
	 * even with overflowing.
	 */
	return ((*ctx = *ctx * 1103515245 + 12345) % ((unsigned long)RAND_MAX + 1));
#else /* !USE_WEAK_SEEDING */
	/*
	 * Compute x = (7^5 * x) mod (2^31 - 1)
	 * without overflowing 31 bits:
	 *      (2^31 - 1) = 127773 * (7^5) + 2836
	 * From "Random number generators: good ones are hard to find",
	 * Park and Miller, Communications of the ACM, vol. 31, no. 10,
	 * October 1988, p. 1195.
	 */
	long hi, lo, x;

	/* Can't be initialized with 0, so use another value. */
	if(*ctx == 0)
		*ctx = 123459876;
	hi = *ctx / 127773;
	lo = *ctx % 127773;
	x = 16807 * lo - 2836 * hi;
	if(x < 0)
		x += 0x7fffffff;
	return ((*ctx = x) % ((unsigned long)RAND_MAX + 1));
#endif /* !USE_WEAK_SEEDING */
}

__attribute__((weak)) int rand_r(unsigned int* ctx)
{
	unsigned long val = (unsigned long)*ctx;
	int r = do_rand(&val);

	*ctx = (unsigned int)val;
	return (r);
}

__attribute__((weak)) int rand()
{
	return (do_rand(&next));
}

__attribute__((weak)) void srand(unsigned seed)
{
	next = seed;
}
