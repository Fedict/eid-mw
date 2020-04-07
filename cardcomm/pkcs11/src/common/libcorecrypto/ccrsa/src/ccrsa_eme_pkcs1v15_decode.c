/*
 * Copyright (c) 2011,2013,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccrsa_priv.h>

/*
 The s_size argument is really meant to be a size_t rather than a cc_size.  It's the size
 in bytes of the key for which this decoding is being done.  's' on the other hand is a
 cc_unit array large enough to contain the blocksize of the key.  We need to start the
 decoding "right justified" within s for s_size bytes.
 */

int ccrsa_eme_pkcs1v15_decode(size_t *r_size, uint8_t *r,
                              size_t s_size, cc_unit *s)

{
    ccn_swap(ccrsa_n_from_size(s_size), s);
    uint8_t *in = ccrsa_block_start(s_size, s, 0);
    size_t i, padlen, inlen = s_size;
    size_t j,outlen=*r_size;
    uint8_t ptr_increment=0;
    size_t  retval=0;

    // Expected structure is
    // 00:02:PS:00:Msg
    
    // -- Check for expected prefix 00:02
    retval |= (in[0] ^ 0x00);  // Error if != 0
    retval |= (in[1] ^ 0x02);  // Error if != 0
    
    // -- Walk through the PS until 00 separator is found
    // Once separator is found it copies the Msg bytes into r
    *r_size=0;
    j=2; //j is invariably = i+1, this is to save some +1 in the loop condition and loop core
    for(i=1; j<inlen && j<outlen;i++,j++)
    {
        // if in[j]!=0, in[j]+0xff >= 0x100 => MS byte is 1
        ptr_increment|=(1^((uint16_t)(in[i])+0xFF)>>8); // flag 1 iff in[i+1]==0;
        *r=in[j];     // The loop writes in r[0] until separator is found
        r+=ptr_increment;
        *r_size+=ptr_increment;
    }
    // Compute the padding length
    padlen=inlen-(*r_size)-3;  // can't be negative (because of j boundaries)
    
    // -- Check whether separator was found: error if ptr_increment==0
    retval |= (0x1 ^ ptr_increment);

    // -- Check (padlen < 8)
    // Use a power of 2 to prevent condition + rely on a specific underflow behavior:
    // Error when padlen>=2^n or if 2^n+7-padlen >= 2^n
    // n define a the biggest number so that 2^n fits in the representation
    retval |= ((padlen & MAX_POWER_OF_2(padlen)) \
            | (((MAX_POWER_OF_2(padlen)+7) - padlen) & MAX_POWER_OF_2(padlen)));
    
    // -- Convert retval to a value in {0,1} where 1 means error
    // use a constant time Heaviside Step function if (reval==0) => retval=0, retval=1 otherwise.
    CC_HEAVISIDE_STEP(retval);
    
    // Format output, presence of error does not matter.
    ccn_swap(ccrsa_n_from_size(s_size), s);
    
    // Error code.
    // In case of error, only r[0] was modified in r
    return -((int)retval); // retval is either 0 or 1
}
