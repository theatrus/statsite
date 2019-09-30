#ifndef _RAND_H_
#define _RAND_H_

/**
 * Gather len bytes of entropy from the system entropy source,
 * e.g. /dev/urandom
 */
extern size_t rand_gather(char* bytes, size_t len);

#endif
