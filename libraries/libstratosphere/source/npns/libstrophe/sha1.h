/* public api for steve reid's public domain SHA-1 implementation */
/* this file is in the public domain */

/** @file
 *  SHA-1 hash API.
 */

#ifndef __LIBSTROPHE_SHA1_H__
#define __LIBSTROPHE_SHA1_H__

#ifdef __cplusplus
extern "C" {
#endif

/* make sure the stdint.h types are available */
#include "ostypes.h"
#include <switch.h>

#define SHA1_CTX Sha1Context

#define SHA1_DIGEST_SIZE 20

#define crypto_SHA1_Init   sha1ContextCreate
#define crypto_SHA1_Update sha1ContextUpdate
#define crypto_SHA1_Final  sha1ContextGetHash
void crypto_SHA1(const uint8_t* data, size_t len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif /* __LIBSTROPHE_SHA1_H__ */
