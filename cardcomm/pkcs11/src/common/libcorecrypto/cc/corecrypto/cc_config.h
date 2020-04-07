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


#ifndef _CORECRYPTO_CC_CONFIG_H_
#define _CORECRYPTO_CC_CONFIG_H_

/* A word about configuration macros:
 
    Conditional configuration macros specific to corecrypto should be named CORECRYPTO_xxx
    or CCxx_yyy and be defined to be either 0 or 1 in this file. You can add an 
    #ifndef #error construct at the end of this file to make sure it's always defined.

    They should always be tested using the #if directive, never the #ifdef directive.

    No other conditional macros shall ever be used (except in this file)

    Configuration Macros that are defined outside of corecrypto (eg: KERNEL, DEBUG, ...)
    shall only be used in this file to define CCxxx macros.
 
    External macros should be assumed to be either undefined, defined with no value,
    or defined as true or false. We shall strive to build with -Wundef whenever possible,
    so the following construct should be used to test external macros in this file:
  
         #if defined(DEBUG) && (DEBUG)
         #define CORECRYPTO_DEBUG 1
         #else
         #define CORECRYPTO_DEBUG 0
         #endif
  

    It is acceptable to define a conditional CC_xxxx macro in an implementation file,
    to be used only in this file.
 
    The current code is not guaranteed to follow those rules, but should be fixed to.
 
    Corecrypto requires GNU and C99 compatibility.
    Typically enabled by passing --gnu --c99 to the compiler (eg. armcc)

*/

#if defined(DEBUG) && (DEBUG)
/* CC_DEBUG is already used in CommonCrypto */
#define CORECRYPTO_DEBUG 1
#else
#define CORECRYPTO_DEBUG 0
#endif

#if defined(KERNEL) && (KERNEL)
#define CC_KERNEL 1 // KEXT, XNU repo or kernel components such as AppleKeyStore
#else
#define CC_KERNEL 0
#endif

// LINUX_BUILD_TEST is for sanity check of the configuration
// > xcodebuild -scheme "corecrypto_test" OTHER_CFLAGS="$(values) -DLINUX_BUILD_TEST"
#if defined(__linux__) || defined(LINUX_BUILD_TEST)
#define CC_LINUX 1
#else
#define CC_LINUX 0
#endif

#if defined(USE_L4) && (USE_L4)
#define CC_USE_L4 1
#else
#define CC_USE_L4 0
#endif

#if defined(USE_SEPROM) && (USE_SEPROM)
#define CC_USE_SEPROM 1
#else
#define CC_USE_SEPROM 0
#endif

#if defined(USE_S3) && (USE_S3)
#define CC_USE_S3 1
#else
#define CC_USE_S3 0
#endif

#if (defined(ICE_FEATURES_ENABLED)) \
    || (defined(MAVERICK) && (MAVERICK))
#define CC_BASEBAND 1
#else
#define CC_BASEBAND 0
#endif

#if defined(EFI) && (EFI)
#define CC_EFI 1
#else
#define CC_EFI 0
#endif

#if defined(IBOOT) && (IBOOT)
#define CC_IBOOT 1
#else
#define CC_IBOOT 0
#endif

// BB configuration
#if CC_BASEBAND

// -- ENDIANESS
#if defined(ENDIAN_LITTLE) || (defined(__arm__) && !defined(__BIG_ENDIAN))
#define __LITTLE_ENDIAN__
#elif !defined(ENDIAN_BIG) && !defined(__BIG_ENDIAN)
#error Baseband endianess not defined.
#endif
#define AESOPT_ENDIAN_NO_FILE

// -- Architecture
#define CCN_UNIT_SIZE  4 // 32 bits
#define aligned(x) aligned((x)>8?8:(x))   // Alignment on 8 bytes max
#define SAFE_IO          // AES support for unaligned Input/Output

// -- External function
#define assert ASSERT   // sanity

// -- Warnings
// Ignore irrelevant warnings after verification
// #1254-D: arithmetic on pointer to void or function type
// #186-D: pointless comparison of unsigned integer with zero
// #546-D: transfer of control bypasses initialization of
#ifdef __arm__

#pragma diag_suppress 186, 1254,546
#elif defined(__GNUC__)
// warning: pointer of type 'void *' used in arithmetic
#pragma GCC diagnostic ignored "-Wpointer-arith"
#endif // arm or gnuc

#endif // CC_BASEBAND

#if !defined(CCN_UNIT_SIZE)
#if defined(__arm64__) || defined(__x86_64__)
#define CCN_UNIT_SIZE  8
#elif defined(__arm__) || defined(__i386__)
#define CCN_UNIT_SIZE  4
#else
#define CCN_UNIT_SIZE  2
#endif
#endif /* !defined(CCN_UNIT_SIZE) */

#if defined(__arm64__) || defined(__arm__)
#define CCN_IOS				   1
#define CCN_OSX				   0
#elif defined(__x86_64__) || defined(__i386__)
#define CCN_IOS				   0
#define CCN_OSX				   1
#endif 

#if CC_USE_L4 || CC_USE_S3
/* No dynamic linking allowed in L4, e.g. avoid nonlazy symbols */
/* For corecrypto kext, CC_STATIC should be undefined */
#define CC_STATIC              1
#endif

#if CC_USE_L4 || CC_IBOOT
/* For L4, stack is too short, need to use HEAP for some computations */
/* CC_USE_HEAP_FOR_WORKSPACE not supported for KERNEL!  */
#define CC_USE_HEAP_FOR_WORKSPACE 1
#else
#define CC_USE_HEAP_FOR_WORKSPACE 0
#endif

/* memset_s is only available in few target */
#if CC_USE_L4 || CC_KERNEL || CC_IBOOT || CC_USE_SEPROM || defined(__CC_ARM) \
    || defined(__hexagon__) || CC_EFI
#define CC_HAS_MEMSET_S 0
#else
#define CC_HAS_MEMSET_S 1
#endif


#if defined(__CC_ARM) || defined(__hexagon__) || defined(__NO_ASM__) \
    || CC_LINUX || CC_EFI
// ARMASM.exe does not to like the file syntax of the asm implementation
#define CCN_DEDICATED_SQR      1
#define CCN_MUL_KARATSUBA      1 // 4*n CCN_UNIT extra memory required.
#define CCN_ADD_ASM            0
#define CCN_SUB_ASM            0
#define CCN_MUL_ASM            0
#define CCN_ADDMUL1_ASM        0
#define CCN_MUL1_ASM           0
#define CCN_CMP_ASM            0
#define CCN_ADD1_ASM           0
#define CCN_SUB1_ASM           0
#define CCN_N_ASM              0
#define CCN_SET_ASM            0
#define CCAES_ARM              0
#define CCAES_INTEL            0
#define CCN_USE_BUILTIN_CLZ    0
#if !defined(__NO_ASM__)
#define CCSHA1_VNG_INTEL       0
#define CCSHA2_VNG_INTEL       0
#define CCSHA1_VNG_ARMV7NEON   0
#define CCSHA2_VNG_ARMV7NEON   0
#endif
#define CCAES_MUX              0

#elif defined(__arm__)
#define CCN_DEDICATED_SQR      1
#define CCN_MUL_KARATSUBA      0 // No perf improvement.
#define CCN_ADD_ASM            1
#define CCN_SUB_ASM            1
#define CCN_MUL_ASM            0
#define CCN_ADDMUL1_ASM        1
#define CCN_MUL1_ASM           1
#define CCN_CMP_ASM            1
#define CCN_ADD1_ASM           0
#define CCN_SUB1_ASM           0
#define CCN_N_ASM              1
#define CCN_SET_ASM            1
#define CCAES_ARM              1
#define CCAES_INTEL            0
#if CC_KERNEL || CC_USE_L4 || CC_IBOOT || CC_USE_SEPROM || CC_USE_S3
#define CCAES_MUX              0
#else
#define CCAES_MUX              1
#endif
#define CCN_USE_BUILTIN_CLZ    1
#define CCSHA1_VNG_INTEL       0
#define CCSHA2_VNG_INTEL       0

#if defined(__ARM_NEON__) || CC_KERNEL
#define CCSHA1_VNG_ARMV7NEON   1
#define CCSHA2_VNG_ARMV7NEON   1
#else /* !defined(__ARM_NEON__) */
#define CCSHA1_VNG_ARMV7NEON   0
#define CCSHA2_VNG_ARMV7NEON   0
#endif /* !defined(__ARM_NEON__) */

#elif defined(__arm64__)
#define CCN_DEDICATED_SQR      1
#define CCN_MUL_KARATSUBA      1 // 4*n CCN_UNIT extra memory required.
#define CCN_ADD_ASM            1
#define CCN_SUB_ASM            1
#define CCN_MUL_ASM            1
#define CCN_ADDMUL1_ASM        0
#define CCN_MUL1_ASM           0
#define CCN_CMP_ASM            1
#define CCN_ADD1_ASM           0
#define CCN_SUB1_ASM           0
#define CCN_N_ASM              1
#define CCN_SET_ASM            0
#define CCAES_ARM              1
#define CCAES_INTEL            0
#define CCAES_MUX              0        // On 64bit SoC, asm is much faster than HW
#define CCN_USE_BUILTIN_CLZ    1
#define CCSHA1_VNG_INTEL       0
#define CCSHA2_VNG_INTEL       0
#define CCSHA1_VNG_ARMV7NEON   1		// reused this to avoid making change to xcode project, put arm64 assembly code with armv7 code
#define CCSHA2_VNG_ARMV7NEON   1

#elif defined(__x86_64__) || defined(__i386__)
#define CCN_DEDICATED_SQR      1
#define CCN_MUL_KARATSUBA      1 // 4*n CCN_UNIT extra memory required.
/* These assembly routines only work for a single CCN_UNIT_SIZE. */
#if (defined(__x86_64__) && CCN_UNIT_SIZE == 8) || (defined(__i386__) && CCN_UNIT_SIZE == 4)
#define CCN_ADD_ASM            1
#define CCN_SUB_ASM            1
#define CCN_MUL_ASM            1
#else
#define CCN_ADD_ASM            0
#define CCN_SUB_ASM            0
#define CCN_MUL_ASM            0
#endif

#if (defined(__x86_64__) && CCN_UNIT_SIZE == 8)
#define CCN_CMP_ASM            1
#define CCN_N_ASM              1
#else
#define CCN_CMP_ASM            0
#define CCN_N_ASM              0
#endif

#define CCN_ADDMUL1_ASM        0
#define CCN_MUL1_ASM           0
#define CCN_ADD1_ASM           0
#define CCN_SUB1_ASM           0
#define CCN_SET_ASM            0
#define CCAES_ARM              0
#define CCAES_INTEL            1
#define CCAES_MUX              0
#define CCN_USE_BUILTIN_CLZ    0
#define CCSHA1_VNG_INTEL       1
#define CCSHA2_VNG_INTEL       1
#define CCSHA1_VNG_ARMV7NEON   0
#define CCSHA2_VNG_ARMV7NEON   0

#else
#define CCN_DEDICATED_SQR      1
#define CCN_MUL_KARATSUBA      1 // 4*n CCN_UNIT extra memory required.
#define CCN_ADD_ASM            0
#define CCN_SUB_ASM            0
#define CCN_MUL_ASM            0
#define CCN_ADDMUL1_ASM        0
#define CCN_MUL1_ASM           0
#define CCN_CMP_ASM            0
#define CCN_ADD1_ASM           0
#define CCN_SUB1_ASM           0
#define CCN_N_ASM              0
#define CCN_SET_ASM            0
#define CCAES_ARM              0
#define CCAES_INTEL            0
#define CCAES_MUX              0
#define CCN_USE_BUILTIN_CLZ    0
#define CCSHA1_VNG_INTEL       0
#define CCSHA2_VNG_INTEL       0
#define CCSHA1_VNG_ARMV7NEON   0
#define CCSHA2_VNG_ARMV7NEON   0

#endif /* !defined(__i386__) */

#define CC_INLINE static inline

#ifdef __GNUC__
#define CC_NORETURN __attribute__((__noreturn__))
#define CC_NOTHROW __attribute__((__nothrow__))
// Transparent Union
#if defined(__CC_ARM) || defined(__hexagon__)
#define CC_NONNULL_TU(N)
#else
#define CC_NONNULL_TU(N) __attribute__((__nonnull__ N))
#endif
#define CC_NONNULL(N) __attribute__((__nonnull__ N))
#define CC_NONNULL1 __attribute__((__nonnull__(1)))
#define CC_NONNULL2 __attribute__((__nonnull__(2)))
#define CC_NONNULL3 __attribute__((__nonnull__(3)))
#define CC_NONNULL4 __attribute__((__nonnull__(4)))
#define CC_NONNULL5 __attribute__((__nonnull__(5)))
#define CC_NONNULL6 __attribute__((__nonnull__(6)))
#define CC_NONNULL7 __attribute__((__nonnull__(7)))
#define CC_NONNULL_ALL __attribute__((__nonnull__))
#define CC_SENTINEL __attribute__((__sentinel__))
#define CC_CONST __attribute__((__const__))
#define CC_PURE __attribute__((__pure__))
#define CC_WARN_RESULT __attribute__((__warn_unused_result__))
#define CC_MALLOC __attribute__((__malloc__))
#define CC_UNUSED __attribute__((unused))
#else /* !__GNUC__ */
/*! @parseOnly */
#define CC_UNUSED
/*! @parseOnly */
#define CC_NONNULL_TU(N)
/*! @parseOnly */
#define CC_NONNULL(N)
/*! @parseOnly */
#define CC_NORETURN
/*! @parseOnly */
#define CC_NOTHROW
/*! @parseOnly */
#define CC_NONNULL1
/*! @parseOnly */
#define CC_NONNULL2
/*! @parseOnly */
#define CC_NONNULL3
/*! @parseOnly */
#define CC_NONNULL4
/*! @parseOnly */
#define CC_NONNULL5
/*! @parseOnly */
#define CC_NONNULL6
/*! @parseOnly */
#define CC_NONNULL7
/*! @parseOnly */
#define CC_NONNULL_ALL
/*! @parseOnly */
#define CC_SENTINEL
/*! @parseOnly */
#define CC_CONST
/*! @parseOnly */
#define CC_PURE
/*! @parseOnly */
#define CC_WARN_RESULT
/*! @parseOnly */
#define CC_MALLOC
#endif /* !__GNUC__ */


#endif /* _CORECRYPTO_CC_CONFIG_H_ */
