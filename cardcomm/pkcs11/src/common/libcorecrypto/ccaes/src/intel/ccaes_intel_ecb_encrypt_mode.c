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

#ifndef __NO_ASM__

#include <corecrypto/ccmode.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/cc_config.h>

#if CCAES_INTEL

static void vng_aesni_encrypt_blocks(const char *key, const char *in, char *out, int Nr, int nblocks)
{
        if (Nr == 160) {

            __asm__ volatile(

			"sub    $4, %[Nb]\n\t"
			"jl		1f\n\t"

            "movups (%[key]), %%xmm4\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups 16(%[pt]), %%xmm1\n\t"
            "movups 32(%[pt]), %%xmm2\n\t"
            "movups 48(%[pt]), %%xmm3\n\t"

            "pxor   %%xmm4, %%xmm0\n\t"
            "pxor   %%xmm4, %%xmm1\n\t"
            "pxor   %%xmm4, %%xmm2\n\t"
            "pxor   %%xmm4, %%xmm3\n\t"
            "movups 32(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 48(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 64(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 80(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 96(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 112(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 128(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 144(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 160(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "aesenclast %%xmm4, %%xmm0\n\t"
            "aesenclast %%xmm4, %%xmm1\n\t"
            "aesenclast %%xmm4, %%xmm2\n\t"
            "aesenclast %%xmm4, %%xmm3\n\t"
            "movups 0(%[key]), %%xmm4\n\t"

            "movups %%xmm0, (%[ct])\n\t"
            "movups %%xmm1, 16(%[ct])\n\t"
            "movups %%xmm2, 32(%[ct])\n\t"
            "movups %%xmm3, 48(%[ct])\n\t"

            "add    $64, %[ct]\n\t"
            "add    $64, %[pt]\n\t"
            "sub    $4, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"

            "add    $4, %[Nb]\n\t"
			"jle	1f\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups (%[key]), %%xmm1\n\t"
            "pxor   %%xmm1, %%xmm0\n\t"
            "movups 16(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 32(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 48(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 64(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 80(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 96(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 112(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 128(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 144(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 160(%[key]), %%xmm1\n\t"
            "aesenclast %%xmm1, %%xmm0\n\t"
            "movups %%xmm0, (%[ct])\n\t"

            "add    $16, %[ct]\n\t"
            "add    $16, %[pt]\n\t"
            "sub    $1, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"


            :
            : [ct] "r" (out), [pt] "r" (in), [key] "r" (key), [Nb] "r" (nblocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
            );

        } else if (Nr == 192) {

            __asm__ volatile(

			"sub    $4, %[Nb]\n\t"
			"jl		1f\n\t"

            "movups (%[key]), %%xmm4\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups 16(%[pt]), %%xmm1\n\t"
            "movups 32(%[pt]), %%xmm2\n\t"
            "movups 48(%[pt]), %%xmm3\n\t"

            "pxor   %%xmm4, %%xmm0\n\t"
            "pxor   %%xmm4, %%xmm1\n\t"
            "pxor   %%xmm4, %%xmm2\n\t"
            "pxor   %%xmm4, %%xmm3\n\t"
            "movups 32(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 48(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 64(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 80(%[key]), %%xmm5\n\t"


            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 96(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 112(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 128(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 144(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 160(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 176(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 192(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "aesenclast %%xmm4, %%xmm0\n\t"
            "aesenclast %%xmm4, %%xmm1\n\t"
            "aesenclast %%xmm4, %%xmm2\n\t"
            "aesenclast %%xmm4, %%xmm3\n\t"
            "movups 0(%[key]), %%xmm4\n\t"

            "movups %%xmm0, (%[ct])\n\t"
            "movups %%xmm1, 16(%[ct])\n\t"
            "movups %%xmm2, 32(%[ct])\n\t"
            "movups %%xmm3, 48(%[ct])\n\t"

            "add    $64, %[ct]\n\t"
            "add    $64, %[pt]\n\t"
            "sub    $4, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"

            "add    $4, %[Nb]\n\t"
			"jle	1f\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups (%[key]), %%xmm1\n\t"
            "pxor   %%xmm1, %%xmm0\n\t"
            "movups 16(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 32(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 48(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 64(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 80(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 96(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 112(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 128(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 144(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 160(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 176(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 192(%[key]), %%xmm1\n\t"
            "aesenclast %%xmm1, %%xmm0\n\t"
            "movups %%xmm0, (%[ct])\n\t"

            "add    $16, %[ct]\n\t"
            "add    $16, %[pt]\n\t"
            "sub    $1, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"


            :
            : [ct] "r" (out), [pt] "r" (in), [key] "r" (key), [Nb] "r" (nblocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
            );

        } else {

            __asm__ volatile(

			"sub    $4, %[Nb]\n\t"
			"jl		1f\n\t"

            "movups (%[key]), %%xmm4\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups 16(%[pt]), %%xmm1\n\t"
            "movups 32(%[pt]), %%xmm2\n\t"
            "movups 48(%[pt]), %%xmm3\n\t"

            "pxor   %%xmm4, %%xmm0\n\t"
            "pxor   %%xmm4, %%xmm1\n\t"
            "pxor   %%xmm4, %%xmm2\n\t"
            "pxor   %%xmm4, %%xmm3\n\t"
            "movups 32(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 48(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 64(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 80(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 96(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 112(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 128(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 144(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 160(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 176(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 192(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 208(%[key]), %%xmm5\n\t"

            "aesenc %%xmm4, %%xmm0\n\t"
            "aesenc %%xmm4, %%xmm1\n\t"
            "aesenc %%xmm4, %%xmm2\n\t"
            "aesenc %%xmm4, %%xmm3\n\t"
            "movups 224(%[key]), %%xmm4\n\t"

            "aesenc %%xmm5, %%xmm0\n\t"
            "aesenc %%xmm5, %%xmm1\n\t"
            "aesenc %%xmm5, %%xmm2\n\t"
            "aesenc %%xmm5, %%xmm3\n\t"
            "movups 16(%[key]), %%xmm5\n\t"

            "aesenclast %%xmm4, %%xmm0\n\t"
            "aesenclast %%xmm4, %%xmm1\n\t"
            "aesenclast %%xmm4, %%xmm2\n\t"
            "aesenclast %%xmm4, %%xmm3\n\t"
            "movups 0(%[key]), %%xmm4\n\t"

            "movups %%xmm0, (%[ct])\n\t"
            "movups %%xmm1, 16(%[ct])\n\t"
            "movups %%xmm2, 32(%[ct])\n\t"
            "movups %%xmm3, 48(%[ct])\n\t"

            "add    $64, %[ct]\n\t"
            "add    $64, %[pt]\n\t"
            "sub    $4, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"

            "add    $4, %[Nb]\n\t"
			"jle	1f\n\t"

            "0:\n\t"

            "movups (%[pt]), %%xmm0\n\t"
            "movups (%[key]), %%xmm1\n\t"
            "pxor   %%xmm1, %%xmm0\n\t"
            "movups 16(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 32(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 48(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 64(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 80(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 96(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 112(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 128(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 144(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 160(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 176(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 192(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 208(%[key]), %%xmm1\n\t"
            "aesenc %%xmm1, %%xmm0\n\t"
            "movups 224(%[key]), %%xmm1\n\t"
            "aesenclast %%xmm1, %%xmm0\n\t"
            "movups %%xmm0, (%[ct])\n\t"

            "add    $16, %[ct]\n\t"
            "add    $16, %[pt]\n\t"
            "sub    $1, %[Nb]\n\t"
            "jg     0b\n\t"

            "1:\n\t"

            :
            : [ct] "r" (out), [pt] "r" (in), [key] "r" (key), [Nb] "r" (nblocks)
            : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5"
            );


        }
}

#include "vng_aesPriv.h"
#define ECB_CTX_SIZE sizeof(vng_aes_encrypt_ctx) 		/* The size of the context */


/* ==========================================================================
	VNG Optimized AES implementation.  This implementation is optimized but
	does not use the AESNI instructions
   ========================================================================== */

/* Initialize a context with the key */
static void init_wrapper_opt(const struct ccmode_ecb *ecb CC_UNUSED, ccecb_ctx *key,
                         unsigned long rawkey_len, const void *rawkey)
{
	vng_aes_encrypt_opt_key((const unsigned char *)rawkey, (int) rawkey_len, (vng_aes_encrypt_ctx*) key);	
}

/* cbc encrypt or decrypt nblocks from in to out. */
static void ecb_wrapper_opt(const ccecb_ctx *key, unsigned long nblocks, const void *in,
                        void *out)
{
	while (nblocks--) 
	{
		vng_aes_encrypt_opt((const unsigned char*)in, (unsigned char *) out, (const vng_aes_encrypt_ctx*) key);
		in += CCAES_BLOCK_SIZE;
		out += CCAES_BLOCK_SIZE;
	}
}

const struct ccmode_ecb ccaes_intel_ecb_encrypt_opt_mode = {
    .size = ECB_CTX_SIZE,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_opt,
    .ecb = ecb_wrapper_opt,
};

/* ==========================================================================
	VNG AESNI implementation.  This implementation uses the AESNI 
	instructions
   ========================================================================== */

/* Initialize a context with the key */
static void init_wrapper_aesni(const struct ccmode_ecb *ecb CC_UNUSED, ccecb_ctx *key,
                         unsigned long rawkey_len, const void *rawkey)
{
	vng_aes_encrypt_aesni_key((const unsigned char *)rawkey, (int) rawkey_len, (vng_aes_encrypt_ctx*) key);	
}

static void ecb_wrapper_aesni(const ccecb_ctx *key, unsigned long nblocks, const void *in,
                        void *out)
{
	const vng_aes_encrypt_ctx *ctx = (const vng_aes_encrypt_ctx *) key;

	vng_aesni_encrypt_blocks((const char *) ctx->ks, (const char *) in, (char *) out, (int) ctx->rn, (int) nblocks);

	return;
}

const struct ccmode_ecb ccaes_intel_ecb_encrypt_aesni_mode = {
    .size = ECB_CTX_SIZE,
    .block_size = CCAES_BLOCK_SIZE,
    .init = init_wrapper_aesni,
    .ecb = ecb_wrapper_aesni,
};

#endif /* CCAES_INTEL */
#endif /* __NO_ASM__ */
