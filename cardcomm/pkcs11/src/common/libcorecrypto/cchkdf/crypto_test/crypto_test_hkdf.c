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


#define USE_SUPER_COOL_NEW_CCOID_T
#include "testmore.h"
#include "testbyteBuffer.h"
#include "testccnBuffer.h"

#if (CCHKDFTEST == 0)
entryPoint(cchkdf_test,"cchkdf test")
#else
#include <corecrypto/ccdigest.h>
#include <corecrypto/cchkdf.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>

static int verbose = 0;

#define type_sha1		1
#define type_sha256		256
#define type_sha512		512

typedef struct {
	int					type;
	const char *		ikm;
	const char *		salt;
	const char *		info;
	size_t				len;
	const char *		okm;
} test_vector_t;

static const test_vector_t	hkdf_sha256_tests[] = {
	// RFC 5869 Test Case 1
	{
	/* Type */	type_sha256, 
	/* IKM */	"0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", 
	/* Salt */	"000102030405060708090a0b0c", 
	/* Info */	"f0f1f2f3f4f5f6f7f8f9", 
	/* Len */	42, 
	/* OKM */	"3cb25f25faacd57a90434f64d0362f2a"
				"2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
				"34007208d5b887185865"
	}, 
	// RFC 5869 Test Case 2
	{
	/* Type */	type_sha256, 
	/* IKM */	"000102030405060708090a0b0c0d0e0f"
				"101112131415161718191a1b1c1d1e1f"
				"202122232425262728292a2b2c2d2e2f"
				"303132333435363738393a3b3c3d3e3f"
				"404142434445464748494a4b4c4d4e4f", 
	/* Salt */	"606162636465666768696a6b6c6d6e6f"
				"707172737475767778797a7b7c7d7e7f"
				"808182838485868788898a8b8c8d8e8f"
				"909192939495969798999a9b9c9d9e9f"
				"a0a1a2a3a4a5a6a7a8a9aaabacadaeaf", 
	/* Info */	"b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
				"c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
				"d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
				"e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
				"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff", 
	/* Len */	82, 
	/* OKM */	"b11e398dc80327a1c8e7f78c596a4934"
				"4f012eda2d4efad8a050cc4c19afa97c"
				"59045a99cac7827271cb41c65e590e09"
				"da3275600c2f09b8367793a9aca3db71"
				"cc30c58179ec3e87c14c01d5c1f3434f"
				"1d87"
	},
	// RFC 5869 Test Case 3
	{
	/* Type */	type_sha256, 
	/* IKM */	"0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", 
	/* Salt */	"", 
	/* Info */	"", 
	/* Len */	42, 
	/* OKM */	"8da4e775a563c18f715f802a063c5a31"
				"b8a11f5c5ee1879ec3454e5f3c738d2d"
				"9d201395faa4b61a96c8"
	},
	// RFC 5869 Test Case 4
	{
	/* Type */	type_sha1, 
	/* IKM */	"0b0b0b0b0b0b0b0b0b0b0b", 
	/* Salt */	"000102030405060708090a0b0c", 
	/* Info */	"f0f1f2f3f4f5f6f7f8f9", 
	/* Len */	42, 
	/* OKM */	"085a01ea1b10f36933068b56efa5ad81"
				"a4f14b822f5b091568a9cdd4f155fda2"
				"c22e422478d305f3f896"
	},
	// RFC 5869 Test Case 5
	{
	/* Type */	type_sha1, 
	/* IKM */	"000102030405060708090a0b0c0d0e0f"
				"101112131415161718191a1b1c1d1e1f"
				"202122232425262728292a2b2c2d2e2f"
				"303132333435363738393a3b3c3d3e3f"
				"404142434445464748494a4b4c4d4e4f", 
	/* Salt */	"606162636465666768696a6b6c6d6e6f"
				"707172737475767778797a7b7c7d7e7f"
				"808182838485868788898a8b8c8d8e8f"
				"909192939495969798999a9b9c9d9e9f"
				"a0a1a2a3a4a5a6a7a8a9aaabacadaeaf", 
	/* Info */	"b0b1b2b3b4b5b6b7b8b9babbbcbdbebf"
				"c0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
				"d0d1d2d3d4d5d6d7d8d9dadbdcdddedf"
				"e0e1e2e3e4e5e6e7e8e9eaebecedeeef"
				"f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff", 
	/* Len */	82, 
	/* OKM */	"0bd770a74d1160f7c9f12cd5912a06eb"
				"ff6adcae899d92191fe4305673ba2ffe"
				"8fa3f1a4e5ad79f3f334b3b202b2173c"
				"486ea37ce3d397ed034c7f9dfeb15c5e"
				"927336d0441f4c4300e2cff0d0900b52"
				"d3b4"
	},
	// RFC 5869 Test Case 6
	{
	/* Type */	type_sha1, 
	/* IKM */	"0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", 
	/* Salt */	"", 
	/* Info */	"", 
	/* Len */	42, 
	/* OKM */	"0ac1af7002b3d761d1e55298da9d0506"
				"b9ae52057220a306e07b6b87e8df21d0"
				"ea00033de03984d34918"
	},
	// RFC 5869 Test Case 7
	{
	/* Type */	type_sha1, 
	/* IKM */	"0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c0c", 
	/* Salt */	"", 
	/* Info */	"", 
	/* Len */	42, 
	/* OKM */	"2c91117204d745f3500d636a62f64f0a"
				"b3bae548aa53d423b0d1f27ebba6f5e5"
				"673a081d70cce7acfc48"
	},
	// RFC 5869 Test Case 1 (updated for SHA-512)
	{
	/* Type */	type_sha512, 
	/* IKM */	"0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B", 
	/* Salt */	"000102030405060708090A0B0C", 
	/* Info */	"F0F1F2F3F4F5F6F7F8F9", 
	/* Len */	42, 
	/* OKM */	"832390086CDA71FB47625BB5CEB168E4"
				"C8E26A1A16ED34D9FC7FE92C14815793"
				"38DA362CB8D9F925D7CB"
	},
	// RFC 5869 Test Case 2 (updated for SHA-512)
	{
	/* Type */	type_sha512, 
	/* IKM */	"000102030405060708090A0B0C0D0E0F"
				"101112131415161718191A1B1C1D1E1F"
				"202122232425262728292A2B2C2D2E2F"
				"303132333435363738393A3B3C3D3E3F"
				"404142434445464748494A4B4C4D4E4F", 
	/* Salt */	"606162636465666768696A6B6C6D6E6F"
				"707172737475767778797A7B7C7D7E7F"
				"808182838485868788898A8B8C8D8E8F"
				"909192939495969798999A9B9C9D9E9F"
				"A0A1A2A3A4A5A6A7A8A9AAABACADAEAF", 
	/* Info */	"B0B1B2B3B4B5B6B7B8B9BABBBCBDBEBF"
				"C0C1C2C3C4C5C6C7C8C9CACBCCCDCECF"
				"D0D1D2D3D4D5D6D7D8D9DADBDCDDDEDF"
				"E0E1E2E3E4E5E6E7E8E9EAEBECEDEEEF"
				"F0F1F2F3F4F5F6F7F8F9FAFBFCFDFEFF", 
	/* Len */	82, 
	/* OKM */	"CE6C97192805B346E6161E821ED16567"
				"3B84F400A2B514B2FE23D84CD189DDF1"
				"B695B48CBD1C8388441137B3CE28F16A"
				"A64BA33BA466B24DF6CFCB021ECFF235"
				"F6A2056CE3AF1DE44D572097A8505D9E"
				"7A93"
	},
	// RFC 5869 Test Case 3 (updated for SHA-512)
	{
	/* Type */	type_sha512, 
	/* IKM */	"0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B0B", 
	/* Salt */	"", 
	/* Info */	"", 
	/* Len */	42, 
	/* OKM */	"F5FA02B18298A72A8C23898A8703472C"
				"6EB179DC204C03425C970E3B164BF90F"
				"FF22D04836D0E2343BAC"
	},
};	

int cchkdftest(TM_UNUSED int argc, TM_UNUSED char *const *argv) {
	size_t i, n;
	int err;
	
	plan_tests(10 * 2);
	
	if(verbose) diag("Starting hkdf tests\n");
	
	n = sizeof(hkdf_sha256_tests) / sizeof(*hkdf_sha256_tests);
	for(i = 0; i < n; ++i) {
		const test_vector_t *			tv   = &hkdf_sha256_tests[ i ];
		byteBuffer						ikm  = hexStringToBytes(tv->ikm);
		byteBuffer						salt = hexStringToBytes(tv->salt);
		byteBuffer						info = hexStringToBytes(tv->info);
		byteBuffer						okmActual = mallocByteBuffer(tv->len);
		byteBuffer						okmExpected = hexStringToBytes(tv->okm);
		const struct ccdigest_info *	di;
		
		if(     tv->type == type_sha1)   di = ccsha1_di();
		else if(tv->type == type_sha256) di = ccsha256_di();
		else if(tv->type == type_sha512) di = ccsha512_di();
		else abort();
		
		err = cchkdf(di, ikm->len, ikm->bytes, salt->len, salt->bytes, info->len, info->bytes, 
			okmActual->len, okmActual->bytes);
		ok(!err, "check return value");
		ok(bytesAreEqual(okmActual, okmExpected), "compare memory of answer");
		
		free(ikm);
		free(salt);
		free(info);
		free(okmActual);
		free(okmExpected);
	}
	return 0;
}
#endif // CCHKDFTEST
