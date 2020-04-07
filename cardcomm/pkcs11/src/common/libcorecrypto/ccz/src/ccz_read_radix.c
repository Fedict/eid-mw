/*
 * Copyright (c) 2012,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccz_priv.h>

#include <ctype.h>  /* For toupper(). */

static int ccn_digit_for_radix(cc_unit *r, char ch, unsigned radix) {
    *r=0; // Default value in case of error
    ch = ((radix < 36) ? (char)toupper((int)ch) : ch);
    for (size_t i = 0; i < radix; ++i) {
        if (ch == ccn_radix_digit_map[i]) {
            *r=i;
            return 0;
        }
    }
    // Not found
    return CCZ_INVALID_INPUT_ERROR;
}

static size_t ccn_read_radix_size(size_t data_size, unsigned radix) {
    cc_unit rad = radix - 1;
    return ccn_nof(ccn_bitlen(1, &rad) * data_size);
}

static int ccn_read_radix(cc_size n, cc_unit *r, size_t data_size,
                    const char *data, unsigned radix) {
    ccn_zero(n, r);
    cc_unit v;
    int rv=0;
    /* TODO: Performance win start n = 0 and increment in the loop as needed. */
    for (const char *end = data + data_size; data != end; ++data) {
        ccn_mul1(n, r, r, radix);
        rv |= ccn_digit_for_radix(&v, *data, radix); // v is zero in case of error
        ccn_add1(n, r, r, v);
    }
    return rv;
}

int ccz_read_radix(ccz *r, size_t data_size, const char *data, unsigned radix) {
    int rv;
    int sign=1;

    if ((radix==0) || (radix>strlen(ccn_radix_digit_map))) {
        return CCZ_INVALID_RADIX_ERROR; // Radix not supported
    }

    // Sign
    if (data_size) {
        if (*data == '-') {
            ++data;
            --data_size;
            sign=-1;
        }
        if (*data == '+') {
            ++data;
            --data_size;
            sign=1;
        }
    }

    // Absolute value
    cc_size n = ccn_read_radix_size(data_size, radix);
    ccz_set_capacity(r, n);
    rv=ccn_read_radix(n, r->u, data_size, data, radix);
    ccz_set_n(r, ccn_n(n, r->u));
    ccz_set_sign(r, sign);
    return rv;
}
