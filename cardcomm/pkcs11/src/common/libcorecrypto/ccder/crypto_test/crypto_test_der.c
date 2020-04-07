/*
 * Copyright (c) 2015 Apple Inc. All rights reserved.
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


#include "testmore.h"
#include <corecrypto/ccder.h>

#if (CCDER == 0)
entryPoint(ccder, "ccder")
#else
#include "crypto_test_der.h"

static void testSizeOf()
{
    is(ccder_sizeof(CCDER_EOL, 0), (size_t)2, "EOL");
    is(ccder_sizeof(CCDER_BOOLEAN, 0), (size_t)2, "BOOLEAN");
    is(ccder_sizeof(CCDER_INTEGER, 0), (size_t)2, "INTEGER");
    is(ccder_sizeof(CCDER_BIT_STRING, 0), (size_t)2, "BIT_STRING");
    is(ccder_sizeof(CCDER_OCTET_STRING, 0), (size_t)2, "OCTET_STRING");
    is(ccder_sizeof(CCDER_NULL, 0), (size_t)2, "NULL");
    is(ccder_sizeof(CCDER_OBJECT_IDENTIFIER, 0), (size_t)2, "OBJECT_IDENTIFIER");
    is(ccder_sizeof(CCDER_REAL, 0), (size_t)2, "REAL");
    is(ccder_sizeof(CCDER_ENUMERATED, 0), (size_t)2, "ENUMERATED");
    is(ccder_sizeof(CCDER_EMBEDDED_PDV, 0), (size_t)2, "EMBEDDED_PDV");
    is(ccder_sizeof(CCDER_UTF8_STRING, 0), (size_t)2, "UTF8_STRING");
    is(ccder_sizeof(CCDER_CONSTRUCTED_SEQUENCE, 0), (size_t)2, "CONSTRUCTED_SEQUENCE");
    is(ccder_sizeof(CCDER_CONSTRUCTED_SET, 0), (size_t)2, "CONSTRUCTED_SET");
    is(ccder_sizeof(CCDER_NUMERIC_STRING, 0), (size_t)2, "NUMERIC_STRING");
    is(ccder_sizeof(CCDER_PRINTABLE_STRING, 0), (size_t)2, "PRINTABLE_STRING");
    is(ccder_sizeof(CCDER_T61_STRING, 0), (size_t)2, "T61_STRING");
    is(ccder_sizeof(CCDER_VIDEOTEX_STRING, 0), (size_t)2, "VIDEOTEX_STRING");
    is(ccder_sizeof(CCDER_IA5_STRING, 0), (size_t)2, "IA5_STRING");
    is(ccder_sizeof(CCDER_UTC_TIME, 0), (size_t)2, "UTC_TIME");
    is(ccder_sizeof(CCDER_GENERALIZED_TIME, 0), (size_t)2, "GENERALIZED_TIME");
    is(ccder_sizeof(CCDER_GRAPHIC_STRING, 0), (size_t)2, "GRAPHIC_STRING");
    is(ccder_sizeof(CCDER_VISIBLE_STRING, 0), (size_t)2, "VISIBLE_STRING");
    is(ccder_sizeof(CCDER_GENERAL_STRING, 0), (size_t)2, "GENERAL_STRING");
    is(ccder_sizeof(CCDER_UNIVERSAL_STRING, 0), (size_t)2, "UNIVERSAL_STRING");
    is(ccder_sizeof(CCDER_BMP_STRING, 0), (size_t)2, "BMP_STRING");
    is(ccder_sizeof(CCDER_HIGH_TAG_NUMBER, 0), (size_t)3, "HIGH_TAG_NUMBER");
    is(ccder_sizeof(0x1f, 0), (size_t)3, "[31]");
    is(ccder_sizeof(0x20, 0), (size_t)3, "[32]");
    is(ccder_sizeof(0x7f, 0), (size_t)3, "[127]");
    is(ccder_sizeof(0x80, 0), (size_t)4, "[128]");
    is(ccder_sizeof(0x3fff, 0), (size_t)4, "[4095]");
    is(ccder_sizeof(0x4000, 0), (size_t)5, "[4096]");
    is(ccder_sizeof(0x1fffff, 0), (size_t)5, "[2097151]");
    is(ccder_sizeof(0x200000, 0), (size_t)6, "[2097152]");

    is(ccder_sizeof(CCDER_OCTET_STRING, 1), (size_t)3, "OCTET_STRING(1)");
    is(ccder_sizeof(CCDER_OCTET_STRING, 127), (size_t)129, "OCTET_STRING(127)");
    is(ccder_sizeof(CCDER_OCTET_STRING, 128), (size_t)131, "OCTET_STRING(128)");
    is(ccder_sizeof(CCDER_OCTET_STRING, 128), (size_t)131, "OCTET_STRING(129)");
}

static void testSizeOfUInt64()
{
    is(ccder_sizeof_uint64(0), (size_t)3, "uint64(0)");
    is(ccder_sizeof_uint64(1), (size_t)3, "uint64(1)");
    is(ccder_sizeof_uint64(0x7f), (size_t)3, "uint64(0x7f)");
    is(ccder_sizeof_uint64(0x80), (size_t)4, "uint64(0x80)");
    is(ccder_sizeof_uint64(0x100), (size_t)4, "uint64(0x100)");
    is(ccder_sizeof_uint64(0x7fff), (size_t)4, "uint64(0x7fff)");
    is(ccder_sizeof_uint64(0x8000), (size_t)5, "uint64(0x8000)");
    is(ccder_sizeof_uint64(0x7fffff), (size_t)5, "uint64(0x7fffff)");
    is(ccder_sizeof_uint64(0x800000), (size_t)6, "uint64(0x800000)");
    is(ccder_sizeof_uint64(0x7fffffff), (size_t)6, "uint64(0x7fffffff)");
    is(ccder_sizeof_uint64(0x80000000), (size_t)7, "uint64(0x80000000)");
    is(ccder_sizeof_uint64(0x7fffffffff), (size_t)7, "uint64(0x7fffffffff)");
    is(ccder_sizeof_uint64(0x8000000000), (size_t)8, "uint64(0x8000000000)");
    is(ccder_sizeof_uint64(0x7fffffffffff), (size_t)8, "uint64(0x7fffffffffff)");
    is(ccder_sizeof_uint64(0x800000000000), (size_t)9, "uint64(0x800000000000)");
    is(ccder_sizeof_uint64(0x7fffffffffffff), (size_t)9, "uint64(0x7fffffffffffff)");
    is(ccder_sizeof_uint64(0x80000000000000), (size_t)10, "uint64(0x80000000000000)");
    is(ccder_sizeof_uint64(0x7fffffffffffffff), (size_t)10, "uint64(0x7fffffffffffffff)");
}

static int testEncodeLen(void)
{
    uint8_t tmp[5];

    // 1 byte
    memset(tmp,0,sizeof(tmp));
    const uint8_t expected_result1[5]={0};
    is(ccder_encode_len(0,(const uint8_t*)&tmp[0],&tmp[1]),&tmp[0],"ccder_encode_len return value for 1byte length");
    ok_memcmp_or_fail(tmp, expected_result1,sizeof(tmp),"ccder_encode_len output for 1byte length");

    // 2 bytes
    memset(tmp,0,sizeof(tmp));
    const uint8_t expected_result2[5]={0x81,0x80};
    is(ccder_encode_len(0x80,(const uint8_t*)&tmp[0],&tmp[2]),&tmp[0],"ccder_encode_len return value for 2byte length");
    ok_memcmp_or_fail(tmp, expected_result2,sizeof(tmp),"ccder_encode_len output for 2byte length");

    // 3 bytes
    memset(tmp,0,sizeof(tmp));
    const uint8_t expected_result3[5]={0x82,0xFF,0xFE};
    is(ccder_encode_len(0xFFFE,(const uint8_t*)&tmp[0],&tmp[3]),&tmp[0],"ccder_encode_len return value for 3byte length");
    ok_memcmp_or_fail(tmp, expected_result3,sizeof(tmp),"ccder_encode_len output for 3byte length");

    // 4 bytes
    memset(tmp,0,sizeof(tmp));
    const uint8_t expected_result4[5]={0x83,0xFF,0xFE,0xFD};
    is(ccder_encode_len(0xFFFEFD,(const uint8_t*)&tmp[0],&tmp[4]),&tmp[0],"ccder_encode_len return value for 4byte length");
    ok_memcmp_or_fail(tmp, expected_result4,sizeof(tmp),"ccder_encode_len output for 4byte length");

    // 5 bytes
    memset(tmp,0,sizeof(tmp));
    const uint8_t expected_result5[5]={0x84,0xFF,0xFE,0xFD,0xFC};
    is(ccder_encode_len(0xFFFEFDFC,(const uint8_t*)&tmp[0],&tmp[5]),&tmp[0],"ccder_encode_len return value for 5byte length");
    ok_memcmp_or_fail(tmp, expected_result5,sizeof(tmp),"ccder_encode_len output for 5byte length");

    if (sizeof(size_t)>4) {
        // 5 bytes
        is(ccder_encode_len((size_t)1<<33,&tmp[0],NULL),NULL, "length bigger than UINT32_MAX not supported"); // Expect error
    } else {
        pass("On 32bit platforms, the length can't exceed UINT32_MAX");
    }
    return 0;
}
int ccder(TM_UNUSED int argc, TM_UNUSED char *const *argv)
{
    plan_tests(67);
    testSizeOfUInt64();
    testSizeOf();
    testEncodeLen();
    
    return 0;
}

#endif //entryPoint(ccder,"ccder")

