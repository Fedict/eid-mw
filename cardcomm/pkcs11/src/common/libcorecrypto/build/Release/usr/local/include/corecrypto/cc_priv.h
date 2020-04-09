/*
 * Copyright (c) 2010,2011,2012,2014,2015 Apple Inc. All rights reserved.
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


#ifndef _CORECRYPTO_CC_PRIV_H_
#define _CORECRYPTO_CC_PRIV_H_

#include <corecrypto/cc.h>
#include <stdint.h>

/* defines the following macros :

 CC_MEMCPY  : optimized memcpy.
 CC_MEMMOVE : optimized memmove.
 CC_MEMSET  : optimized memset.

 CC_STORE32_BE : store 32 bit value in big endian in unaligned buffer.
 CC_STORE32_LE : store 32 bit value in little endian in unaligned buffer.
 CC_STORE64_BE : store 64 bit value in big endian in unaligned buffer.
 CC_STORE64_LE : store 64 bit value in little endian in unaligned buffer.

 CC_LOAD32_BE : load 32 bit value in big endian from unaligned buffer.
 CC_LOAD32_LE : load 32 bit value in little endian from unaligned buffer.
 CC_LOAD64_BE : load 64 bit value in big endian from unaligned buffer.
 CC_LOAD64_LE : load 64 bit value in little endian from unaligned buffer.

 CC_ROR  : Rotate Right 32 bits. Rotate count can be a variable.
 CC_ROL  : Rotate Left 32 bits. Rotate count can be a variable.
 CC_RORc : Rotate Right 32 bits. Rotate count must be a constant.
 CC_ROLc : Rotate Left 32 bits. Rotate count must be a constant.

 CC_ROR64  : Rotate Right 64 bits. Rotate count can be a variable.
 CC_ROL64  : Rotate Left 64 bits. Rotate count can be a variable.
 CC_ROR64c : Rotate Right 64 bits. Rotate count must be a constant.
 CC_ROL64c : Rotate Left 64 bits. Rotate count must be a constant.

 CC_BSWAP  : byte swap a 32 bits variable.

 CC_H2BE32 : convert a 32 bits value between host and big endian order.
 CC_H2LE32 : convert a 32 bits value between host and little endian order.

The following are not defined yet... define them if needed.

 CC_BSWAPc   : byte swap a 32 bits constant

 CC_BSWAP64  : byte swap a 64 bits variable
 CC_BSWAP64c : byte swap a 64 bits constant

 CC_READ_LE32 : read a 32 bits little endian value
 CC_READ_LE64 : read a 64 bits little endian value
 CC_READ_BE32 : read a 32 bits big endian value
 CC_READ_BE64 : read a 64 bits big endian value

 CC_WRITE_LE32 : write a 32 bits little endian value
 CC_WRITE_LE64 : write a 64 bits little endian value
 CC_WRITE_BE32 : write a 32 bits big endian value
 CC_WRITE_BE64 : write a 64 bits big endian value

 CC_H2BE64 : convert a 64 bits value between host and big endian order
 CC_H2LE64 : convert a 64 bits value between host and little endian order

*/

/* TODO: optimized versions */
#define CC_MEMCPY(D,S,L) memcpy((D),(S),(L))
#define CC_MEMMOVE(D,S,L) memmove((D),(S),(L))
#define CC_MEMSET(D,V,L) memset((D),(V),(L))

// MARK: - Loads and Store

// MARK: -- 32 bits - little endian

// MARK: --- Default version

#define	CC_STORE32_LE(x, y) do {                                    \
    ((unsigned char *)(y))[3] = (unsigned char)(((x)>>24)&255);		\
    ((unsigned char *)(y))[2] = (unsigned char)(((x)>>16)&255);		\
    ((unsigned char *)(y))[1] = (unsigned char)(((x)>>8)&255);		\
    ((unsigned char *)(y))[0] = (unsigned char)((x)&255);			\
} while(0)

#define	CC_LOAD32_LE(x, y) do {                                     \
x = ((uint32_t)(((const unsigned char *)(y))[3] & 255)<<24) |			    \
    ((uint32_t)(((const unsigned char *)(y))[2] & 255)<<16) |			    \
    ((uint32_t)(((const unsigned char *)(y))[1] & 255)<<8)  |			    \
    ((uint32_t)(((const unsigned char *)(y))[0] & 255));				    \
} while(0)

// MARK: -- 64 bits - little endian

#define	CC_STORE64_LE(x, y) do {                                    \
    ((unsigned char *)(y))[7] = (unsigned char)(((x)>>56)&255);     \
    ((unsigned char *)(y))[6] = (unsigned char)(((x)>>48)&255);		\
    ((unsigned char *)(y))[5] = (unsigned char)(((x)>>40)&255);		\
    ((unsigned char *)(y))[4] = (unsigned char)(((x)>>32)&255);		\
    ((unsigned char *)(y))[3] = (unsigned char)(((x)>>24)&255);		\
    ((unsigned char *)(y))[2] = (unsigned char)(((x)>>16)&255);		\
    ((unsigned char *)(y))[1] = (unsigned char)(((x)>>8)&255);		\
    ((unsigned char *)(y))[0] = (unsigned char)((x)&255);			\
} while(0)

#define	CC_LOAD64_LE(x, y) do {                                     \
x = (((uint64_t)(((const unsigned char *)(y))[7] & 255))<<56) |           \
    (((uint64_t)(((const unsigned char *)(y))[6] & 255))<<48) |           \
    (((uint64_t)(((const unsigned char *)(y))[5] & 255))<<40) |           \
    (((uint64_t)(((const unsigned char *)(y))[4] & 255))<<32) |           \
    (((uint64_t)(((const unsigned char *)(y))[3] & 255))<<24) |           \
    (((uint64_t)(((const unsigned char *)(y))[2] & 255))<<16) |           \
    (((uint64_t)(((const unsigned char *)(y))[1] & 255))<<8)  |           \
    (((uint64_t)(((const unsigned char *)(y))[0] & 255)));                \
} while(0)

// MARK: -- 32 bits - big endian
// MARK: --- intel version

#if (defined(__i386__) || defined(__x86_64__))

#define CC_STORE32_BE(x, y)     \
    __asm__ __volatile__ (      \
    "bswapl %0     \n\t"        \
    "movl   %0,(%1)\n\t"        \
    "bswapl %0     \n\t"        \
    ::"r"(x), "r"(y))

#define CC_LOAD32_BE(x, y)      \
    __asm__ __volatile__ (      \
    "movl (%1),%0\n\t"          \
    "bswapl %0\n\t"             \
    :"=r"(x): "r"(y))

#else
// MARK: --- default version
#define	CC_STORE32_BE(x, y) do {                                \
    ((unsigned char *)(y))[0] = (unsigned char)(((x)>>24)&255);	\
    ((unsigned char *)(y))[1] = (unsigned char)(((x)>>16)&255);	\
    ((unsigned char *)(y))[2] = (unsigned char)(((x)>>8)&255);	\
    ((unsigned char *)(y))[3] = (unsigned char)((x)&255);       \
} while(0)

#define	CC_LOAD32_BE(x, y) do {                             \
x = ((uint32_t)(((const unsigned char *)(y))[0] & 255)<<24) |	    \
    ((uint32_t)(((const unsigned char *)(y))[1] & 255)<<16) |		\
    ((uint32_t)(((const unsigned char *)(y))[2] & 255)<<8)  |		\
    ((uint32_t)(((const unsigned char *)(y))[3] & 255));          \
} while(0)

#endif

// MARK: -- 64 bits - big endian

// MARK: --- intel 64 bits version

#if defined(__x86_64__)

#define	CC_STORE64_BE(x, y)   \
__asm__ __volatile__ (        \
"bswapq %0     \n\t"          \
"movq   %0,(%1)\n\t"          \
"bswapq %0     \n\t"          \
::"r"(x), "r"(y))

#define	CC_LOAD64_BE(x, y)    \
__asm__ __volatile__ (        \
"movq (%1),%0\n\t"            \
"bswapq %0\n\t"               \
:"=r"(x): "r"(y))

#else

// MARK: --- default version

#define CC_STORE64_BE(x, y) do {                                    \
    ((unsigned char *)(y))[0] = (unsigned char)(((x)>>56)&255);		\
    ((unsigned char *)(y))[1] = (unsigned char)(((x)>>48)&255);		\
    ((unsigned char *)(y))[2] = (unsigned char)(((x)>>40)&255);		\
    ((unsigned char *)(y))[3] = (unsigned char)(((x)>>32)&255);		\
    ((unsigned char *)(y))[4] = (unsigned char)(((x)>>24)&255);		\
    ((unsigned char *)(y))[5] = (unsigned char)(((x)>>16)&255);		\
    ((unsigned char *)(y))[6] = (unsigned char)(((x)>>8)&255);		\
    ((unsigned char *)(y))[7] = (unsigned char)((x)&255);			\
} while(0)

#define	CC_LOAD64_BE(x, y) do {                                     \
x = (((uint64_t)(((const unsigned char *)(y))[0] & 255))<<56) |           \
    (((uint64_t)(((const unsigned char *)(y))[1] & 255))<<48) |           \
    (((uint64_t)(((const unsigned char *)(y))[2] & 255))<<40) |           \
    (((uint64_t)(((const unsigned char *)(y))[3] & 255))<<32) |           \
    (((uint64_t)(((const unsigned char *)(y))[4] & 255))<<24) |           \
    (((uint64_t)(((const unsigned char *)(y))[5] & 255))<<16) |           \
    (((uint64_t)(((const unsigned char *)(y))[6] & 255))<<8)  |          	\
    (((uint64_t)(((const unsigned char *)(y))[7] & 255)));	            \
} while(0)

#endif

// MARK: - 32-bit Rotates

#if defined(_MSC_VER)
// MARK: -- MSVC version

#include <stdlib.h>
#pragma intrinsic(_lrotr,_lrotl)
#define	CC_ROR(x,n) _lrotr(x,n)
#define	CC_ROL(x,n) _lrotl(x,n)
#define	CC_RORc(x,n) _lrotr(x,n)
#define	CC_ROLc(x,n) _lrotl(x,n)

#elif (defined(__i386__) || defined(__x86_64__))
// MARK: -- intel asm version

CC_INLINE uint32_t CC_ROL(uint32_t word, int i)
{
    __asm__ ("roll %%cl,%0"
         :"=r" (word)
         :"0" (word),"c" (i));
    return word;
}

CC_INLINE uint32_t CC_ROR(uint32_t word, int i)
{
    __asm__ ("rorl %%cl,%0"
         :"=r" (word)
         :"0" (word),"c" (i));
    return word;
}

/* Need to be a macro here, because 'i' is an immediate (constant) */
#define CC_ROLc(word, i)                \
({  uint32_t _word=(word);              \
    __asm__ __volatile__ ("roll %2,%0"  \
        :"=r" (_word)                   \
        :"0" (_word),"I" (i));          \
    _word;                              \
})


#define CC_RORc(word, i)                \
({  uint32_t _word=(word);              \
    __asm__ __volatile__ ("rorl %2,%0"  \
        :"=r" (_word)                   \
        :"0" (_word),"I" (i));          \
    _word;                              \
})

#else

// MARK: -- default version

CC_INLINE uint32_t CC_ROL(uint32_t word, int i)
{
    return ( (word<<(i&31)) | (word>>(32-(i&31))) );
}

CC_INLINE uint32_t CC_ROR(uint32_t word, int i)
{
    return ( (word>>(i&31)) | (word<<(32-(i&31))) );
}

#define	CC_ROLc(x, y) CC_ROL(x, y)
#define	CC_RORc(x, y) CC_ROR(x, y)

#endif

// MARK: - 64 bits rotates

#if defined(__x86_64__)
// MARK: -- intel 64 asm version

CC_INLINE uint64_t CC_ROL64(uint64_t word, int i)
{
    __asm__("rolq %%cl,%0"
        :"=r" (word)
        :"0" (word),"c" (i));
    return word;
}

CC_INLINE uint64_t CC_ROR64(uint64_t word, int i)
{
    __asm__("rorq %%cl,%0"
        :"=r" (word)
        :"0" (word),"c" (i));
    return word;
}

/* Need to be a macro here, because 'i' is an immediate (constant) */
#define CC_ROL64c(word, i)      \
({                              \
    uint64_t _word=(word);      \
    __asm__("rolq %2,%0"        \
        :"=r" (_word)           \
        :"0" (_word),"J" (i));  \
    _word;                      \
})

#define CC_ROR64c(word, i)      \
({                              \
    uint64_t _word=(word);      \
    __asm__("rorq %2,%0"        \
        :"=r" (_word)           \
        :"0" (_word),"J" (i));  \
    _word;                      \
})


#else /* Not x86_64  */

// MARK: -- default C version

CC_INLINE uint64_t CC_ROL64(uint64_t word, int i)
{
    return ( (word<<(i&63)) | (word>>(64-(i&63))) );
}

CC_INLINE uint64_t CC_ROR64(uint64_t word, int i)
{
    return ( (word>>(i&63)) | (word<<(64-(i&63))) );
}

#define	CC_ROL64c(x, y) CC_ROL64(x, y)
#define	CC_ROR64c(x, y) CC_ROR64(x, y)

#endif


// MARK: - Byte Swaps

CC_INLINE uint32_t CC_BSWAP(uint32_t x)
{
    return (
        ((x>>24)&0x000000FF) |
        ((x<<24)&0xFF000000) |
        ((x>>8) &0x0000FF00) |
        ((x<<8) &0x00FF0000)
    );
}

#define CC_BSWAP64(x) \
((uint64_t)((((uint64_t)(x) & 0xff00000000000000ULL) >> 56) | \
(((uint64_t)(x) & 0x00ff000000000000ULL) >> 40) | \
(((uint64_t)(x) & 0x0000ff0000000000ULL) >> 24) | \
(((uint64_t)(x) & 0x000000ff00000000ULL) >>  8) | \
(((uint64_t)(x) & 0x00000000ff000000ULL) <<  8) | \
(((uint64_t)(x) & 0x0000000000ff0000ULL) << 24) | \
(((uint64_t)(x) & 0x000000000000ff00ULL) << 40) | \
(((uint64_t)(x) & 0x00000000000000ffULL) << 56)))

#ifdef __LITTLE_ENDIAN__
#define CC_H2BE32(x) CC_BSWAP(x)
#define CC_H2LE32(x) (x)
#else
#define CC_H2BE32(x) (x)
#define CC_H2LE32(x) CC_BSWAP(x)
#endif


/* extract a byte portably */
#ifdef _MSC_VER
#define cc_byte(x, n) ((unsigned char)((x) >> (8 * (n))))
#else
#define cc_byte(x, n) (((x) >> (8 * (n))) & 255)
#endif

/* HEAVISIDE_STEP (shifted by one)
   function f(x): x->0, when x=0 
                  x->1, when x>0
   Can also be seen as a bitwise operation: 
      f(x): x -> y
        y[0]=(OR x[i]) for all i (all bits)
        y[i]=0 for all i>0
   Run in constant time (log2(<bitsize of x>))  
   Useful to run constant time checks
*/
#define HEAVISIDE_STEP_UINT64(x) {uint64_t _t; \
    _t=(((uint64_t)x>>32) | x); \
    _t=(0xFFFFFFFF + (_t & 0xFFFFFFFF)); \
    x=_t >> 32;}

#define HEAVISIDE_STEP_UINT32(x) {uint32_t _t; \
    _t=(((uint32_t)x>>16) | x); \
    _t=(0xFFFF + (_t & 0xFFFF)); \
    x=_t >> 16;}

#define HEAVISIDE_STEP_UINT16(x) {uint16_t _t; \
    _t=(((uint16_t)x>>8) | x); \
    _t=(0xFF + (_t & 0xFF)); \
    x=_t >> 8;}

#define HEAVISIDE_STEP_UINT8(x) {uint8_t _t; \
    _t=(((uint8_t)x>>4) | (uint8_t)x); \
    _t=((_t>>2) | _t); \
    _t=((_t>>1) | _t); \
    x=_t & 0x1;}

#define CC_HEAVISIDE_STEP(x) { \
    if (sizeof(x) == 1) {HEAVISIDE_STEP_UINT8(x);}  \
    else if (sizeof(x) == 2) {HEAVISIDE_STEP_UINT16(x);} \
    else if (sizeof(x) == 4) {HEAVISIDE_STEP_UINT32(x);} \
    else if (sizeof(x) == 8) {HEAVISIDE_STEP_UINT64(x);} \
    else {x=((x==0)?0:1);} \
    }

/* Return 1 if x mod 4 =1,2,3, 0 otherwise */
#define CC_CARRY_2BITS(x) (((x>>1) | x) & 0x1)
#define CC_CARRY_3BITS(x) (((x>>2) | (x>>1) | x) & 0x1)

/* Set a variable to the biggest power of 2 which can be represented */ 
#define MAX_POWER_OF_2(x)   ((__typeof__(x))1<<(8*sizeof(x)-1))

#define cc_ceiling(a,b)  (((a)+((b)-1))/(b))
#define CC_BITLEN_TO_BYTELEN(x) cc_ceiling((x), 8)

//cc_abort() is implemented to comply with FIPS 140-2. See radar 19129408
void cc_abort(const char * msg , ...);

#endif /* _CORECRYPTO_CC_PRIV_H_ */
