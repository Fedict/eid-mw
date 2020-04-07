/*
 * Copyright (c) 2010,2011,2012,2013,2014,2015 Apple Inc. All rights reserved.
 * 
 * corecrypto Internal Use License Agreement
 * 
 * IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
 * in consideration of your agreement to the following terms, and your download or use
 * of this Apple software constitutes acceptance of these terms.  If you do not agree
 * with these terms, please do not download or use this Apple software.
 * 
 * 1.	As used in this Agreement, the term "Apple Software" collectively means and
 * includes all of the Apple corecrypto materials provided by Apple here, including
 * but not limited to the Apple corecrypto software, frameworks, libraries, documentation
 * and other Apple-created materials. In consideration of your agreement to abide by the
 * following terms, conditioned upon your compliance with these terms and subject to
 * these terms, Apple grants you, for a period of ninety (90) days from the date you
 * download the Apple Software, a limited, non-exclusive, non-sublicensable license
 * under Apple’s copyrights in the Apple Software to make a reasonable number of copies
 * of, compile, and run the Apple Software internally within your organization only on
 * devices and computers you own or control, for the sole purpose of verifying the
 * security characteristics and correct functioning of the Apple Software; provided
 * that you must retain this notice and the following text and disclaimers in all
 * copies of the Apple Software that you make. You may not, directly or indirectly,
 * redistribute the Apple Software or any portions thereof. The Apple Software is only
 * licensed and intended for use as expressly stated above and may not be used for other
 * purposes or in other contexts without Apple's prior written permission.  Except as
 * expressly stated in this notice, no other rights or licenses, express or implied, are
 * granted by Apple herein.
 * 
 * 2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
 * OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
 * THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
 * SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
 * REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
 * ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
 * SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
 * OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
 * WILL CREATE A WARRANTY. 
 * 
 * 3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
 * IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
 * SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
 * NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 * 4.	This Agreement is effective until terminated. Your rights under this Agreement will
 * terminate automatically without notice from Apple if you fail to comply with any term(s)
 * of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
 * and destroy all copies, full or partial, of the Apple Software. This Agreement will be
 * governed and construed in accordance with the laws of the State of California, without
 * regard to its choice of law rules.
 * 
 * You may report security issues about Apple products to product-security@apple.com,
 * as described here:  https://www.apple.com/support/security/.  Non-security bugs and
 * enhancement requests can be made via https://bugreport.apple.com as described
 * here: https://developer.apple.com/bug-reporting/
 *
 * EA1350 
 * 10/5/15
 */


#ifndef __BLOCKS__
#warning no blocks support
#else
/* Blocks support is required (gnu extension) */

#include <corecrypto/ccrng_CommonCrypto.h>
#include <corecrypto/ccrng_system.h>

#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#if CC_LINUX
#ifdef LINUX_BUILD_TEST
#define __rdtsc_time() 0   // To test compilation on mac
#else
#define __rdtsc_time() ({ uint64_t _t; asm volatile ("rdtsc" : "=a" (_t) :: "rdx"); _t; })
#endif // LINUX_BUILD_TEST
#else
# include <mach/mach_time.h>
#endif

#if CC_KERNEL
#include <sys/random.h>
#endif

#define RNG_MAX_SEED_RETRY 100

//
// Support routines for getting entropy from /dev/random.
// If we ever want to have an RNG that is not tied to /dev/random, but to some
// other form of entropy input to the underlying DRBG, then there will have
// to be parallel ones for these.
//

static int init_entropy(void);
static int get_entropy(void *entropy, size_t entropy_size, size_t *read_entropy_size);

#if CC_KERNEL
static int init_entropy(void) {
    return 0;
}

static int get_entropy(void *entropy, size_t entropy_size, size_t *read_entropy_size)
{
    read_random(entropy, entropy_size);
    *read_entropy_size=entropy_size;
    return 0;
}
#else

// Global state per process
struct ccrng_system_state entropy_src;
static dispatch_once_t	randinit;
static int dev_random_status = CC_ERR_INTERNAL;

// Return a corecrypto rng error code.
// Init once per process.
// Never close the source of entropy since the process can't
// start it init again.

static int init_entropy(void)
{
    dispatch_once(&randinit, ^{
            dev_random_status = ccrng_system_init(&entropy_src);
		});
	return dev_random_status;
}

// Return a corecrypto rng error code.
// Require init_entropy to have been executed once in the process.
static int get_entropy(void *entropy, size_t entropy_size, size_t *read_entropy_size)
{
    int status=ccrng_generate((struct ccrng_state *)&entropy_src, entropy_size, entropy);

    // Need to redo this read to deal with read not returing the
	//entropy_size
    if(status == 0) {
        *read_entropy_size=entropy_size;
    }
	return status;
}
#endif


// Mix in fresh entropy in the rng state.
// To be call within the RNG queue
static void prediction_break(struct ccrng_CommonCrypto_state *rng)
{
	char entropy[64];
	uint64_t now;
	size_t readSize;
    int status;

	// Get some entropy and use present time as additional input.
	status = get_entropy(entropy, sizeof(entropy), &readSize);
    if ((status==0) && (readSize > 0)) {
#if CC_LINUX
        now = __rdtsc_time();
#else
        now = mach_absolute_time();
#endif
        // Reseed.
        status=ccdrbg_reseed(rng->drbg_info, rng->drbg_state, (unsigned long)readSize, entropy, sizeof(now), &now);
    }
    rng->predictionBreak_status=status;
}

static int
ccrng_CommonCrypto_generate(struct ccrng_state *r, unsigned long count, void *bytes)
{
    struct ccrng_CommonCrypto_state *rng = (struct ccrng_CommonCrypto_state *)r;
	size_t	readAmount = CCDRBG_MAX_REQUEST_SIZE;
    size_t	readCount = count;

	// If we are strictFIPS, we read in CCDRBG_MAX_REQUEST_SIZE byte chunks.
	// Otherwise, just read the whole thing in at once.

	if (!rng->strictFIPS)
		readAmount = count;

	// Loop making calls to read in all the chunks.
	while (readCount > 0)
	{
        // Last chunk
        if (readCount < readAmount) {
            readAmount = readCount;
        }

		// It is possible that we could loop so many times that the DRBG will
		// need a reseed.
#if CC_KERNEL
        int gen_status = 0;
#else
        __block int gen_status = 0;
        dispatch_sync(rng->q, ^{
#endif
            int doWork = 1;

            // Exit after too many tries (DRBG in permanent reseed state)
            for (size_t i=0; (i<RNG_MAX_SEED_RETRY) && doWork && (gen_status==0); i++) {

                // Generate from DRBG state if predictionBreak is ready
                if (rng->predictionBreak_status==0) {
                    gen_status = ccdrbg_generate(rng->drbg_info, rng->drbg_state, readAmount, bytes, 0, NULL);
                    doWork = 0;
                }

                // If prediction break failed or if reseed is needed we mix in
                // some new entropy
                if ((gen_status==CCDRBG_STATUS_NEED_RESEED) || (rng->predictionBreak_status!=0))
                {
                    // Get entropy and reseed the drbg
                    prediction_break(rng);
                    gen_status = rng->predictionBreak_status;
                    doWork = 1; // Loop and try again to generate.
                }
			}
#if !CC_KERNEL
        });
#endif
        if (gen_status != 0)
        {
            return gen_status;
        }
		bytes += readAmount;
		readCount -= readAmount;
	}

	// If we're prediction-resistant, then break prediction
    // on a background thread.
    // Will check the status on next generate with predictionBreak_status
	if (rng->predictionResistant)
	{
#if !CC_KERNEL
		dispatch_async(rng->q, ^{
#endif
			prediction_break(rng);
#if !CC_KERNEL
		});
#endif
	}

	return 0;
}

int ccrng_CommonCrypto_init(struct ccrng_CommonCrypto_state *rng, const struct ccdrbg_info *info, struct ccdrbg_state *state, int options)
{

	struct timeval now;
    int entropy_status;
    int status;

    rng->drbg_info=info;
    rng->drbg_state=state;
    rng->predictionBreak_status=CC_ERR_INTERNAL;

    // Init entropy
    entropy_status=init_entropy();
    if (entropy_status)
    {
        return entropy_status;
    }

	// Get a nonce. NIST recommends using the time it's called
	// as a nonce. We will use the microseconds as additional
	// input. Inside the DRBG, the pointers are all just
	// catenated together, so it doesn't really matter how
	// we do it. It's one big nonce.

	gettimeofday(&now, NULL);

	// Get the option flags. I apologize for inverting the polarity here,
	// but I wanted a 0 value of options to be the sensible defaults.

	rng->strictFIPS = !(options & ccrngOptionIgnoreFIPS);
	rng->predictionResistant = !(options & ccrngOptionNoPredictionResistance);

    /* typecast: readSize will be less than the size of the entropy buffer */
    char entropy[64];
    size_t readSize;
    status = get_entropy(entropy, sizeof(entropy), &readSize);
    if (status==0 && readSize > 0)
    {
        status=ccdrbg_init(info, state,
                           (uint32_t) readSize, entropy,
                           sizeof(now.tv_sec), &now.tv_sec,
                           sizeof(now.tv_usec), &now.tv_usec);
    }
    rng->predictionBreak_status=status;
    cc_clear(sizeof(entropy), entropy);

    /* We might return an error here if the underlying DRBG init fails
       This may happen if, for example, the entropy length is not supported */
    if(status)
        return status;

	// Create a dispatch queue to run the RNG on.
    rng->generate = ccrng_CommonCrypto_generate;
    rng->q = dispatch_queue_create("CCRNG", NULL);
    dispatch_retain(rng->q);
	return 0;
}

int ccrng_CommonCrypto_done(struct ccrng_CommonCrypto_state *rng)
{
	// Do this synchronously on the queue so we know there aren't things pending.
#if !CC_KERNEL
	dispatch_sync(rng->q, ^{
#endif
		ccdrbg_done(rng->drbg_info, rng->drbg_state);
#if !CC_KERNEL
	});
    // Now release the queue.
    dispatch_release(rng->q);
    rng->generate = NULL;
#endif

	return 0;
}

#endif /* __BLOCKS__ */
