# 
#  Copyright (c) 2015 Apple Inc. All rights reserved.
#  
#  corecrypto Internal Use License Agreement
#  
#  IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
#  in consideration of your agreement to the following terms, and your download or use
#  of this Apple software constitutes acceptance of these terms.  If you do not agree
#  with these terms, please do not download or use this Apple software.
#  
#  1.	As used in this Agreement, the term "Apple Software" collectively means and
#  includes all of the Apple corecrypto materials provided by Apple here, including
#  but not limited to the Apple corecrypto software, frameworks, libraries, documentation
#  and other Apple-created materials. In consideration of your agreement to abide by the
#  following terms, conditioned upon your compliance with these terms and subject to
#  these terms, Apple grants you, for a period of ninety (90) days from the date you
#  download the Apple Software, a limited, non-exclusive, non-sublicensable license
#  under Apple’s copyrights in the Apple Software to make a reasonable number of copies
#  of, compile, and run the Apple Software internally within your organization only on
#  devices and computers you own or control, for the sole purpose of verifying the
#  security characteristics and correct functioning of the Apple Software; provided
#  that you must retain this notice and the following text and disclaimers in all
#  copies of the Apple Software that you make. You may not, directly or indirectly,
#  redistribute the Apple Software or any portions thereof. The Apple Software is only
#  licensed and intended for use as expressly stated above and may not be used for other
#  purposes or in other contexts without Apple's prior written permission.  Except as
#  expressly stated in this notice, no other rights or licenses, express or implied, are
#  granted by Apple herein.
#  
#  2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
#  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
#  OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
#  THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
#  SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
#  REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
#  ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
#  SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
#  OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
#  WILL CREATE A WARRANTY. 
#  
#  3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
#  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
#  IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
#  SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
#  NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#  
#  4.	This Agreement is effective until terminated. Your rights under this Agreement will
#  terminate automatically without notice from Apple if you fail to comply with any term(s)
#  of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
#  and destroy all copies, full or partial, of the Apple Software. This Agreement will be
#  governed and construed in accordance with the laws of the State of California, without
#  regard to its choice of law rules.
#  
#  You may report security issues about Apple products to product-security@apple.com,
#  as described here:  https://www.apple.com/support/security/.  Non-security bugs and
#  enhancement requests can be made via https://bugreport.apple.com as described
#  here: https://developer.apple.com/bug-reporting/
#  
#  EA1350
#  10/5/15
# 

#ifndef __NO_ASM__

//  This file implements schoolbook multiplication implemented for arm64.
//  Pseudo-code in C:
//
//    cc_dunit mulAdd2(cc_unit a, cc_unit b, cc_unit c, cc_unit d) { return (cc_dunit)a*b + c + d; }
//    cc_unit hi(cc_dunit a) { return a >> CCN_UNIT_BITS; }
//    cc_unit lo(cc_dunit a) { return a; }
//
//    void ccn_mul(cc_size n, cc_unit *r, const cc_unit *s, const cc_unit *t) {
//      cc_unit c = 0;
//      for (cc_size j=0; j<n; ++j) {
//        cc_dunit p = mulAdd2(s[j], t[0], 0, c);
//        r[j] = lo(p);
//        c = hi(p);
//      }
//      r[n] = c;
//      for (cc_size i=1; i<n; ++i) {
//        c = 0;
//        for (cc_size j=0; j<n; ++j) {
//          cc_dunit p = mulAdd2(s[j], t[i], r[i+j], c);
//          r[i+j] = lo(p);
//          c = hi(p);
//        }
//        r[i+n] = c;
//      }
//    }

#include <corecrypto/cc_config.h>

#if defined __arm64__ && CCN_MUL_ASM

#define n  x0
#define r  x1
#define s  x2
#define t  x3
#define ti x4
#define sj x5
#define p  x6
#define q  x7
#define c  x8
#define j  x9
#define i  x10

.align 4
.globl _ccn_mul
_ccn_mul:
//  Establish a stack frame and early out if n is zero.
    stp     fp, lr, [sp, #-16]!
    mov     fp, sp
    cbz     n,      L_exit
//  Scale n by 8 to convert elements to bytes and negate so that we can use
//  negative indexing that counts up to zero, avoiding an extra compare in our
//  loops; advance r, s, and t to point to the nth element of the corresponding
//  buffers to be compatible with this scheme.
    neg     n,      n,  lsl #3
    sub     r,      r,  n
    sub     s,      s,  n
    sub     t,      t,  n
//  First multiplication loop; we compute the (n+1)-word product s[0...n]*t[0]
//  and store it to the low-order n+1 words of r.  Before the loop starts, we
//  need to set up a counter, zero out a c register (since there is no
//  c-in to the low partial product), and load t[0].
    mov     i,      n
    mov     j,      n
    mov     c,      xzr
    ldr     ti,    [t,i]
//  In the loop itself, we load a word from s, compute the full product with
//  t[0], add the high part of the previous multiplication, and store the
//  result to r.
L_sj_t0_product:
    ldr     sj,    [s,j]
    mul     p,      sj, ti
    umulh   q,      sj, ti
    adds    p,      p,  c
    adc     q,      q,  xzr
    str     p,     [r,j]
    mov     c,      q
    adds    j,      j,  #8
    b.ne    L_sj_t0_product
//  Store the remaining high word of the product we just computed; decrement
//  the outer-loop counter and exit if we have run out of words in t.
L_loop_over_i:
    str     c, [r]
    adds    i,      i,  #8
    b.eq    L_exit
//  More work remains to be done; load the next word of t, and add its product
//  with s to the accumulated buffer.  This code is essentially identical
//  to the s * t0 product above, except that we need to add the corresponding
//  already-computed word from the result buffer to each partial product.
    add     r,      r,  #8
    mov     j,      n
    mov     c,      xzr
    ldr     ti,    [t,i]
L_sj_ti_product:
//  Main work loop: compute the partial product p:q = s[j]*t[i] + r[i+j] + c.
//  The low part of the result (p) is stored back to r[i+j].  The high part
//  of the result (q) becomes c for the next iteration.
    ldr     sj,    [s,j]
    mul     p,      sj, ti
    umulh   q,      sj, ti
    adds    p,      p,  c
    adc     q,      q,  xzr
    ldr     c,     [r,j]
    adds    p,      p,  c
    adc     q,      q,  xzr
    str     p,     [r,j]
    mov     c,      q
    adds    j,      j,  #8
    b.ne    L_sj_ti_product
    b       L_loop_over_i
L_exit:
    ldp     fp, lr, [sp], #16
    ret     lr

#endif // defined __arm64__ && CCN_MUL_ASM
#endif // __NO_ASM__
