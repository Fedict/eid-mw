/*
 * Copyright (c) 2013,2014,2015 Apple Inc. All rights reserved.
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


#ifndef corecrypto_ccnistkdf_priv_h
#define corecrypto_ccnistkdf_priv_h

#include <corecrypto/ccdigest.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/cc.h>
#include <corecrypto/cc_priv.h>

CC_INLINE void ccdigest_update_uint32_t(const struct ccdigest_info *di, ccdigest_ctx_t ctx, uint32_t n) {
    uint32_t tmp;
    CC_STORE32_BE(n, &tmp);
    ccdigest_update(di, ctx, 4, &tmp);
}


#define cchmac_state_cache(_di_,_name_) cc_unit (_name_)[ccn_nof_size((_di_)->state_size)]
CC_INLINE void cchmac_cache_state(const struct ccdigest_info *di, cchmac_ctx_t hc, cc_unit *cache) {
    ccdigest_copy_state(di, cache, cchmac_istate32(di, hc));
}

CC_INLINE void cchmac_reset_from_cache(const struct ccdigest_info *di,
                                           cchmac_ctx_t hc,
                                           const cc_unit *cache) {
    ccdigest_copy_state(di, cchmac_istate32(di, hc), cache);
    cchmac_nbits(di, hc) = di->block_size * 8;
    cchmac_num(di, hc)=0;
}

CC_INLINE size_t cc_div_ceiling(size_t n, size_t div) {
    size_t retval = n / div;
    if((retval * div) < n) retval++;
    return retval;
}

#define DEBUG_DUMP 0
#if DEBUG_DUMP

static void cc_internal_print_fixed_data(uint8_t *fixedData, size_t labelLen, size_t contextLen) {
    size_t i;
    uint8_t *p = fixedData;
    
    printf("Fixed Data\nLabel: ");
    for(i = 0; i < labelLen; i++, p++) printf("%02x", *p);
    printf("\nZeroByte: %02x\n", *p);
    p++;
    printf("Context: ");
    for(i = 0; i < contextLen; i++, p++) printf("%02x", *p);
    printf("\nKeyLength: %02x%02x%02x%02x\n", *p, *(p+1), *(p+2), *(p+3));
}

#else

#define cc_internal_print_fixed_data(X,Y,Z) do { } while(0)

#endif

static void construct_fixed_data(size_t labelLen, const uint8_t *label,
                                 size_t contextLen, const uint8_t *context,
                                 size_t dkLen, uint8_t *fixedData) {
    // fixedData = Label || 0x00 || Context || dkLen*8 (bitlength of key) as 4 bytes
    if(labelLen > 0 && label != NULL) CC_MEMCPY(fixedData, label, labelLen);
    fixedData[labelLen] = 0;
    if(contextLen > 0 && context != NULL) CC_MEMCPY(fixedData + labelLen + 1, context, contextLen);
    CC_STORE32_BE((uint32_t)dkLen*8, fixedData + labelLen + contextLen + 1);
    cc_internal_print_fixed_data(fixedData, labelLen, contextLen);
}

#endif
