/*
 * Copyright (c) 2010,2012,2014,2015 Apple Inc. All rights reserved.
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


#include <stdint.h>
#include <string.h>
#include <corecrypto/ccdes.h>

static uint8_t weak_keys[][8] = {
    {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01}, /* weak keys */
    {0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE,0xFE},
    {0x1F,0x1F,0x1F,0x1F,0x0E,0x0E,0x0E,0x0E},
    {0xE0,0xE0,0xE0,0xE0,0xF1,0xF1,0xF1,0xF1},
    {0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE}, /* semi-weak keys */
    {0xFE,0x01,0xFE,0x01,0xFE,0x01,0xFE,0x01},
    {0x1F,0xE0,0x1F,0xE0,0x0E,0xF1,0x0E,0xF1},
    {0xE0,0x1F,0xE0,0x1F,0xF1,0x0E,0xF1,0x0E},
    {0x01,0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1},
    {0xE0,0x01,0xE0,0x01,0xF1,0x01,0xF1,0x01},
    {0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E,0xFE},
    {0xFE,0x1F,0xFE,0x1F,0xFE,0x0E,0xFE,0x0E},
    {0x01,0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E},
    {0x1F,0x01,0x1F,0x01,0x0E,0x01,0x0E,0x01},
    {0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1,0xFE},
    {0xFE,0xE0,0xFE,0xE0,0xFE,0xF1,0xFE,0xF1}
};


int ccdes_key_is_weak( void *key, unsigned long  length)
{
    unsigned long  n;

    if (length != 8)
        return -1;

    for (n = 0; n < sizeof(weak_keys) / sizeof(weak_keys[0]); n++)
        if (cc_cmp_safe(8, weak_keys[n], key) == 0)
            return -1;

    return 0;
}

static uint8_t odd_parity[256] = {
    1,  1,  2,  2,  4,  4,  7,  7,  8,  8, 11, 11, 13, 13, 14, 14,
    16, 16, 19, 19, 21, 21, 22, 22, 25, 25, 26, 26, 28, 28, 31, 31,
    32, 32, 35, 35, 37, 37, 38, 38, 41, 41, 42, 42, 44, 44, 47, 47,
    49, 49, 50, 50, 52, 52, 55, 55, 56, 56, 59, 59, 61, 61, 62, 62,
    64, 64, 67, 67, 69, 69, 70, 70, 73, 73, 74, 74, 76, 76, 79, 79,
    81, 81, 82, 82, 84, 84, 87, 87, 88, 88, 91, 91, 93, 93, 94, 94,
    97, 97, 98, 98,100,100,103,103,104,104,107,107,109,109,110,110,
    112,112,115,115,117,117,118,118,121,121,122,122,124,124,127,127,
    128,128,131,131,133,133,134,134,137,137,138,138,140,140,143,143,
    145,145,146,146,148,148,151,151,152,152,155,155,157,157,158,158,
    161,161,162,162,164,164,167,167,168,168,171,171,173,173,174,174,
    176,176,179,179,181,181,182,182,185,185,186,186,188,188,191,191,
    193,193,194,194,196,196,199,199,200,200,203,203,205,205,206,206,
    208,208,211,211,213,213,214,214,217,217,218,218,220,220,223,223,
    224,224,227,227,229,229,230,230,233,233,234,234,236,236,239,239,
    241,241,242,242,244,244,247,247,248,248,251,251,253,253,254,254,
};

void ccdes_key_set_odd_parity(void *key, unsigned long length)
{
    uint8_t *p = key;
    unsigned long  n;

    for (n = 0; n < length; n++)
        p[n] = odd_parity[p[n]];

}


