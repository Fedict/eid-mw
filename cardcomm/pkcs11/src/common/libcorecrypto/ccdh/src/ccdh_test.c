/*
 * Copyright (c) 2011,2014,2015 Apple Inc. All rights reserved.
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


#include <corecrypto/ccdh.h>
#include <corecrypto/cctest.h>


int ccdh_test_compute_vector(const struct ccdh_compute_vector *v)
{
    int result,r1,r2;
    const cc_size n = ccn_nof(v->len);
    const size_t s = ccn_sizeof_n(n);
    unsigned char z[v->zLen];
    size_t zLen;
    unsigned char tmp[v->zLen]; // for negative testing
    uint32_t status=0;
    uint32_t nb_test=0;

    ccdh_gp_decl(s, gp);
    ccdh_full_ctx_decl(s, a);
    ccdh_full_ctx_decl(s, b);
    cc_unit p[n];
    cc_unit g[n];
    cc_unit r[n];
    cc_unit q[n];

    // Bail to errOut when unexpected error happens.
    // Try all usecases otherwise

    if((result=ccn_read_uint(n, p, v->pLen, v->p)))
        goto errOut;
    if((result=ccn_read_uint(n, g, v->gLen, v->g)))
        goto errOut;
    if((result=ccn_read_uint(n, q, v->qLen, v->q)))
        goto errOut;

    ccdh_init_gp_with_order(gp, n, p, g, q);

    ccdh_ctx_init(gp, a);
    ccdh_ctx_init(gp, b);

    if((result=ccn_read_uint(n, ccdh_ctx_x(a), v->xaLen, v->xa))) // private key
        goto errOut;
    if((result=ccn_read_uint(n, ccdh_ctx_y(a), v->yaLen, v->ya))) // public key
        goto errOut;
    if((result=ccn_read_uint(n, ccdh_ctx_x(b), v->xbLen, v->xb))) // private key
        goto errOut;
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), v->ybLen, v->yb))) // public key
        goto errOut;

    /*
     * Main test
     */

    /* try one side */
    zLen = v->zLen;
    r1=ccdh_compute_key(a, b, r);
    ccn_write_uint_padded(n, r, zLen, z);
    r1|=memcmp(z, v->z, zLen);

    /* try the other side */
    zLen = v->zLen;
    r2=ccdh_compute_key(b, a, r);
    ccn_write_uint_padded(n, r, zLen, z);
    r2|=memcmp(z, v->z, zLen);

    if ((!(r1||r2) && v->valid)||((r1||r2) && !v->valid))
    {
        status|=1<<nb_test;
    }
    nb_test++;

    // We are done if the test is not valid
    if (!v->valid) goto doneOut;

    /*
     * Corner case / negative testing
     * Only applicable for valid tests
     */

    /* Output is 1 (use private key is (p-1)/2)*/
    if((result=ccn_read_uint(n, ccdh_ctx_x(a), v->pLen, v->p))) // private key
        goto errOut;
    ccn_sub1(n,ccdh_ctx_x(a),ccdh_ctx_x(a),1);
    ccn_shift_right(n,ccdh_ctx_x(a),ccdh_ctx_x(a),1);
    if ((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    if((result=ccn_read_uint(n, ccdh_ctx_x(a), v->xaLen, v->xa))) // restore private key
        goto errOut;
    nb_test++;
    
    
    /* negative testing (1 < y < p-1)*/
    /* public y = 0 */
    zLen = v->zLen;
    cc_zero(sizeof(tmp),tmp);
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), zLen, tmp)))
    {
        goto errOut;
    }
    if((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* public y = 1 */
    zLen = v->zLen;
    cc_zero(sizeof(tmp),tmp);
    tmp[zLen-1]=1;
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), zLen, tmp)))
    {
        goto errOut;
    }
    if((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* public y = p */
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), v->pLen, v->p)))
        goto errOut;

    if((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* public y = p-1 */
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), v->pLen, v->p)))
    {
        goto errOut;
    }
    ccn_sub1(n,ccdh_ctx_y(b),ccdh_ctx_y(b),1);

    if((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* 
     * When the order is in defined in the group 
     *  check that the implementation check the order of the public value:
     *      public y = g+1 (for rfc5114 groups, g+1 is not of order q)
     */
    if (ccdh_gp_order_bitlen(gp))
    {
        if((result=ccn_read_uint(n, ccdh_ctx_y(b), v->gLen, v->g)))
        {
            goto errOut;
        }
        ccn_add1(n,ccdh_ctx_y(b),ccdh_ctx_y(b),1);

        if((result=ccdh_compute_key(a, b, r))!=0)
        {
            status|=1<<nb_test;
        }
        nb_test++;
    }


    /* positive testing at the boundaries of (1 < y < p-1)*/

    // Don't set the order in gp because 2 and p-2 are not of order q
    ccdh_init_gp(gp, n, p, g, 0);

    /* public y = 2 */
    zLen = v->zLen;
    cc_zero(sizeof(tmp),tmp);
    tmp[zLen-1]=2;
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), zLen, tmp)))
    {
        goto errOut;
    }
    if((result=ccdh_compute_key(a, b, r))==0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* public y = p-2 */
    if((result=ccn_read_uint(n, ccdh_ctx_y(b), v->pLen, v->p)))
    {
        goto errOut;
    }
    ccn_sub1(n,ccdh_ctx_y(b),ccdh_ctx_y(b),2);

    if((result=ccdh_compute_key(a, b, r))==0)
    {
        status|=1<<nb_test;
    }
    nb_test++;

    /* Negative testing: p is even */
    if((result=ccn_read_uint(n, p, v->pLen, v->p)))
        goto errOut;
    ccn_set_bit(p,0,0); // Set LS bit to 0
    ccdh_init_gp(gp, n, p, g, 0);
    ccdh_ctx_init(gp, a);
    ccdh_ctx_init(gp, b);

    if((result=ccdh_compute_key(a, b, r))!=0)
    {
        status|=1<<nb_test;
    }
    nb_test++;


    /* Test aftermath */
doneOut:
    if ((nb_test==0) || (status!=((1<<nb_test)-1)))
    {
        result=1;
    }
    else
    {
        result=0; // Test is successful, Yeah!
    }

errOut:
    return result;
}

