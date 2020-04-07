/*
 * Copyright (c) 2010,2012,2013,2014,2015 Apple Inc. All rights reserved.
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


/* A very simple RNG for osx/ios user mode that just get random bytes from /dev/random */

#include <corecrypto/cc_config.h>
#include <corecrypto/ccrng_system.h>
#include <corecrypto/cc.h>
#include <corecrypto/cc_debug.h>

#if CC_KERNEL

#include <sys/types.h>
#include <sys/random.h>


static int get_kernel_entropy(CC_UNUSED struct ccrng_state *rng, unsigned long entropy_size, void *entropy)
{
    read_random(entropy, (u_int)entropy_size);
    return 0;
}


int ccrng_system_init(struct ccrng_system_state *rng)
{
    rng->generate = get_kernel_entropy;
    return 0;
}

void ccrng_system_done(CC_UNUSED struct ccrng_system_state *rng) { }


//#error This file is not supported in kernel mode. This is an RNG implementation using /dev/random.

#else

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#if !CC_LINUX
# include <TargetConditionals.h>
#endif

#define OP_INTERRUPT_MAX 1024

#if (CC_LINUX || \
    ((defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)  \
    && (defined(IPHONE_SIMULATOR_HOST_MIN_VERSION_REQUIRED) && IPHONE_SIMULATOR_HOST_MIN_VERSION_REQUIRED < 1090)) \
    || (defined(__MAC_OS_X_VERSION_MIN_REQUIRED) && __MAC_OS_X_VERSION_MIN_REQUIRED < 1090))
#undef USE_GUARDED_OPEN
#else
#include <sys/guarded.h>
#define USE_GUARDED_OPEN 1
const unsigned int guard_flags = GUARD_CLOSE | GUARD_DUP |
    GUARD_SOCKET_IPC | GUARD_FILEPORT;
#endif

#if CC_LINUX
# define DEV_RANDOM "/dev/urandom"
#else
# define DEV_RANDOM "/dev/random"
#endif

//extern int errno;
#include <sys/errno.h>

static int init_dev_random(struct ccrng_system_state *rng)
{

    int interrupts = 0;
    rng->fd = -1;
    while(rng->fd == -1) {
#ifdef USE_GUARDED_OPEN
        guardid_t guard = (uintptr_t)rng;
        rng->fd = guarded_open_np(DEV_RANDOM, &guard, guard_flags, O_RDONLY | O_CLOEXEC);
#else
        rng->fd = open(DEV_RANDOM, O_RDONLY | O_CLOEXEC);
#endif
        if(rng->fd != -1) {
            break;
        }
        switch(errno) {
            case EINTR:
                interrupts++;
                if(OP_INTERRUPT_MAX && interrupts > OP_INTERRUPT_MAX) return CC_ERR_INTERUPTS;
                break;
            case EACCES:
                return CC_ERR_PERMS;
            case ENFILE:
            case EMFILE:
                return CC_ERR_FILEDESC;
            case EISDIR:    /* FALLTHROUGH */
            case ELOOP:     /* FALLTHROUGH */
            case ENOENT:    /* FALLTHROUGH */
            case ENXIO:     /* FALLTHROUGH */
            default:
                return CC_ERR_CRYPTO_CONFIG;  // We might actually want to abort here - any of these
                                              // indicate a bad system.
        }
    }
    if (rng->fd>0) {
        return 0;  // success
    }
    return rng->fd;
}

static int get_entropy(struct ccrng_state *rng, unsigned long entropy_size, void *entropy)
{
    struct ccrng_system_state *thisrng = (struct ccrng_system_state *)rng;
    if(thisrng->fd < 0) return CC_ERR_DEVICE;
    int interrupts = 0;
    size_t pos = 0;
    
    while(entropy_size) {
        ssize_t read_now = read(thisrng->fd, entropy+pos, entropy_size);
        if(read_now > -1) {
            entropy_size -= read_now;
            pos += read_now;
        } else {
            switch(errno) {
                case EINTR:
                    interrupts++;
                    if(OP_INTERRUPT_MAX && interrupts > OP_INTERRUPT_MAX) return CC_ERR_INTERUPTS;
                    break;
                case EAGAIN:
                    break;
                case EBADF: /* FALLTHROUGH */
                case ENXIO:
                    return CC_ERR_DEVICE;
                case EACCES:
                    return CC_ERR_PERMS;
                case EFAULT:
                    return CC_ERR_PARAMETER;
                case ENOBUFS: /* FALLTHROUGH */
                case ENOMEM:
                    return CC_ERR_MEMORY;
                default:
                    return CC_ERR_CRYPTO_CONFIG;  // We might actually want to abort here - any of these
                    // indicate a bad system.
            }
        }
    }

    /* TODO: Proper error codes */
    return 0;
}

int ccrng_system_init(struct ccrng_system_state *rng)
{
    rng->generate=get_entropy;
    return init_dev_random(rng);
}

void ccrng_system_done(struct ccrng_system_state *rng) {
#ifdef USE_GUARDED_OPEN
    guardid_t guard = (uintptr_t)rng;
    guarded_close_np(rng->fd, &guard);
#else
    close(rng->fd);
#endif
    rng->fd = -1;
}

#endif
