
using System;

namespace Net.Sf.Pkcs11.Wrapper
{
	/// <summary>
	/// PKCS11Constants.
	/// </summary>
	public static class PKCS11Constants
	{
		internal const uint CK_EFFECTIVELY_INFINITE = 0 ;
		internal const uint CK_INVALID_HANDLE = 0 ;
		internal const uint CKA_AC_ISSUER = 0x00000083 ;
		internal const uint CKA_ALLOWED_MECHANISMS =(CKF_ARRAY_ATTRIBUTE|0x00000600) ;
		internal const uint CKA_ALWAYS_AUTHENTICATE = 0x00000202 ;
		internal const uint CKA_ALWAYS_SENSITIVE = 0x00000165 ;
		internal const uint CKA_APPLICATION = 0x00000010 ;
		internal const uint CKA_ATTR_TYPES = 0x00000085 ;
		internal const uint CKA_AUTH_PIN_FLAGS = 0x00000201 /* Deprecated */ ;
		internal const uint CKA_BASE = 0x00000132 ;
		internal const uint CKA_BITS_PER_PIXEL = 0x00000406 ;
		internal const uint CKA_CERTIFICATE_CATEGORY = 0x00000087 ;
		internal const uint CKA_CERTIFICATE_TYPE = 0x00000080 ;
		internal const uint CKA_CHAR_COLUMNS = 0x00000404 ;
		internal const uint CKA_CHAR_ROWS = 0x00000403 ;
		internal const uint CKA_CHAR_SETS = 0x00000480 ;
		internal const uint CKA_CHECK_VALUE = 0x00000090 ;
		internal const uint CKA_CLASS = 0x00000000 ;
		internal const uint CKA_COEFFICIENT = 0x00000128 ;
		internal const uint CKA_COLOR = 0x00000405 ;
		internal const uint CKA_DECRYPT = 0x00000105 ;
		internal const uint CKA_DEFAULT_CMS_ATTRIBUTES = 0x00000502 ;
		internal const uint CKA_DERIVE = 0x0000010C ;
		internal const uint CKA_EC_PARAMS = 0x00000180 ;
		internal const uint CKA_EC_POINT = 0x00000181 ;
		internal const uint CKA_ECDSA_PARAMS = 0x00000180 ;
		internal const uint CKA_ENCODING_METHODS = 0x00000481 ;
		internal const uint CKA_ENCRYPT = 0x00000104 ;
		internal const uint CKA_END_DATE = 0x00000111 ;
		internal const uint CKA_EXPONENT_1 = 0x00000126 ;
		internal const uint CKA_EXPONENT_2 = 0x00000127 ;
		internal const uint CKA_EXTRACTABLE = 0x00000162 ;
		internal const uint CKA_HAS_RESET = 0x00000302 ;
		internal const uint CKA_HASH_OF_ISSUER_PUBLIC_KEY = 0x0000008B ;
		internal const uint CKA_HASH_OF_SUBJECT_PUBLIC_KEY = 0x0000008A ;
		internal const uint CKA_HW_FEATURE_TYPE = 0x00000300 ;
		internal const uint CKA_ID = 0x00000102 ;
		internal const uint CKA_ISSUER = 0x00000081 ;
		internal const uint CKA_JAVA_MIDP_SECURITY_DOMAIN = 0x00000088 ;
		internal const uint CKA_KEY_GEN_MECHANISM = 0x00000166 ;
		internal const uint CKA_KEY_TYPE = 0x00000100 ;
		internal const uint CKA_LABEL = 0x00000003 ;
		internal const uint CKA_LOCAL = 0x00000163 ;
		internal const uint CKA_MECHANISM_TYPE = 0x00000500 ;
		internal const uint CKA_MIME_TYPES = 0x00000482 ;
		internal const uint CKA_MODIFIABLE = 0x00000170 ;
		internal const uint CKA_MODULUS = 0x00000120 ;
		internal const uint CKA_MODULUS_BITS = 0x00000121 ;
		internal const uint CKA_NEVER_EXTRACTABLE = 0x00000164 ;
		internal const uint CKA_OBJECT_ID = 0x00000012 ;
		internal const uint CKA_OWNER = 0x00000084 ;
		internal const uint CKA_PIXEL_X = 0x00000400 ;
		internal const uint CKA_PIXEL_Y = 0x00000401 ;
		internal const uint CKA_PRIME = 0x00000130 ;
		internal const uint CKA_PRIME_1 = 0x00000124 ;
		internal const uint CKA_PRIME_2 = 0x00000125 ;
		internal const uint CKA_PRIME_BITS = 0x00000133 ;
		internal const uint CKA_PRIVATE = 0x00000002 ;
		internal const uint CKA_PRIVATE_EXPONENT = 0x00000123 ;
		internal const uint CKA_PUBLIC_EXPONENT = 0x00000122 ;
		internal const uint CKA_REQUIRED_CMS_ATTRIBUTES = 0x00000501 ;
		internal const uint CKA_RESET_ON_INIT = 0x00000301 ;
		internal const uint CKA_RESOLUTION = 0x00000402 ;
		internal const uint CKA_SECONDARY_AUTH = 0x00000200 /* Deprecated */ ;
		internal const uint CKA_SENSITIVE = 0x00000103 ;
		internal const uint CKA_SERIAL_NUMBER = 0x00000082 ;
		internal const uint CKA_SIGN = 0x00000108 ;
		internal const uint CKA_SIGN_RECOVER = 0x00000109 ;
		internal const uint CKA_START_DATE = 0x00000110 ;
		internal const uint CKA_SUBJECT = 0x00000101 ;
		internal const uint CKA_SUBPRIME = 0x00000131 ;
		internal const uint CKA_SUBPRIME_BITS = 0x00000134 ;
		internal const uint CKA_SUPPORTED_CMS_ATTRIBUTES = 0x00000503 ;
		internal const uint CKA_TOKEN = 0x00000001 ;
		internal const uint CKA_TRUSTED = 0x00000086 ;
		internal const uint CKA_UNWRAP = 0x00000107 ;
		internal const uint CKA_UNWRAP_TEMPLATE = (CKF_ARRAY_ATTRIBUTE|0x00000212) ;
		internal const uint CKA_URL = 0x00000089 ;
		internal const uint CKA_VALUE = 0x00000011 ;
		internal const uint CKA_VALUE_BITS = 0x00000160 ;
		internal const uint CKA_VALUE_LEN = 0x00000161 ;
		internal const uint CKA_VENDOR_DEFINED = 0x80000000 ;
		internal const uint CKA_VERIFY = 0x0000010A ;
		internal const uint CKA_VERIFY_RECOVER = 0x0000010B ;
		internal const uint CKA_WRAP = 0x00000106 ;
		internal const uint CKA_WRAP_TEMPLATE = (CKF_ARRAY_ATTRIBUTE|0x00000211) ;
		internal const uint CKA_WRAP_WITH_TRUSTED = 0x00000210 ;


        /// <summary>
        /// Набор эталонных параметров из RFC 4357,
        /// используемых в алгоритме формирования ключевой пары,
        /// описанном в ГОСТ 34.10-2001.
        /// </summary>
        internal const uint CKA_GOST3410PARAMS_EX = 0xD4321001;

        internal const uint CKA_GOSTR3410PARAMS = 0x00000250;
        internal const uint CKA_GOSTR3411PARAMS = 0x00000251;
        internal const uint CKA_GOST28147PARAMS = 0x00000252;

		internal const uint CKC_VENDOR_DEFINED = 0x80000000 ;
		internal const uint CKC_WTLS = 0x00000002 ;
		internal const uint CKC_X_509 = 0x00000000 ;
		internal const uint CKC_X_509_ATTR_CERT =0x00000001 ;

		internal const uint CKF_ARRAY_ATTRIBUTE = 0x40000000 ;
		internal const uint CKF_DONT_BLOCK = 1 ;
		internal const uint CKF_RW_SESSION = 0x00000002;
		internal const uint CKF_SERIAL_SESSION = 0x00000004;

		internal const uint CKH_CLOCK = 0x00000002 ;
		internal const uint CKH_MONOTONIC_COUNTER = 0x00000001 ;
		internal const uint CKH_USER_INTERFACE = 0x00000003 ;
		internal const uint CKH_VENDOR_DEFINED = 0x80000000 ;

		internal const uint CKK_AES = 0x0000001F ;
		internal const uint CKK_BATON = 0x0000001C ;
		internal const uint CKK_BLOWFISH = 0x00000020 ;
		internal const uint CKK_CAST = 0x00000016 ;
		internal const uint CKK_CAST128 = 0x00000018 ;
		internal const uint CKK_CAST3 = 0x00000017 ;
		internal const uint CKK_CAST5 = 0x00000018 ;
		internal const uint CKK_CDMF = 0x0000001E ;
		internal const uint CKK_DES = 0x00000013 ;
		internal const uint CKK_DES2 = 0x00000014 ;
		internal const uint CKK_DES3 = 0x00000015 ;
		internal const uint CKK_DH = 0x00000002 ;
		internal const uint CKK_DSA = 0x00000001 ;
		internal const uint CKK_EC = 0x00000003 ;
		internal const uint CKK_ECDSA = 0x00000003 ;
		internal const uint CKK_GENERIC_SECRET = 0x00000010 ;
		internal const uint CKK_IDEA = 0x0000001A ;
		internal const uint CKK_JUNIPER = 0x0000001D ;
		internal const uint CKK_KEA = 0x00000005 ;
		internal const uint CKK_RC2 = 0x00000011 ;
		internal const uint CKK_RC4 = 0x00000012 ;
		internal const uint CKK_RC5 = 0x00000019 ;
		internal const uint CKK_RSA = 0x00000000 ;
		internal const uint CKK_SKIPJACK = 0x0000001B ;
		internal const uint CKK_TWOFISH = 0x00000021 ;
		internal const uint CKK_VENDOR_DEFINED = 0x80000000 ;
		internal const uint CKK_X9_42_DH = 0x00000004 ;
        internal const uint CKK_GOSTR3410 = 0x00000030 ;

		internal const uint CKM_AES_CBC = 0x00001082 ;
		internal const uint CKM_AES_CBC_ENCRYPT_DATA = 0x00001105 ;
		internal const uint CKM_AES_CBC_PAD = 0x00001085 ;
		internal const uint CKM_AES_ECB = 0x00001081 ;
		internal const uint CKM_AES_ECB_ENCRYPT_DATA = 0x00001104 ;
		internal const uint CKM_AES_KEY_GEN = 0x00001080 ;
		internal const uint CKM_AES_MAC = 0x00001083 ;
		internal const uint CKM_AES_MAC_GENERAL = 0x00001084 ;
		internal const uint CKM_BATON_CBC128 = 0x00001033 ;
		internal const uint CKM_BATON_COUNTER = 0x00001034 ;
		internal const uint CKM_BATON_ECB128 = 0x00001031 ;
		internal const uint CKM_BATON_ECB96 = 0x00001032 ;
		internal const uint CKM_BATON_KEY_GEN = 0x00001030 ;
		internal const uint CKM_BATON_SHUFFLE = 0x00001035 ;
		internal const uint CKM_BATON_WRAP = 0x00001036 ;
		internal const uint CKM_BLOWFISH_CBC = 0x00001091 ;
		internal const uint CKM_BLOWFISH_KEY_GEN = 0x00001090 ;
		internal const uint CKM_CAST_CBC = 0x00000302 ;
		internal const uint CKM_CAST_CBC_PAD = 0x00000305 ;
		internal const uint CKM_CAST_ECB = 0x00000301 ;
		internal const uint CKM_CAST_KEY_GEN = 0x00000300 ;
		internal const uint CKM_CAST_MAC = 0x00000303 ;
		internal const uint CKM_CAST_MAC_GENERAL = 0x00000304 ;
		internal const uint CKM_CAST128_CBC = 0x00000322 ;
		internal const uint CKM_CAST128_CBC_PAD = 0x00000325 ;
		internal const uint CKM_CAST128_ECB = 0x00000321 ;
		internal const uint CKM_CAST128_KEY_GEN = 0x00000320 ;
		internal const uint CKM_CAST128_MAC = 0x00000323 ;
		internal const uint CKM_CAST128_MAC_GENERAL = 0x00000324 ;
		internal const uint CKM_CAST3_CBC = 0x00000312 ;
		internal const uint CKM_CAST3_CBC_PAD = 0x00000315 ;
		internal const uint CKM_CAST3_ECB = 0x00000311 ;
		internal const uint CKM_CAST3_KEY_GEN = 0x00000310 ;
		internal const uint CKM_CAST3_MAC = 0x00000313 ;
		internal const uint CKM_CAST3_MAC_GENERAL = 0x00000314 ;
		internal const uint CKM_CAST5_CBC = 0x00000322 ;
		internal const uint CKM_CAST5_CBC_PAD = 0x00000325 ;
		internal const uint CKM_CAST5_ECB = 0x00000321 ;
		internal const uint CKM_CAST5_KEY_GEN = 0x00000320 ;
		internal const uint CKM_CAST5_MAC = 0x00000323 ;
		internal const uint CKM_CAST5_MAC_GENERAL = 0x00000324 ;
		internal const uint CKM_CDMF_CBC = 0x00000142 ;
		internal const uint CKM_CDMF_CBC_PAD = 0x00000145 ;
		internal const uint CKM_CDMF_ECB = 0x00000141 ;
		internal const uint CKM_CDMF_KEY_GEN = 0x00000140 ;
		internal const uint CKM_CDMF_MAC = 0x00000143 ;
		internal const uint CKM_CDMF_MAC_GENERAL = 0x00000144 ;
		internal const uint CKM_CMS_SIG = 0x00000500 ;
		internal const uint CKM_CONCATENATE_BASE_AND_DATA = 0x00000362 ;
		internal const uint CKM_CONCATENATE_BASE_AND_KEY = 0x00000360 ;
		internal const uint CKM_CONCATENATE_DATA_AND_BASE = 0x00000363 ;
		internal const uint CKM_DES_CBC = 0x00000122 ;
		internal const uint CKM_DES_CBC_ENCRYPT_DATA = 0x00001101 ;
		internal const uint CKM_DES_CBC_PAD = 0x00000125 ;
		internal const uint CKM_DES_CFB64 = 0x00000152 ;
		internal const uint CKM_DES_CFB8 = 0x00000153 ;
		internal const uint CKM_DES_ECB = 0x00000121 ;
		internal const uint CKM_DES_ECB_ENCRYPT_DATA = 0x00001100 ;
		internal const uint CKM_DES_KEY_GEN = 0x00000120 ;
		internal const uint CKM_DES_MAC = 0x00000123 ;
		internal const uint CKM_DES_MAC_GENERAL = 0x00000124 ;
		internal const uint CKM_DES_OFB64 = 0x00000150 ;
		internal const uint CKM_DES_OFB8 = 0x00000151 ;
		internal const uint CKM_DES2_KEY_GEN = 0x00000130 ;
		internal const uint CKM_DES3_CBC = 0x00000133 ;
		internal const uint CKM_DES3_CBC_ENCRYPT_DATA = 0x00001103 ;
		internal const uint CKM_DES3_CBC_PAD = 0x00000136 ;
		internal const uint CKM_DES3_ECB = 0x00000132 ;
		internal const uint CKM_DES3_ECB_ENCRYPT_DATA = 0x00001102 ;
		internal const uint CKM_DES3_KEY_GEN = 0x00000131 ;
		internal const uint CKM_DES3_MAC = 0x00000134 ;
		internal const uint CKM_DES3_MAC_GENERAL = 0x00000135 ;
		internal const uint CKM_DH_PKCS_DERIVE = 0x00000021 ;
		internal const uint CKM_DH_PKCS_KEY_PAIR_GEN = 0x00000020 ;
		internal const uint CKM_DH_PKCS_PARAMETER_GEN = 0x00002001 ;
		internal const uint CKM_DSA = 0x00000011 ;
		internal const uint CKM_DSA_KEY_PAIR_GEN = 0x00000010 ;
		internal const uint CKM_DSA_PARAMETER_GEN = 0x00002000 ;
		internal const uint CKM_DSA_SHA1 = 0x00000012 ;
		internal const uint CKM_EC_KEY_PAIR_GEN = 0x00001040 ;
		internal const uint CKM_ECDH1_COFACTOR_DERIVE = 0x00001051 ;
		internal const uint CKM_ECDH1_DERIVE = 0x00001050 ;
		internal const uint CKM_ECDSA = 0x00001041 ;
		internal const uint CKM_ECDSA_KEY_PAIR_GEN = 0x00001040 ;
		internal const uint CKM_ECDSA_SHA1 = 0x00001042 ;
		internal const uint CKM_ECMQV_DERIVE = 0x00001052 ;
		internal const uint CKM_EXTRACT_KEY_FROM_KEY = 0x00000365 ;
		internal const uint CKM_FASTHASH = 0x00001070 ;
		internal const uint CKM_FORTEZZA_TIMESTAMP = 0x00001020 ;
		internal const uint CKM_GENERIC_SECRET_KEY_GEN = 0x00000350 ;
		internal const uint CKM_IDEA_CBC = 0x00000342 ;
		internal const uint CKM_IDEA_CBC_PAD = 0x00000345 ;
		internal const uint CKM_IDEA_ECB = 0x00000341 ;
		internal const uint CKM_IDEA_KEY_GEN = 0x00000340 ;
		internal const uint CKM_IDEA_MAC = 0x00000343 ;
		internal const uint CKM_IDEA_MAC_GENERAL = 0x00000344 ;
		internal const uint CKM_JUNIPER_CBC128 = 0x00001062 ;
		internal const uint CKM_JUNIPER_COUNTER = 0x00001063 ;
		internal const uint CKM_JUNIPER_ECB128 = 0x00001061 ;
		internal const uint CKM_JUNIPER_KEY_GEN = 0x00001060 ;
		internal const uint CKM_JUNIPER_SHUFFLE = 0x00001064 ;
		internal const uint CKM_JUNIPER_WRAP = 0x00001065 ;
		internal const uint CKM_KEA_KEY_DERIVE = 0x00001011 ;
		internal const uint CKM_KEA_KEY_PAIR_GEN = 0x00001010 ;
		internal const uint CKM_KEY_WRAP_LYNKS = 0x00000400 ;
		internal const uint CKM_KEY_WRAP_SET_OAEP = 0x00000401 ;
		internal const uint CKM_MD2 = 0x00000200 ;
		internal const uint CKM_MD2_HMAC = 0x00000201 ;
		internal const uint CKM_MD2_HMAC_GENERAL = 0x00000202 ;
		internal const uint CKM_MD2_KEY_DERIVATION = 0x00000391 ;
		internal const uint CKM_MD2_RSA_PKCS = 0x00000004 ;
		internal const uint CKM_MD5 = 0x00000210 ;
		internal const uint CKM_MD5_HMAC = 0x00000211 ;
		internal const uint CKM_MD5_HMAC_GENERAL = 0x00000212 ;
		internal const uint CKM_MD5_KEY_DERIVATION = 0x00000390 ;
		internal const uint CKM_MD5_RSA_PKCS = 0x00000005 ;
		internal const uint CKM_PBA_SHA1_WITH_SHA1_HMAC = 0x000003C0 ;
		internal const uint CKM_PBE_MD2_DES_CBC = 0x000003A0 ;
		internal const uint CKM_PBE_MD5_CAST_CBC = 0x000003A2 ;
		internal const uint CKM_PBE_MD5_CAST128_CBC = 0x000003A4 ;
		internal const uint CKM_PBE_MD5_CAST3_CBC = 0x000003A3 ;
		internal const uint CKM_PBE_MD5_CAST5_CBC = 0x000003A4 ;
		internal const uint CKM_PBE_MD5_DES_CBC = 0x000003A1 ;
		internal const uint CKM_PBE_SHA1_CAST128_CBC = 0x000003A5 ;
		internal const uint CKM_PBE_SHA1_CAST5_CBC = 0x000003A5 ;
		internal const uint CKM_PBE_SHA1_DES2_EDE_CBC = 0x000003A9 ;
		internal const uint CKM_PBE_SHA1_DES3_EDE_CBC = 0x000003A8 ;
		internal const uint CKM_PBE_SHA1_RC2_128_CBC = 0x000003AA ;
		internal const uint CKM_PBE_SHA1_RC2_40_CBC = 0x000003AB ;
		internal const uint CKM_PBE_SHA1_RC4_128 = 0x000003A6 ;
		internal const uint CKM_PBE_SHA1_RC4_40 = 0x000003A7 ;
		internal const uint CKM_PKCS5_PBKD2 = 0x000003B0 ;
		internal const uint CKM_RC2_CBC = 0x00000102 ;
		internal const uint CKM_RC2_CBC_PAD = 0x00000105 ;
		internal const uint CKM_RC2_ECB = 0x00000101 ;
		internal const uint CKM_RC2_KEY_GEN = 0x00000100 ;
		internal const uint CKM_RC2_MAC = 0x00000103 ;
		internal const uint CKM_RC2_MAC_GENERAL = 0x00000104 ;
		internal const uint CKM_RC4 = 0x00000111 ;
		internal const uint CKM_RC4_KEY_GEN = 0x00000110 ;
		internal const uint CKM_RC5_CBC = 0x00000332 ;
		internal const uint CKM_RC5_CBC_PAD = 0x00000335 ;
		internal const uint CKM_RC5_ECB = 0x00000331 ;
		internal const uint CKM_RC5_KEY_GEN = 0x00000330 ;
		internal const uint CKM_RC5_MAC = 0x00000333 ;
		internal const uint CKM_RC5_MAC_GENERAL = 0x00000334 ;
		internal const uint CKM_RIPEMD128 = 0x00000230 ;
		internal const uint CKM_RIPEMD128_HMAC = 0x00000231 ;
		internal const uint CKM_RIPEMD128_HMAC_GENERAL = 0x00000232 ;
		internal const uint CKM_RIPEMD128_RSA_PKCS = 0x00000007 ;
		internal const uint CKM_RIPEMD160 = 0x00000240 ;
		internal const uint CKM_RIPEMD160_HMAC = 0x00000241 ;
		internal const uint CKM_RIPEMD160_HMAC_GENERAL = 0x00000242 ;
		internal const uint CKM_RIPEMD160_RSA_PKCS = 0x00000008 ;
		internal const uint CKM_RSA_9796 = 0x00000002 ;
		internal const uint CKM_RSA_PKCS = 0x00000001 ;
		internal const uint CKM_RSA_PKCS_KEY_PAIR_GEN = 0x00000000 ;
		internal const uint CKM_RSA_PKCS_OAEP = 0x00000009 ;
		internal const uint CKM_RSA_PKCS_PSS = 0x0000000D ;
		internal const uint CKM_RSA_X_509 = 0x00000003 ;
		internal const uint CKM_RSA_X9_31 = 0x0000000B ;
		internal const uint CKM_RSA_X9_31_KEY_PAIR_GEN = 0x0000000A ;
		internal const uint CKM_SHA_1 = 0x00000220 ;
		internal const uint CKM_SHA_1_HMAC = 0x00000221 ;
		internal const uint CKM_SHA_1_HMAC_GENERAL = 0x00000222 ;
		internal const uint CKM_SHA1_KEY_DERIVATION = 0x00000392 ;
		internal const uint CKM_SHA1_RSA_PKCS = 0x00000006 ;
		internal const uint CKM_SHA1_RSA_PKCS_PSS = 0x0000000E ;
		internal const uint CKM_SHA1_RSA_X9_31 = 0x0000000C ;
		internal const uint CKM_SHA256 = 0x00000250 ;
		internal const uint CKM_SHA256_HMAC = 0x00000251 ;
		internal const uint CKM_SHA256_HMAC_GENERAL = 0x00000252 ;
		internal const uint CKM_SHA256_KEY_DERIVATION = 0x00000393 ;
		internal const uint CKM_SHA256_RSA_PKCS = 0x00000040 ;
		internal const uint CKM_SHA256_RSA_PKCS_PSS = 0x00000043 ;
		internal const uint CKM_SHA384 = 0x00000260 ;
		internal const uint CKM_SHA384_HMAC = 0x00000261 ;
		internal const uint CKM_SHA384_HMAC_GENERAL = 0x00000262 ;
		internal const uint CKM_SHA384_KEY_DERIVATION = 0x00000394 ;
		internal const uint CKM_SHA384_RSA_PKCS = 0x00000041 ;
		internal const uint CKM_SHA384_RSA_PKCS_PSS = 0x00000044 ;
		internal const uint CKM_SHA512 = 0x00000270 ;
		internal const uint CKM_SHA512_HMAC = 0x00000271 ;
		internal const uint CKM_SHA512_HMAC_GENERAL = 0x00000272 ;
		internal const uint CKM_SHA512_KEY_DERIVATION = 0x00000395 ;
		internal const uint CKM_SHA512_RSA_PKCS = 0x00000042 ;
		internal const uint CKM_SHA512_RSA_PKCS_PSS = 0x00000045 ;
		internal const uint CKM_SKIPJACK_CBC64 = 0x00001002 ;
		internal const uint CKM_SKIPJACK_CFB16 = 0x00001006 ;
		internal const uint CKM_SKIPJACK_CFB32 = 0x00001005 ;
		internal const uint CKM_SKIPJACK_CFB64 = 0x00001004 ;
		internal const uint CKM_SKIPJACK_CFB8 = 0x00001007 ;
		internal const uint CKM_SKIPJACK_ECB64 = 0x00001001 ;
		internal const uint CKM_SKIPJACK_KEY_GEN = 0x00001000 ;
		internal const uint CKM_SKIPJACK_OFB64 = 0x00001003 ;
		internal const uint CKM_SKIPJACK_PRIVATE_WRAP = 0x00001009 ;
		internal const uint CKM_SKIPJACK_RELAYX = 0x0000100a ;
		internal const uint CKM_SKIPJACK_WRAP = 0x00001008 ;
		internal const uint CKM_SSL3_KEY_AND_MAC_DERIVE = 0x00000372 ;
		internal const uint CKM_SSL3_MASTER_KEY_DERIVE = 0x00000371 ;
		internal const uint CKM_SSL3_MASTER_KEY_DERIVE_DH = 0x00000373 ;
		internal const uint CKM_SSL3_MD5_MAC = 0x00000380 ;
		internal const uint CKM_SSL3_PRE_MASTER_KEY_GEN = 0x00000370 ;
		internal const uint CKM_SSL3_SHA1_MAC = 0x00000381 ;
		internal const uint CKM_TLS_KEY_AND_MAC_DERIVE = 0x00000376 ;
		internal const uint CKM_TLS_MASTER_KEY_DERIVE = 0x00000375 ;
		internal const uint CKM_TLS_MASTER_KEY_DERIVE_DH = 0x00000377 ;
		internal const uint CKM_TLS_PRE_MASTER_KEY_GEN = 0x00000374 ;
		internal const uint CKM_TLS_PRF = 0x00000378 ;
		internal const uint CKM_TWOFISH_CBC = 0x00001093 ;
		internal const uint CKM_TWOFISH_KEY_GEN = 0x00001092 ;
		internal const uint CKM_VENDOR_DEFINED = 0x80000000 ;
		internal const uint CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE = 0x000003D5 ;
		internal const uint CKM_WTLS_MASTER_KEY_DERIVE = 0x000003D1 ;
		internal const uint CKM_WTLS_MASTER_KEY_DERVIE_DH_ECC = 0x000003D2 ;
		internal const uint CKM_WTLS_PRE_MASTER_KEY_GEN = 0x000003D0 ;
		internal const uint CKM_WTLS_PRF = 0x000003D3 ;
		internal const uint CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE = 0x000003D4 ;
		internal const uint CKM_X9_42_DH_DERIVE = 0x00000031 ;
		internal const uint CKM_X9_42_DH_HYBRID_DERIVE = 0x00000032 ;
		internal const uint CKM_X9_42_DH_KEY_PAIR_GEN = 0x00000030 ;
		internal const uint CKM_X9_42_DH_PARAMETER_GEN = 0x00002002 ;
		internal const uint CKM_X9_42_MQV_DERIVE = 0x00000033 ;
		internal const uint CKM_XOR_BASE_AND_DATA = 0x00000364 ;
        
        /// <summary>
        /// Механизм для генерации и проверки ЭЦП с использованием в качестве входа
        /// ранее вычисленного значения хэш-функции (32 байта ровно).
        /// </summary>
        internal const uint CKM_GOSTR3410 = 0x00001201;

        /// <summary>
        /// Механизм для генерации ключевой пары.
        /// </summary>
        internal const uint CKM_GOSTR3410_KEY_PAIR_GEN = 0x00001200;
                
        /// <summary>
        /// Механизм для генерации ключевой пары согласно стандарту ГОСТ Р 34.10-2001.
        /// (From Etoken documentation, см. п.5.2. Параметры цифровой подписи).
        /// </summary>
        internal const uint CKM_GOSTR3410_KEY_PAIR_GEN_EX = 0xD4321010;

        /// <summary>
        /// Механизм для генерации и проверки ЭЦП с хэшированием подаваемых на вход данных.
        /// Размер входных данных не ограничен.
        /// </summary>
        internal const uint CKM_GOSTR3410_WITH_GOSTR3411 = 0x00001202;
        
        /// <summary>
        /// Механизм для выработки ключа согласования.
        /// </summary>
        internal const uint CKM_GOSTR3410_DERIVE = 0x00001204;

        /// <summary>
        /// Механизм вычисления хэш-функции.
        /// </summary>
        internal const uint CKM_GOSTR3411 = 0x00001210;

        /// <summary>
        /// Механизм шифрования данных.
        /// </summary>
        internal const uint CKM_GOST28147 = 0x00001222;

        /// <summary>
        /// Механизм шифрования данных с использованием метода простой замены.
        /// </summary>
        internal const uint CKM_GOST28147_ECB = 0x00001221;

        /// <summary>
        /// Механизм экспорта и импорта открытых ключей.
        /// </summary>
        internal const uint CKM_GOST28147_KEY_WRAP = 0x00001224;

        /// <summary>
        /// Механизм выработки симметричных ключей.
        /// </summary>
        internal const uint CKM_GOST28147_KEY_GEN = 0x00001220;      


        /// <summary>
        /// Parameters for elleptic curves (according to RFC 4357, paragraph 10.8)
        /// 
        /// Параметры эллиптических кривых (по RFC 4357, п.10.8)
        /// </summary>
        internal static byte[] SC_PARAMSET_GOSTR3410_A = { 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x23, 0x01 };
        internal static byte[] SC_PARAMSET_GOSTR3410_B = { 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x23, 0x02 };
        internal static byte[] SC_PARAMSET_GOSTR3410_C = { 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x23, 0x03 };
        internal static byte[] SC_PARAMSET_GOSTR3410_D = { 0x06, 0x07, 0x2A, 0x85, 0x03, 0x02, 0x02, 0x23, 0x04 };

		internal const uint CKN_SURRENDER = 0 ;

		internal const uint CKO_CERTIFICATE = 0x00000001 ;
		internal const uint CKO_DATA = 0x00000000 ;
		internal const uint CKO_DOMAIN_PARAMETERS = 0x00000006 ;
		internal const uint CKO_HW_FEATURE = 0x00000005 ;
		internal const uint CKO_MECHANISM = 0x00000007 ;
		internal const uint CKO_PRIVATE_KEY = 0x00000003 ;
		internal const uint CKO_PUBLIC_KEY = 0x00000002 ;
		internal const uint CKO_SECRET_KEY = 0x00000004 ;
		internal const uint CKO_VENDOR_DEFINED = 0x80000000 ;


		internal const uint CKR_ARGUMENTS_BAD = 0x00000007 ;
		internal const uint CKR_ATTRIBUTE_READ_ONLY = 0x00000010 ;
		internal const uint CKR_ATTRIBUTE_SENSITIVE = 0x00000011 ;
		internal const uint CKR_ATTRIBUTE_TYPE_INVALID = 0x00000012 ;
		internal const uint CKR_ATTRIBUTE_VALUE_INVALID = 0x00000013 ;
		internal const uint CKR_BUFFER_TOO_SMALL = 0x00000150 ;
		internal const uint CKR_CANCEL = 0x00000001 ;
		internal const uint CKR_CANT_LOCK = 0x0000000A ;
		internal const uint CKR_CRYPTOKI_ALREADY_INITIALIZED = 0x00000191 ;
		internal const uint CKR_CRYPTOKI_NOT_INITIALIZED = 0x00000190 ;
		internal const uint CKR_DATA_INVALID = 0x00000020 ;
		internal const uint CKR_DATA_LEN_RANGE = 0x00000021 ;
		internal const uint CKR_DEVICE_ERROR = 0x00000030 ;
		internal const uint CKR_DEVICE_MEMORY = 0x00000031 ;
		internal const uint CKR_DEVICE_REMOVED = 0x00000032 ;
		internal const uint CKR_DOMAIN_PARAMS_INVALID = 0x00000130 ;
		internal const uint CKR_ENCRYPTED_DATA_INVALID = 0x00000040 ;
		internal const uint CKR_ENCRYPTED_DATA_LEN_RANGE = 0x00000041 ;
		internal const uint CKR_FUNCTION_CANCELED = 0x00000050 ;
		internal const uint CKR_FUNCTION_FAILED = 0x00000006 ;
		internal const uint CKR_FUNCTION_NOT_PARALLEL = 0x00000051 ;
		internal const uint CKR_FUNCTION_NOT_SUPPORTED = 0x00000054 ;
		internal const uint CKR_FUNCTION_REJECTED = 0x00000200 ;
		internal const uint CKR_GENERAL_ERROR = 0x00000005 ;
		internal const uint CKR_HOST_MEMORY = 0x00000002 ;
		internal const uint CKR_INFORMATION_SENSITIVE = 0x00000170 ;
		internal const uint CKR_KEY_CHANGED = 0x00000065 ;
		internal const uint CKR_KEY_FUNCTION_NOT_PERMITTED = 0x00000068 ;
		internal const uint CKR_KEY_HANDLE_INVALID = 0x00000060 ;
		internal const uint CKR_KEY_INDIGESTIBLE = 0x00000067 ;
		internal const uint CKR_KEY_NEEDED = 0x00000066 ;
		internal const uint CKR_KEY_NOT_NEEDED = 0x00000064 ;
		internal const uint CKR_KEY_NOT_WRAPPABLE = 0x00000069 ;
		internal const uint CKR_KEY_SIZE_RANGE = 0x00000062 ;
		internal const uint CKR_KEY_TYPE_INCONSISTENT = 0x00000063 ;
		internal const uint CKR_KEY_UNEXTRACTABLE = 0x0000006A ;
		internal const uint CKR_MECHANISM_INVALID = 0x00000070 ;
		internal const uint CKR_MECHANISM_PARAM_INVALID = 0x00000071 ;
		internal const uint CKR_MUTEX_BAD = 0x000001A0 ;
		internal const uint CKR_MUTEX_NOT_LOCKED = 0x000001A1 ;
		internal const uint CKR_NEED_TO_CREATE_THREADS = 0x00000009 ;
		internal const uint CKR_NO_EVENT = 0x00000008 ;
		internal const uint CKR_OBJECT_HANDLE_INVALID = 0x00000082 ;
		internal const uint CKR_OK = 0x00000000 ;
		internal const uint CKR_OPERATION_ACTIVE = 0x00000090 ;
		internal const uint CKR_OPERATION_NOT_INITIALIZED = 0x00000091 ;
		internal const uint CKR_PIN_EXPIRED = 0x000000A3 ;
		internal const uint CKR_PIN_INCORRECT = 0x000000A0 ;
		internal const uint CKR_PIN_INVALID = 0x000000A1 ;
		internal const uint CKR_PIN_LEN_RANGE = 0x000000A2 ;
		internal const uint CKR_PIN_LOCKED = 0x000000A4 ;
		internal const uint CKR_RANDOM_NO_RNG = 0x00000121 ;
		internal const uint CKR_RANDOM_SEED_NOT_SUPPORTED = 0x00000120 ;
		internal const uint CKR_SAVED_STATE_INVALID = 0x00000160 ;
		internal const uint CKR_SESSION_CLOSED = 0x000000B0 ;
		internal const uint CKR_SESSION_COUNT = 0x000000B1 ;
		internal const uint CKR_SESSION_EXISTS = 0x000000B6 ;
		internal const uint CKR_SESSION_HANDLE_INVALID = 0x000000B3 ;
		internal const uint CKR_SESSION_PARALLEL_NOT_SUPPORTED = 0x000000B4 ;
		internal const uint CKR_SESSION_READ_ONLY = 0x000000B5 ;
		internal const uint CKR_SESSION_READ_ONLY_EXISTS = 0x000000B7 ;
		internal const uint CKR_SESSION_READ_WRITE_SO_EXISTS = 0x000000B8 ;
		internal const uint CKR_SIGNATURE_INVALID = 0x000000C0 ;
		internal const uint CKR_SIGNATURE_LEN_RANGE = 0x000000C1 ;
		internal const uint CKR_SLOT_ID_INVALID = 0x00000003 ;
		internal const uint CKR_STATE_UNSAVEABLE = 0x00000180 ;
		internal const uint CKR_TEMPLATE_INCOMPLETE = 0x000000D0 ;
		internal const uint CKR_TEMPLATE_INCONSISTENT = 0x000000D1 ;
		internal const uint CKR_TOKEN_NOT_PRESENT = 0x000000E0 ;
		internal const uint CKR_TOKEN_NOT_RECOGNIZED = 0x000000E1 ;
		internal const uint CKR_TOKEN_WRITE_PROTECTED = 0x000000E2 ;
		internal const uint CKR_UNWRAPPING_KEY_HANDLE_INVALID = 0x000000F0 ;
		internal const uint CKR_UNWRAPPING_KEY_SIZE_RANGE = 0x000000F1 ;
		internal const uint CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT = 0x000000F2 ;
		internal const uint CKR_USER_ALREADY_LOGGED_IN = 0x00000100 ;
		internal const uint CKR_USER_ANOTHER_ALREADY_LOGGED_IN = 0x00000104 ;
		internal const uint CKR_USER_NOT_LOGGED_IN = 0x00000101 ;
		internal const uint CKR_USER_PIN_NOT_INITIALIZED = 0x00000102 ;
		internal const uint CKR_USER_TOO_MANY_TYPES = 0x00000105 ;
		internal const uint CKR_USER_TYPE_INVALID = 0x00000103 ;
		internal const uint CKR_VENDOR_DEFINED = 0x80000000;
		internal const uint CKR_WRAPPED_KEY_INVALID = 0x00000110 ;
		internal const uint CKR_WRAPPED_KEY_LEN_RANGE = 0x00000112 ;
		internal const uint CKR_WRAPPING_KEY_HANDLE_INVALID = 0x00000113 ;
		internal const uint CKR_WRAPPING_KEY_SIZE_RANGE = 0x00000114 ;
		internal const uint CKR_WRAPPING_KEY_TYPE_INCONSISTENT = 0x00000115 ;

		internal const uint CKS_RO_PUBLIC_SESSION = 0 ;
		internal const uint CKS_RO_USER_FUNCTIONS = 1 ;
		internal const uint CKS_RW_PUBLIC_SESSION = 2 ;
		internal const uint CKS_RW_SO_FUNCTIONS = 4 ;
		internal const uint CKS_RW_USER_FUNCTIONS = 3 ;

		internal const uint CKU_CONTEXT_SPECIFIC = 2 ;
		internal const uint CKU_SO = 0 ;
		internal const uint CKU_USER = 1 ;
	}
}
