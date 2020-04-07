/*
 * Copyright (c) 2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccpad.h>
#include <corecrypto/cc_priv.h>
#include <stdbool.h>

#define DECODE_MINIMAL                  1
#define DECODE_NOT_CONSTANT_TIME        2
#define FULL_DECODE_CONSTANT_TIME       3
#define LAST_BYTE_DECODE_CONSTANT_TIME  4

#define DECODE_SELECT LAST_BYTE_DECODE_CONSTANT_TIME

/* Constant time pkcs7 padding check. */
size_t ccpad_pkcs7_decode(const size_t block_size, const uint8_t* last_block) {
    size_t pad = last_block[block_size - 1];
#if (DECODE_SELECT == LAST_BYTE_DECODE_CONSTANT_TIME)
    /* RECOMMENDED: This processes the last byte only so that leaks are harmless.
     It provides the correct info if the decryption was correct.
     Although this may seem incomplete, it provides good protection against timing attacks since behavior
     based on incorrect length at the application level only leaks info about the last byte.

     A partial check is ok because a padding check (or associate length) is NOT a good way to verify 
     that decryption is correct.
     
    */
    size_t default_size=0; // Value returned in case of incorrect padding
    size_t failed=0;
    failed=(0xff& (~block_size))+pad;  // MSByte>0 iff pad>block_size
    failed|=(0x100-pad);  // MSByte>0 iff pad>block_size
    failed=(0x100-(failed>>8));          // => 0xff iff failed==1
    return ((~failed) & pad) + ((failed) & default_size);
#elif (DECODE_SELECT==FULL_DECODE_CONSTANT_TIME)
#warning "Not recommended: behavior of applicaton can still leak padding info"
    /* Actual time constant version 
    Behaves the same as the legacy version below */
    unsigned char default_size=block_size; // Value returned in case of incorrect padding
    uint16_t mask;
    uint16_t counting=0x100;
    uint16_t failed=0;
    size_t pad_counter=0x100;

    for(size_t i=block_size-1; i>0;i--)
    {
        uint16_t flag=(pad^last_block[i - 1]); // 0 if ok
        flag=(0x100^(flag+0xFF)); // flag 1 iff pad==0;
        counting&=flag;
        pad_counter+= (flag & counting);
    }
    failed|=(0xFF+(pad^(pad_counter>>8))); // Failed flag is the MSB
    mask=(0xff*(failed>>8));        // => 0xff iff failed==1
    return ((~mask) & pad) + ((mask) & default_size);
#elif (DECODE_SELECT==DECODE_NOT_CONSTANT_TIME)
#warning "Not recommended: not constant time"
    /* Not constant time pkcs7 padding check. 
     Kept here to play with the ccpad constant time detection algorithm
     */
    bool failed = pad > block_size;
    failed |= pad == 0;
    for (size_t ix = 2; ix <= block_size; ++ix) {
        failed |= ((ix > pad)
                   ? false
                   : last_block[block_size - ix] != pad);
    }
    /* To be safe we remove the entire offending block if the pkcs7 padding
     checks failed.  However we purposely don't report the failure to decode
     the padding since any use of this error leads to potential security
     exploits. */
    return failed ? block_size : pad;
#elif (DECODE_SELECT==DECODE_MINIMAL)
#warning "Not recommended: just a stub to have the correct behavior, not to be used"
    /* Time constant implementation
     If padding is not correct, it returns an arbitrary length in [1..block_size]
     Kept here as a reference for time constant algorithm */
    return ((pad-1) & (block_size-1))+1;
#else
#error "Invalid value of DECODE_SELECT"
#endif

}
