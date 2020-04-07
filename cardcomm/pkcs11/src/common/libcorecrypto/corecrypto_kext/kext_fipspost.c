/*
 * Copyright (c) 2012,2013,2014,2015 Apple Inc. All rights reserved.
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
#include "kext_fipspost.h"
#include <corecrypto/ccdigest.h>
#include <corecrypto/ccsha1.h>
#include <corecrypto/ccsha2.h>
#include <corecrypto/cchmac.h>
#include <corecrypto/ccmode.h>
#include <corecrypto/ccaes.h>
#include <corecrypto/ccdes.h>
#include <corecrypto/ccrng_system.h>
#include <corecrypto/ccec.h>
#include <corecrypto/ccdrbg.h>
#include <corecrypto/ccrsa.h>


#include <mach/mach_types.h>
#include <libkern/OSMalloc.h>
#include <mach/kmod.h>
#include <mach-o/loader.h>
#include <libkern/libkern.h>

#include <pexpert/pexpert.h>


// NOTE: For FIPS testing these test need to be made to fail
// To do so uncomment out the next line
//#define FORCE_FAIL 1

/* ==========================================================================	
		Static Declarations
   ========================================================================== */

static int Integrity_POST(kmod_info_t* pkmod, void* d, int verbose);
/*
static int AES_CBC_POST(void);
static int TDES_CBC_POST(void);
static int SHA_POST(void);
static int HMAC_POST(void);
static int ECDSA_POST(void);
static int DRBG_POST(void);
*/

static char byteMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static int byteMapLen = sizeof(byteMap);

/* Utility function to convert nibbles (4 bit values) into a hex character representation */
/* Utility function to convert nibbles (4 bit values) into a hex character representation */
static char nibbleToChar(uint8_t nibble)
{
	if(nibble < byteMapLen) return byteMap[nibble];
	return '*';
}


static unsigned char* bytesToHexString(unsigned char* bytes, unsigned long byteLength, unsigned char* buffer, unsigned long bufferLength)
{
	unsigned long i;
	
	// Ensure the output buffer is big enough
	if (byteLength > ((bufferLength - 1)/2))
	{
		return NULL;
	}
    
	// clear out the output buffer
	memset(buffer, 0, bufferLength);
	
	// loop through the bytes and fill in the output buffer
	for(i = 0; i < byteLength; i++) 
    {
		buffer[i*2] = nibbleToChar(bytes[i] >> 4);
		buffer[i*2+1] = nibbleToChar(bytes[i] & 0x0f);
	}
	
	// return the buffer pointer
	return buffer;
}


typedef struct AppleTEXTHash 
{
	int			ath_version;    // version of this structure 
	int			ath_length;     // length of hash data
	void *		ath_hash;       // hash extracted from AppleTextHashes dict 
} AppleTEXTHash_t;

static int Integrity_POST(kmod_info_t* pkmod, void* d, int verbose)
{
	int result = -1; // Set to zero for sucesses until it all works
	size_t sha256DigestBufferLength = 32;

	if (NULL == d)
	{
		if (verbose)
		{
			kprintf("The AppleTEXTHash_t pointer was NOT passed to the Integrity_POST function\n");
		}
		return result;
	}

	AppleTEXTHash_t* pHashData = (AppleTEXTHash_t*)d;
	
	if (pHashData->ath_version != 1 || pHashData->ath_length != (int)sha256DigestBufferLength)
	{
		if (verbose)
		{
			kprintf("The AppleTEXTHash_t pointer passed to Integrity_POST function, is invalid\n");
		}
		return result;
	}
	
	if (NULL == pHashData->ath_hash)
	{
		if (verbose)
		{
			kprintf("The AppleTEXTHash_t pointer passed to Integrity_POST function,has a null HASH pointer\n");
		}
		return result;
	}
	
	unsigned long plist_hash_output_buffer_size = (sha256DigestBufferLength * 2) + 1;
	unsigned char plist_hash_output_buffer[plist_hash_output_buffer_size];
	char* pPlistHexBuf = (char*)bytesToHexString(pHashData->ath_hash, pHashData->ath_length, plist_hash_output_buffer, plist_hash_output_buffer_size);
	
	if (verbose)
	{
		kprintf("Plist hmac value is    %s\n", pPlistHexBuf);
	}
	
	// Now calcuate the HMAC
    struct mach_header* pmach_header = (struct mach_header*)pkmod->address;
    
	struct load_command* pLoadCommand = NULL;

	uint32_t num_load_commands = 0;
	if (pmach_header->magic == MH_MAGIC_64)
	{
		struct mach_header_64* pmach64_header = (struct mach_header_64*)pmach_header;
		num_load_commands = pmach64_header->ncmds;		
		pLoadCommand = (struct load_command*)(((unsigned char*)pmach_header) + sizeof(struct mach_header_64));
	}
	else if (pmach_header->magic == MH_MAGIC)
	{
		num_load_commands = pmach_header->ncmds;
		pLoadCommand = (struct load_command*)(((unsigned char*)pmach_header) + sizeof(struct mach_header));
	}

	if (NULL == pLoadCommand)
	{
		if (verbose)
		{
			kprintf("pLoadCommand is NULL!\n");
		}
		return result;
	}
	
	const struct ccdigest_info* di = ccsha256_di();
    unsigned char hmac_key = 0;
    cchmac_ctx_decl(di->state_size, di->block_size, ctx);
    cchmac_init(di, ctx, 1, &hmac_key);
	int hashCreated = 0;
    unsigned long iCnt;
    unsigned long jCnt;
	
	struct segment_command* pSniffPtr = (struct segment_command*)pLoadCommand;
    
	
	// Loop through the Segments to find the __TEXT, __text segment
	for (iCnt = 0; iCnt < num_load_commands; iCnt++)
    {
        // The struct segment_command and the struct segment_command_64  have the same
        // first three fields so sniff the name by casting to a struct segment_command 

        if (strncmp("__TEXT", pSniffPtr->segname, strlen("__TEXT")))
        {
            // These are not the droids we are looking for
			// MOve the SniffPtr to the next segment;
            if (LC_SEGMENT_64 == pSniffPtr->cmd)
            {
                struct segment_command_64* pSegmentPtr = (struct segment_command_64*)pSniffPtr;
                pSniffPtr = (struct segment_command*)(((unsigned char *)pSegmentPtr) + pSegmentPtr->cmdsize);
            }
            else if (LC_SEGMENT == pSniffPtr->cmd)
            {
                pSniffPtr = (struct segment_command*)(((unsigned char *)pSniffPtr) + pSniffPtr->cmdsize);
            }
			
			// Go back to the top of the loop and look again
            continue; 
        }
        
		// Bingo! We found the __TEXT segment!
		// Deal with a 64 bit segment
        if (LC_SEGMENT_64 == pLoadCommand->cmd)
        {
            struct segment_command_64* pSegmentPtr = NULL;

            // This is a 64 bit load segment command
            pSegmentPtr = (struct segment_command_64*)pSniffPtr;
            unsigned int numSections = (unsigned int)pSegmentPtr->nsects;
            struct section_64* pSectionPtr = (struct section_64*)(((unsigned char*)pSegmentPtr) + sizeof(struct segment_command_64));
            int texttextsectionprocessed = 0;

            // Need to find the __text __TEXT section
            for (jCnt = 0; jCnt < numSections; jCnt++)
            {
                if ( !strcmp(pSectionPtr->sectname, "__text") && !strcmp(pSectionPtr->segname, "__TEXT"))
                {
                    // Found it
                    unsigned char* pSectionData = (unsigned char*)(((unsigned char*)pmach_header) + pSectionPtr->offset);

                    cchmac_update(di, ctx, (unsigned long)pSectionPtr->size, pSectionData);	
					hashCreated = 1;

                    texttextsectionprocessed = 1;
                    break;
                }
                else
                {
                    // Move to the next section record
                    pSectionPtr++;
                }
            }
            if (texttextsectionprocessed)
            {
                // The text text section was found and processed
                break;
            }
        }
		else if (LC_SEGMENT == pLoadCommand->cmd) // Deal with a 32 bit segment
		{
			struct segment_command* pSegmentPtr = NULL;

            // This is a 32 bit load segment command
            pSegmentPtr = (struct segment_command*)pLoadCommand;
            unsigned int numSections = (unsigned int)pSegmentPtr->nsects;
            struct section* pSectionPtr = (struct section*)(((unsigned char*)pSegmentPtr) + sizeof(struct segment_command));
			int texttextsectionprocessed = 0;

            // Need to find the __text __TEXT section
            for (jCnt = 0; jCnt < numSections; jCnt++)
			{
				if ( !strcmp(pSectionPtr->sectname, "__text") && !strcmp(pSectionPtr->segname, "__TEXT"))
				{
					// Found it
					unsigned char* pSectionData = (unsigned char*)(((unsigned char*)pmach_header) + pSectionPtr->offset);

					cchmac_update(di, ctx, (unsigned long)pSectionPtr->size, pSectionData);	
					hashCreated = 1;

					texttextsectionprocessed = 1;
                    break;
				}
				else
                {
                    // Move to the next section record
                    pSectionPtr++;
                }
			}
			if (texttextsectionprocessed)
			{
				// The text text section was found and processed
				// Time to bail on the loop
				break;
			}
		}
    }
	
	unsigned long hash_output_buffer_size = (sha256DigestBufferLength * 2) + 1;
	unsigned char hash_output_buffer[hash_output_buffer_size];
	unsigned char hmac_buffer[sha256DigestBufferLength];
    memset(hmac_buffer, 0, sha256DigestBufferLength);
		
	// Check to see if the hash was created 
	if (hashCreated) 
	{
		// finalize the HMAC
        
		cchmac_final(di, ctx, hmac_buffer);
	    char* pHexBuf = (char*)bytesToHexString(hmac_buffer, sha256DigestBufferLength, hash_output_buffer, hash_output_buffer_size);
		if (verbose)
		{
	    	kprintf("Computed hmac value is %s\n", pHexBuf);
		}
        
	}  
	else
	{
		if (verbose)
		{
			kprintf("Integrity_POST: WARNING! could not create the hash!\n");
		}
		return -1;
	}
	
#ifdef FORCE_FAIL
    // futz with the generated hmac
	hash_output_buffer[0] = 0;	// This will always work because it is the charter representation of the 
								// hash that is being checked.
#endif 
	result = memcmp(hash_output_buffer, plist_hash_output_buffer, hash_output_buffer_size);
	
	return result;
}



// Test the AES CBC mode
static int AES_CBC_POST()
{
	size_t key128Length = 16;
	size_t key192Length = 24;
	size_t key256Length = 32;
	
	typedef struct 
	{
		size_t				keyLength;
		int                 forEncryption;
		unsigned char*		keyData;
		unsigned char*		ivData;
		unsigned char*		inputData;
		unsigned char*		outputData;
	} testData;
	
	// AES 128 Encryption Test Data
	unsigned char* key128EncryptBuffer = (unsigned char*)"\x34\x49\x1b\x26\x6d\x8f\xb5\x4c\x5c\xe1\xa9\xfb\xf1\x7b\x09\x8c";
	unsigned char* iv128EncryptBuffer = (unsigned char*)"\x9b\xc2\x0b\x29\x51\xff\x72\xd3\xf2\x80\xff\x3b\xd2\xdc\x3d\xcc";
	unsigned char* input128EncryptBuffer = (unsigned char*)"\x06\xfe\x99\x71\x63\xcb\xcb\x55\x85\x3e\x28\x57\x74\xcc\xa8\x9d";
#ifdef FORCE_FAIL
	unsigned char* output128EncryptBuffer = (unsigned char*)"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";
#else	
	unsigned char* output128EncryptBuffer = (unsigned char*)"\x32\x5d\xe3\x14\xe9\x29\xed\x08\x97\x87\xd0\xa2\x05\xd1\xeb\x33";
#endif	
	// AES 128 Decryption Test Data
	unsigned char* key128DecryptBuffer = (unsigned char*)"\xc6\x8e\x4e\xb2\xca\x2a\xc5\xaf\xee\xac\xad\xea\xa3\x97\x11\x94";
	unsigned char* iv128DecryptBuffer = (unsigned char*)"\x11\xdd\x9d\xa1\xbd\x22\x3a\xcf\x68\xc5\xa1\xe1\x96\x4c\x18\x9b";
	unsigned char* input128DecryptBuffer = (unsigned char*)"\xaa\x36\x57\x9b\x0c\x72\xc5\x28\x16\x7b\x70\x12\xd7\xfa\xf0\xde";
	unsigned char* output128DecryptBuffer = (unsigned char*)"\x9e\x66\x1d\xb3\x80\x39\x20\x9a\x72\xc7\xd2\x96\x40\x66\x88\xf2";
    
    // AES 192 Encryption Test Data
	unsigned char* key192EncryptBuffer = (unsigned char*)"\x05\x85\xfc\xe6\xa9\x8c\x54\xd7\x2a\x4d\x7d\x53\x13\xbd\xeb\x0c\x1c\x42\xbc\xba\xf1\x68\x9b\x6b";	
	unsigned char* iv192EncryptBuffer = (unsigned char*)"\xf5\xc8\x5d\x21\x3c\xe2\x7e\xb2\xea\xf6\x29\x3c\xee\x44\x6f\x18";
	unsigned char* input192EncryptBuffer = (unsigned char*)"\x5b\xb1\x61\xa5\x8d\xa6\x18\xfc\x55\x2d\x28\x23\x21\x4b\x8e\xee";
	unsigned char* output192EncryptBuffer = (unsigned char*)"\x34\x91\xb7\xa7\x88\x67\x71\xf7\xc8\x0d\x85\x95\x4d\x6f\x7f\x28";
	
	// AES 192 Decryption Test Data
	unsigned char* key192DecryptBuffer = (unsigned char*)"\x05\x8f\x89\xc4\xc9\x09\x0c\xad\x01\xd1\xa9\x37\x7f\x3f\xea\x14\x42\x24\xea\x49\x4b\x53\xc2\xd5";
	unsigned char* iv192DecryptBuffer = (unsigned char*)"\xb8\xb1\x88\x69\x88\x18\x10\xdd\x4a\xb4\xd1\x19\x55\x94\x70\xc2";
	unsigned char* input192DecryptBuffer = (unsigned char*)"\x12\xb9\xd4\x81\xeb\x0c\x7b\xe3\xfd\x6a\xe7\x55\xc9\xde\x3f\x6f";
	unsigned char* output192DecryptBuffer = (unsigned char*)"\x7e\xb8\x86\x01\xa8\xb4\x85\x60\x5e\xbe\x7d\x88\x95\x75\xea\x94";
	
	// AES 256 Encryption Test Data
	unsigned char* key256EncryptBuffer = (unsigned char*)"\xcd\x53\x9b\x65\x93\x19\x45\xb9\xe8\x65\xbc\x80\x4c\x4b\x16\x89\x24\x29\x8c\x3e\x02\xd9\xda\x79\x27\x2b\xe5\x99\x47\x92\xa0\xd4";
	unsigned char* iv256EncryptBuffer = (unsigned char*)"\x0d\x52\xe3\x43\x57\x31\x4e\x28\x78\xa0\xca\x96\xc5\x32\xef\xd8";
	unsigned char* input256EncryptBuffer = (unsigned char*)"\x17\x55\xb7\x73\x0b\x8c\x1c\x65\x0b\x42\x13\x70\x5b\x3e\x9c\xa2";
	unsigned char* output256EncryptBuffer = (unsigned char*)"\x48\x8f\x47\x28\xa5\xac\xd5\x19\x84\x11\x0a\x23\x37\x49\x37\x30";
	
	// AES 256 Decryption Test Data	
	unsigned char* key256DecryptBuffer = (unsigned char*)"\x10\x42\x2a\x2a\x6c\x95\x41\xf4\x25\x63\x29\x88\xcb\x91\x83\x10\xab\xc5\xab\x8d\x96\x1c\x98\x27\xe4\x20\x04\xc1\xb9\xf4\xcd\xd1";
	unsigned char* iv256DecryptBuffer = (unsigned char*)"\x0b\xf1\x8e\xba\x5e\x67\x1f\x71\x3c\x06\xd7\xc3\x15\x4c\xd4\xa9";
	unsigned char* input256DecryptBuffer = (unsigned char*)"\xcc\x1b\xbc\x58\x78\xef\xf3\xde\xa7\x61\xde\x94\x5e\xf2\x50\x12";
	unsigned char* output256DecryptBuffer = (unsigned char*)"\x61\xb7\x6a\x6d\xce\xaf\xc6\xae\xc1\xc3\x8d\xf3\x4a\xab\x49\x97";
	
	testData dataToTest[] =
	{
		{key128Length, 1, key128EncryptBuffer, iv128EncryptBuffer, input128EncryptBuffer, output128EncryptBuffer},
		{key128Length, 0, key128DecryptBuffer, iv128DecryptBuffer, input128DecryptBuffer, output128DecryptBuffer},
		{key192Length, 1, key192EncryptBuffer, iv192EncryptBuffer, input192EncryptBuffer, output192EncryptBuffer},
		{key192Length, 0, key192DecryptBuffer, iv192DecryptBuffer, input192DecryptBuffer, output192DecryptBuffer},
		{key256Length, 1, key256EncryptBuffer, iv256EncryptBuffer, input256EncryptBuffer, output256EncryptBuffer},
		{key256Length, 0, key256DecryptBuffer, iv256DecryptBuffer, input256DecryptBuffer, output256DecryptBuffer}
	};
    
    const struct ccmode_cbc* mode_enc = ccaes_cbc_encrypt_mode();
	const struct ccmode_cbc* mode_dec = ccaes_cbc_decrypt_mode();
	
	
    struct {
        const struct ccmode_cbc*	enc_mode_ptr;
		const struct ccmode_cbc*	dec_mode_ptr;
    } impl[] = {
        {mode_enc, mode_dec},
    };
	
	int memCheckResult = 0;
	unsigned char outputBuffer[CCAES_BLOCK_SIZE];
	
	int numDataToTest = 6;
	int numModesToTest = 1;
	
	for (int iCnt = 0; iCnt < numDataToTest; iCnt++)
	{
		for(int jCnt = 0; jCnt < numModesToTest; jCnt++)
		{
			
			cccbc_one_shot((dataToTest[iCnt].forEncryption ? 
                            impl[jCnt].enc_mode_ptr : 
                            impl[jCnt].dec_mode_ptr), 
		                   dataToTest[iCnt].keyLength, dataToTest[iCnt].keyData,
						   dataToTest[iCnt].ivData,
						   1, /* Only 1 block */
						   dataToTest[iCnt].inputData, outputBuffer);
            
			memCheckResult = (0 == memcmp(dataToTest[iCnt].outputData, outputBuffer, CCAES_BLOCK_SIZE));
            
            
			if (!memCheckResult)
			{
				return -1;
			}
		}
	}
	return 0;		
}

static int AES_ECB_POST()
{
	size_t key128Length = 16;
	//KEY = e680747f14e8a6ee00bba6bd6557ae51
#ifdef FORCE_FAIL
	unsigned char* key_data = (unsigned char *)"\xe6\x80\x74\x7f\x14\xe8\xa6\xee\x00\xbb\xa6\xbd\x65\x57\xae\x42";
#else
	unsigned char* key_data = (unsigned char *)"\xe6\x80\x74\x7f\x14\xe8\xa6\xee\x00\xbb\xa6\xbd\x65\x57\xae\x51";
#endif 
	//PLAINTEXT = 7fea96f90fbae12a857f5c97e0cba579
	unsigned char* plaintext_data =  (unsigned char *)"\x7f\xea\x96\xf9\x0f\xba\xe1\x2a\x85\x7f\x5c\x97\xe0\xcb\xa5\x79";
	//CIPHERTEXT = 3d30e6364585461671aa671026b2ecd9
	unsigned char* ciphertext_data = (unsigned char *)"\x3d\x30\xe6\x36\x45\x85\x46\x16\x71\xaa\x67\x10\x26\xb2\xec\xd9";
	
	const struct ccmode_ecb* ecm_mode = ccaes_ecb_encrypt_mode();
	
	unsigned char output[16];
	
	ccecb_one_shot(ecm_mode, key128Length, key_data, 1, plaintext_data, output);
	
	if (memcmp(ciphertext_data, output, 16))
	{
		return -1;
	}
	
	unsigned char decrypted_output[16];
	const struct ccmode_ecb* decrypt_ecm_mode = ccaes_ecb_decrypt_mode();
	
	ccecb_one_shot(decrypt_ecm_mode, key128Length, key_data, 1, output, decrypted_output);
	
	if (memcmp(plaintext_data, decrypted_output, 16))
	{
		return -1;
	}
 	
	return 0;
}

static int AES_XTS_POST()
{
	size_t key128Length = 16;
	
#ifdef FORCE_FAIL    
	//Key = 3970cbb4b09a50f428890024876607d04f9b3621728d8a67549f74aa082d58ef
	unsigned char* key_data =  (unsigned char*)"\x39\x70\xcb\xb4\xb0\x9a\x50\xf4\x28\x89\x00\x24\x87\x66\x07\x42";
#else
	unsigned char* key_data =  (unsigned char*)"\x39\x70\xcb\xb4\xb0\x9a\x50\xf4\x28\x89\x00\x24\x87\x66\x07\xd0";
#endif

	unsigned char* key2_data = (unsigned char*)"\x4f\x9b\x36\x21\x72\x8d\x8a\x67\x54\x9f\x74\xaa\x08\x2d\x58\xef";
	// PT = 18147bb2a205974d1efd386885b24797
	unsigned char* pt_enc_data =  (unsigned char *)"\x18\x14\x7b\xb2\xa2\x05\x97\x4d\x1e\xfd\x38\x68\x85\xb2\x47\x97";
	
	// CT = b91a3884ffd4e6151c5aaaaecb5fa9ff
	unsigned char* ct_enc_data =  (unsigned char *)"\xb9\x1a\x38\x84\xff\xd4\xe6\x15\x1c\x5a\xaa\xae\xcb\x5f\xa9\xff";
	unsigned int	dataUnitSeqNumber = 41;

	int8_t         tweak_buffer[CCAES_BLOCK_SIZE];
    
    memset(tweak_buffer, 0, CCAES_BLOCK_SIZE);
    unsigned char* dataUnitSeqNumberPtr = (unsigned char*)&dataUnitSeqNumber;
    size_t numBytes = sizeof(dataUnitSeqNumber);
    for(size_t iCnt = 0; iCnt < numBytes; iCnt++)
    {
        tweak_buffer[iCnt] = (unsigned char)*dataUnitSeqNumberPtr;
        dataUnitSeqNumberPtr++;
    }

	const struct ccmode_xts* xts_enc =  ccaes_xts_encrypt_mode();
	
	unsigned char output[16];
	memset(output, 0, 16);
		
	ccxts_one_shot(xts_enc, key128Length, key_data, key2_data, tweak_buffer, 1, pt_enc_data, output);
	if (memcmp(output, ct_enc_data, 16))
	{
		return -1;
	}
	
	const struct ccmode_xts* xts_dec =  ccaes_xts_decrypt_mode();
	memset(output, 0, 16);
	ccxts_one_shot(xts_dec, key128Length, key_data, key2_data, tweak_buffer, 1, ct_enc_data, output);
	if (memcmp(output, pt_enc_data, 16))
	{
		return -1;
	}
	return 0;
}


static int TDES_CBC_POST()
{
	size_t keyLength = 24;
	
	// TDES Encryption Test Data
	unsigned char* keyEncryptBuffer = (unsigned char*)"\xc4\xdc\xcd\x01\x4f\xd9\x51\x5e\xc4\xdc\xcd\x01\x4f\xd9\x51\x5e\xc4\xdc\xcd\x01\x4f\xd9\x51\x5e";
	unsigned char* ivEncryptBuffer = (unsigned char*)"\x97\xa1\xc5\x7d\x19\x6a\xba\xf3";
	unsigned char* inputEncryptBuffer = (unsigned char*)"\x6e\x66\x27\x95\x2a\x00\x2b\x56";
#ifdef FORCE_FAIL
	unsigned char* outputEncryptBuffer = (unsigned char*)"\x00\x01\x02\x03\x04\x05\x06\x07";
#else	
	unsigned char* outputEncryptBuffer = (unsigned char*)"\x54\xe9\x0c\x47\xc2\x26\x40\x3d";
#endif
    
	// TDES Decryption Test Data
	unsigned char* keyDecryptBuffer = (unsigned char*)"\xbc\x15\xd3\xf2\x79\x5e\xb0\x64\xbc\x15\xd3\xf2\x79\x5e\xb0\x64\xbc\x15\xd3\xf2\x79\x5e\xb0\x64";
	unsigned char* ivDecryptBuffer = (unsigned char*)"\xec\x00\x68\xb4\xf6\xaf\x31\x15";
	unsigned char* inputDecryptBuffer = (unsigned char*)"\x9e\x47\x30\x78\x87\xd4\xf4\x30";
	unsigned char* outputDecryptBuffer = (unsigned char*)"\x11\x11\xe8\x06\xe7\x0a\x00\x84";
	
	unsigned char outputBuffer[CCDES_BLOCK_SIZE];
    int memCheckResult = 0; // Guilty until proven	
    
      
    const struct ccmode_cbc*  cbc_mode_dec = ccdes3_cbc_decrypt_mode();
    const struct ccmode_cbc*  cbc_mode_enc = ccdes3_cbc_encrypt_mode();
	
	// Encryption Test	
	cccbc_one_shot(cbc_mode_enc, keyLength, keyEncryptBuffer, 
                   ivEncryptBuffer, 1,  inputEncryptBuffer, outputBuffer);
    
	memCheckResult  = memcmp(outputEncryptBuffer, outputBuffer, CCDES_BLOCK_SIZE);
	
	if (!memCheckResult)
	{
        // Decryption Test
        cccbc_one_shot(cbc_mode_dec, keyLength, keyDecryptBuffer, 
                       ivDecryptBuffer, 1,  inputDecryptBuffer, outputBuffer);
        
        memCheckResult = memcmp(outputDecryptBuffer, outputBuffer, CCDES_BLOCK_SIZE);
    }
    
	return memCheckResult;
}

static int SHA_POST()
{
    int result = -1;
	size_t messageSize = 10;
    
    const struct ccdigest_info* sha1Impl = ccsha1_di();
    const struct ccdigest_info* sha224Impl = ccsha224_di();
    const struct ccdigest_info* sha256Impl = ccsha256_di();
    const struct ccdigest_info* sha384Impl = ccsha384_di();
    const struct ccdigest_info* sha512Impl = ccsha512_di();
    
    
	typedef struct
	{
		const struct ccdigest_info*  digest_info;
		unsigned char*	msgBuffer;
		unsigned char*	digestBuffer;
		size_t			digestBufferLength;	
	} digest_test;
    
	// SHA1 Test Data
	unsigned char* sha1MsgBuffer = (unsigned char*)"\xd2\x32\xb4\x7e\xa4\x52\xd5\xd7\xa0\x89";
#ifdef FORCE_FAIL
	unsigned char*	shaDigestBuffer = (unsigned char*)"\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13";
#else
	unsigned char*	shaDigestBuffer = (unsigned char*)"\xb3\x31\xfe\x2c\xda\xc9\xc5\x2b\x0f\xae\xff\xfb\x04\x09\xa7\x3a\xaf\x86\xb3\x28";
#endif

	size_t sha1DigestBufferLength = 20;
	
	// SHA224 Test Data
	unsigned char* sha224MsgBuffer = (unsigned char*)"\x1e\x3b\x6a\x3b\xdc\x28\x69\x2a\x91\xcb";
	unsigned char* sha224DigestBuffer = (unsigned char*)"\x15\x11\x75\xc0\x02\x80\x87\x3a\x2a\xc1\x62\x8d\x84\x4b\x7b\x67\x42\xa0\x39\x2d\xc5\xbd\x3a\xa7\x2b\x31\xed\x0b";
	size_t sha224DigestBufferLength = 28;
	
	// SHA256 Test Data
	unsigned char* sha256MsgBuffer = (unsigned char*)"\xbe\x3b\x88\x4a\x36\x46\x4d\x88\x14\x28";
	unsigned char* sha256DigestBuffer = (unsigned char*)"\xb3\x34\x8a\x1a\x93\x23\xbe\xef\x01\x77\x47\x99\xa2\x46\xea\xd9\x4b\xd8\x97\x41\x54\x09\xc7\xec\x3f\xb1\xa4\x5c\x62\x89\x85\xaa";
	size_t sha256DigestBufferLength = 32;
	
	// SHA384 Test Data
	unsigned char* sha384MsgBuffer = (unsigned char*)"\x23\xcb\xcb\x15\xc6\xb5\xd3\xe7\xcc\x6a";
	unsigned char* sha384DigestBuffer = (unsigned char*)"\x83\x3d\xdc\xf2\xf2\x9e\x7e\x3c\x3f\x73\x16\xa2\xdc\x10\x8c\x83\x1f\xbd\x4f\x7b\x61\x59\x53\x0d\xa9\x63\x64\xc8\xcd\x9e\xcf\xbe\x98\x9d\x7e\x97\x4b\xed\xba\xa0\x03\xa8\x2e\xd7\x06\x0b\xb3\xc8";
	size_t sha384DigestBufferLength = 48;
	
	// SHA512 Test Data
	unsigned char* sha512MsgBuffer = (unsigned char*)"\x44\xc8\x73\x00\x21\x13\x2b\x4a\x2b\x7e";
	unsigned char* sha512DigestBuffer = (unsigned char*)"\xbd\x5c\xa4\x21\xe4\x85\x8d\x3f\x1e\x11\xdf\xfb\x27\x38\xb0\x54\x7f\xec\xdb\xc1\x66\x7c\xed\x6a\x69\x44\x9b\x69\xcd\xf9\xb3\x91\x29\x55\x78\xb3\x80\xab\xde\x52\x48\x96\x6a\xd0\x7a\xf9\x5c\x77\x70\x7c\xc1\x5f\xf9\x08\xf6\xef\xb5\x9e\xf7\xc2\x45\x95\x46\x02";
	size_t sha512DigestBufferLength = 64;
    
	uint8_t outputDigestBuffer[sha512DigestBufferLength];
	memset(outputDigestBuffer, 0, sha512DigestBufferLength);
	
	digest_test testToRun[] =
	{
		// SHA1
		{sha1Impl, sha1MsgBuffer, shaDigestBuffer, sha1DigestBufferLength},
		// SHA224
		{sha224Impl, sha224MsgBuffer, sha224DigestBuffer, sha224DigestBufferLength},
		// SHA356
		{sha256Impl, sha256MsgBuffer, sha256DigestBuffer, sha256DigestBufferLength},
		// SHA384
		{sha384Impl, sha384MsgBuffer, sha384DigestBuffer, sha384DigestBufferLength},
		// SHA512
		{sha512Impl, sha512MsgBuffer, sha512DigestBuffer, sha512DigestBufferLength}
	};
	
	int numTestToRun = 5;
	for (int iCnt = 0; iCnt < numTestToRun; iCnt++)
	{
		digest_test* currentTest = &(testToRun[iCnt]);
        const struct ccdigest_info* di_ptr = currentTest->digest_info;
        ccdigest(di_ptr, messageSize, currentTest->msgBuffer, outputDigestBuffer);
        if (0 != memcmp(currentTest->digestBuffer, outputDigestBuffer, currentTest->digestBufferLength))
        {
            return result;
        }        
		
	}
	return 0;
}

static int HMAC_POST()
{
	int result = -1;	
    
	typedef struct
	{
		const struct ccdigest_info*  digest_info;
		size_t			key_size;
		unsigned char*	key_buffer;
		size_t			msg_size;
		unsigned char*	msg_buffer;
		size_t			digest_size;
		unsigned char*	digest_buffer;
	} hmac_test;
	
	
	// HMAC-SHA1 Test Data
#ifdef FORCE_FAIL
	unsigned char* hmacSha1KeyBuffer = (unsigned char*)"\x0f\x00\x00\x81\x31\x2a\x16\xfa\xc6\xa8\x1e\x98\x05\xe1\xd7\x84\x44\x5b\xff\x8c\xe6\x84\x99\xbe\x95\xeb\x4a\x36\xcb\xb0\x03\x34\x97\x3a\xe0\x72\x9e\x27\x50\xb4\x60\x89\x77\xa0\xbc\x4f\xd0\xd0\xb6\x01";
#else
	unsigned char* hmacSha1KeyBuffer = (unsigned char*)"\xee\xd8\x1b\x81\x31\x2a\x16\xfa\xc6\xa8\x1e\x98\x05\xe1\xd7\x84\x44\x5b\xff\x8c\xe6\x84\x99\xbe\x95\xeb\x4a\x36\xcb\xb0\x03\x34\x97\x3a\xe0\x72\x9e\x27\x50\xb4\x60\x89\x77\xa0\xbc\x4f\xd0\xd0\xb6\x01";
#endif
	size_t hmacSha1KeyBufferLength = 50;
	unsigned char* sha1MsgBuffer = (unsigned char*)"\xf0\x6b\xb4\x5b\xd0\x60\x58\x27\x82\x4c\xd0\x4d\xa7\x5b\x68\x7a\x86\xc9\x39\xef\xaf\xf9\xf1\x32\xdd\xc1\xd7\x04\x21\x08\x09\x94\x3d\x94\x08\xf2\x4e\x1d\x77\xc6\xaf\xa6\x20\x42\x19\x0d\x38\x55\x0f\xe0\xe4\x22\x79\x72\xfc\xb0\x8f\x2e\x0e\xe3\xf8\x2c\xa6\xab\x33\x02\xcc\x7b\x37\xdd\xcf\xfd\x56\xd0\x41\x04\x67\x6b\x43\xc2\x24\x90\x03\x3b\xd1\x82\x82\xf9\x1f\x3f\x9b\x01\x4f\x10\x41\x07\x9a\x5e\x08\xde\xd1\xc7\xe6\x32\x41\x71\x3b\x79\xd9\x9e\x10\x27\x8f\x81\x9c\x21\xff\x51\x0d\x75\x55\x9b\x85\x48\x6e\xdc\x62\x10";
	size_t sha1MsgBufferLength = 128;
	unsigned char* sha1DigestBuffer = (unsigned char*)"\x88\x6e\xae\x70\xf3\x6b\xd3\x80\x5e\xeb\x12\x74\xb3\x24\x8f\xcc\xf5\xe1\x5b\x80";
	size_t sha1DigestBufferLength = 20;	
	
	// HMAC-SHA224 Test Data
	unsigned char*  hmacSha224KeyBuffer = (unsigned char*)"\x76\x67\xba\x7e\x64\xcc\x36\x44\xa6\x8e\xa0\x3e\x54\xef\x7b\xef\xb7\xa5\xf7\xdd\x0b\x8e\xcd\xe7\x83\xa0\x6b\xd9\x8e\x1f\x51\x3e\x59\x12\xbb\x76\x95\xf7\x80\x08\x7a\xb6\x3a\x39\x0c\xd4\xa7\xdd\x9d\xe9";
	size_t hmacSha224KeyBufferLength = 50;
	unsigned char*  sha224MsgBuffer = (unsigned char*)"\xd3\x9b\x1f\xae\xa3\x0e\x5f\x64\x5b\x94\xfe\x32\x2d\xab\x54\xa3\xc6\x59\x1d\x0d\x76\xc0\xc4\xc4\x91\xf3\xe1\x8d\x15\x08\x00\x55\x8c\x6e\xa4\x23\x34\x81\x8b\x4c\x5e\x5c\x34\xe6\x14\xee\x9f\x7e\xfe\x51\x54\x0e\x87\x56\xa6\x29\x3b\x65\x39\x79\x73\x84\xc2\x6b\x35\xb5\x0e\x29\x56\x85\x46\xc9\xe1\xe2\x71\xc3\xbd\x2d\x3a\xa9\x89\x90\xf3\x12\xcb\x92\xdb\xd8\x4a\xe8\x9c\x83\xb8\x8e\x18\x04\x94\x17\x65\xa7\x4f\x42\x16\x43\xae\xcd\xbd\xb3\x6f\x8e\xae\x8a\x2f\xbf\x05\x03\x8b\x9a\xea\x37\x86\x26\x13\x93\x2a\x50\x8b\x88";
	size_t sha224MsgBufferLength = 128;
	unsigned char* sha224DigestBuffer = (unsigned char*)"\xcb\xff\xc6\x5b\x24\x8c\x87\x8c\x85\xac\xf9\x4d\x1c\x0b\xfb\x1f\xe6\x8c\x22\x85\x76\x62\xac\x20\x2e\x9b\x47\xc1";
	size_t sha224DigestBufferLength = 28;
	
	// HMAC-SHA256 Test Data
	unsigned char*  hmacSha256KeyBuffer = (unsigned char*)"\xf3\xd9\x84\x1c\x78\x2e\x1d\xc8\x4d\x5f\xbd\x95\x23\x68\x3b\x8d\x29\xd1\xd5\x85\x2d\x3a\x74\x52\x04\x38\x32\x3a\x2d\x5f\x19\xa9\xb1\xa0\x2d\x25\x4d\xd6\xad\x84\xd3\x9b\x97\x5d\xce\x60\x6a\xbc\x65\xbd";
	size_t hmacSha256KeyBufferLength = 50;
	unsigned char*  sha256MsgBuffer = (unsigned char*)"\xce\xb6\xa4\x96\x7c\xc8\x60\xd3\xb8\x7a\x53\x1c\xb2\x4f\xc5\x31\x76\x35\xbf\x80\x11\x13\x5b\x50\xf6\xa1\x3d\x40\xa0\x7c\x62\xf0\x78\x7a\x19\xfe\xf8\x3a\x4e\x34\x11\x00\x0e\xff\xca\xc0\x48\x23\x2b\x79\xd1\xae\x59\xc5\xab\x2a\x02\xad\x87\x17\xfb\xc1\x88\x99\x28\x69\x4a\x6d\x9d\x76\x23\x21\x02\xfc\xa9\x85\x3c\x64\x74\x5d\x4a\xbd\x25\x58\x6c\x53\xa6\x46\x8b\x83\xb4\x85\xd5\xcd\x9b\xbc\xa8\x2b\x41\xcc\xb1\xa1\x66\x04\x55\x16\x2a\x95\x4f\x62\xd0\x45\x9b\xa8\xc1\x67\x93\xe6\xd4\x0a\x59\xca\xcc\x71\x74\xc8\x23\xc3";
	size_t sha256MsgBufferLength = 128;
	unsigned char*  sha256DigestBuffer = (unsigned char*)"\xc1\xcc\xf9\x1e\x8e\xa5\x8a\x16\x75\x6b\xe8\xe4\x08\xf3\x08\xc9\xe1\x02\xea\xe3\x54\x4f\xb6\x36\xdb\x18\x29\x34\x80\xae\xd1\xe1";
	size_t sha256DigestBufferLength = 32;
	
	// HMAC-SHA384 Test Data
	unsigned char*  hmacSha384KeyBuffer = (unsigned char*)"\xc6\xc9\xe0\x68\xd4\x39\x02\xee\xc3\x2b\x2f\x9f\x48\x0e\x69\x83\x1e\x47\xd2\x4b\x03\x26\xca\xe2\x03\x19\x58\x44\xf8\x20\xcb\x93\x52\x81\xa1\x68\x14\xbd\xa7\x67\x1f\xc2\x62\x16\xb3\x62\xae\xef\x33\x04\xa5\xb3\x28\x9a\xc2\x02\x89\xd3\x46\x29\x3a\x91\xdb\x3b\xd2\x9d\x78\x63\xa1\x9a\x84\x78\xf2\x37\x3e\xd2\x8d\xa9\x5c\x5c\x80\x59\xf2\xeb\x1f\xd9\x96\xce\x4a\x1d\xc2\xa7\xee\xe8\x7a\x79\xce\x84\x2e\x01\x84\xb6\xe1\x4a\xc2\xf1\x8b\x7c\xde\xca";
	size_t hmacSha384KeyBufferLength = 110;
	unsigned char* sha384MsgBuffer =  (unsigned char*)"\xd3\x69\xd5\x7c\x74\xb3\xac\x14\x97\xca\x2b\x8b\x92\xfb\x7a\x3d\xe0\x20\x17\x7e\x19\xc7\x6d\xda\x21\x90\xa9\xa1\xa0\xd7\xc9\xa4\xb3\x93\xf5\x39\xe2\x7e\x13\x85\x51\x77\x96\xfa\x3a\x24\x96\x1e\x9a\xdb\x26\x8e\x2b\xc3\xe2\x43\x93\x36\xc7\x15\xdb\x6a\x48\x7d\x22\x51\x9d\x9f\x93\xbd\x5f\x88\x94\xc8\x52\xb4\x40\x73\x82\xd3\x18\x8d\x91\xcc\xf5\xd7\x4f\x03\x42\x65\x8b\xd8\x67\x47\x29\x70\x88\x69\x75\xb7\x70\xb8\xb7\xa5\xc8\x86\x08\xc1\x8d\x30\x63\xe4\xc1\xfe\xff\x40\x60\x4a\xdc\xa0\x95\xe5\x9c\xf1\x2e\xb6\x95\x02";
	size_t sha384MsgBufferLength = 128;
	unsigned char* sha384DigestBuffer = (unsigned char*)"\x01\x96\x0e\xcb\xda\xdb\x6c\x2f\x6a\xcd\xeb\x4c\x90\xb8\x74\x4c\x15\xe8\x17\x3e\x84\x2b\x47\x33\x7d\x9c\xf2\x28\x3c\x37\x9c\x6f\xca\x93\xe6\x00\x3c\x80\x3c\xcd\x7c\x75\x30\xb7\xdc\x4a\x7f\x7a";
	size_t sha384DigestBufferLength = 48;
	
	// HMAC-SHA512 Test Data
	unsigned char*  hmacSha512KeyBuffer = (unsigned char*)"\x14\x62\x37\xf2\xc2\x3c\x3c\x65\x22\x30\x60\x33\x44\x60\x4f\x29\x56\x92\xc7\xe0\x18\xd5\xa6\x88\x90\x66\x48\xe3\xfe\xd4\x0f\x94\x12\x63\x43\x9a\x5d\xae\x1d\x18\x06\x91\x1e\x57\xbe\x46\xa5\x0d\xf5\xce\xc1\xe0\x4e\x93\x44\x26\x51\x4b\x19\xef\x61\x1f\xd7\x45\xed\x0d\x95\xb2\xea\x8b\x01\x03\x80\x6d\xac\x4c\x03\x08\xa8\x2c\x26\x98\x55\x52\x6b\xdf\x78\x40\xde\x0f\x8d\x4f\x03\xea\x5f\xf4\x0d\x28\xd5\x41\x50\x19\x0f\xaf\xf9\x8b\xb1\x1a\xfd\xee";
	size_t hmacSha512KeyBufferLength = 110;
	unsigned char* sha512MsgBuffer = (unsigned char*)"\x94\x14\x7a\x46\xd6\x7e\xef\x0b\x5e\x9f\x99\x24\x0a\xf4\xd3\x14\xa3\x30\x4d\x58\x02\xd6\x54\x9a\x77\x06\x54\x27\x6b\x97\x9b\xa3\x43\x67\x5f\xfc\x88\xef\x03\x9c\x45\x1b\x96\xe3\xb1\x58\x9d\xd4\x0e\xee\x12\x0e\xd1\x90\xac\xfe\x03\x31\x61\x54\xe7\x1a\x2a\xb3\xdd\x3a\x06\xad\x86\xbd\x41\xee\x29\xe0\xf6\xb7\x56\x03\xd3\x8f\xc9\xff\x1c\x35\x90\xbb\xf2\xd8\x71\xfa\xd6\x8a\x86\x77\x69\xf3\x2a\x34\x75\x42\x19\x0f\x31\x69\xd2\x96\xc1\x8a\x40\x1c\xfd\xac\x9a\x0d\x73\xb3\x88\x7e\xaf\x8f\x87\x08\xba\xb3\x8e\xd7\xe0\xc9";
	size_t sha512MsgBufferLength = 128;
	unsigned char* sha512DigestBuffer = (unsigned char*)"\x57\x05\xbf\xd5\x85\x4b\x7c\xc3\xb7\xc8\xea\xca\x32\x41\x88\x40\xae\xde\x68\xe7\xcd\x62\x1e\x43\x8f\x6d\x61\x58\x3b\x45\x61\xfc\xa5\x47\x19\x04\xbd\x79\x22\x89\x59\xa3\x90\x86\x4c\x8a\x70\xa5\x30\x69\xda\x41\x72\xca\x34\xc0\xea\x49\x6e\x12\x8d\xcd\xb8\xc3";
	size_t sha512DigestBufferLength = 64;
    
	uint8_t outputDigestBuffer[sha512DigestBufferLength];
	memset(outputDigestBuffer, 0, sha512DigestBufferLength);
    
    const struct ccdigest_info* sha1Impl = ccsha1_di();
    const struct ccdigest_info* sha224Impl = ccsha224_di();
    const struct ccdigest_info* sha256Impl = ccsha256_di();
    const struct ccdigest_info* sha384Impl = ccsha384_di();
    const struct ccdigest_info* sha512Impl = ccsha512_di();
    
	
	hmac_test testToRun[] =
	{
		{sha1Impl,  hmacSha1KeyBufferLength, hmacSha1KeyBuffer, sha1MsgBufferLength, sha1MsgBuffer, sha1DigestBufferLength, sha1DigestBuffer},
		{sha224Impl,  hmacSha224KeyBufferLength, hmacSha224KeyBuffer, sha224MsgBufferLength, sha224MsgBuffer, sha224DigestBufferLength, sha224DigestBuffer},
		{sha256Impl,  hmacSha256KeyBufferLength, hmacSha256KeyBuffer, sha256MsgBufferLength, sha256MsgBuffer, sha256DigestBufferLength, sha256DigestBuffer},
		{sha384Impl,  hmacSha384KeyBufferLength, hmacSha384KeyBuffer, sha384MsgBufferLength, sha384MsgBuffer, sha384DigestBufferLength, sha384DigestBuffer},
		{sha512Impl,  hmacSha512KeyBufferLength, hmacSha512KeyBuffer, sha512MsgBufferLength, sha512MsgBuffer, sha512DigestBufferLength, sha512DigestBuffer}		
	};
	
	int numTestToRun = 5;
	for (int iCnt = 0; iCnt < numTestToRun; iCnt++)
	{
		hmac_test* currentTest = &(testToRun[iCnt]);
        const struct ccdigest_info* di_ptr = currentTest->digest_info;
        cchmac(di_ptr, currentTest->key_size, currentTest->key_buffer, 
               currentTest->msg_size, currentTest->msg_buffer, outputDigestBuffer);
        
        if (memcmp(outputDigestBuffer, currentTest->digest_buffer, currentTest->digest_size))
        {
            return result;
        }
        
    }
    
	return 0;
}

static int ECDSA_POST()
{
	// Pair wise consistency test
	size_t keySize = 256; 
	int iResult = 0;

	struct ccrng_system_state system_rng;
	struct ccrng_state *rng = (struct ccrng_state *)&system_rng;
    ccrng_system_init(&system_rng);
	
	ccec_const_cp_t cp = ccec_get_cp(keySize);
	ccec_full_ctx_decl_cp(cp, full_ec_key);
    size_t signedDataLen = ccec_sign_max_size(cp);
    uint8_t signedData[signedDataLen];
    cc_zero(signedDataLen, signedData);
	
    iResult = ccec_generate_key_fips(cp, rng, full_ec_key);
	ccec_full_ctx_clear_cp(cp, full_ec_key);
    ccrng_system_done(&system_rng);
	return iResult;
}

static int DRBG_POST()
{
	int result = -1;

#ifdef FORCE_FAIL 	
	unsigned char*  entropyInputBuffer = (unsigned char*)"\x0f\x00\x00\x1f\x3d\xb3\x31\x52\x9a\x13\xc3\x6d\xc6\xeb\xd2\xef";
#else
	unsigned char*  entropyInputBuffer = (unsigned char*)"\x74\x7a\xe6\x1f\x3d\xb3\x31\x52\x9a\x13\xc3\x6d\xc6\xeb\xd2\xef";
#endif
	size_t entropyInputBufferLength = 16;
	unsigned char* nonceBuffer = (unsigned char *)"\xff\xbd\xdc\xdf\x7f\xdd\xce\xa4";
	size_t nonceBufferLength = 8;
	unsigned char* personalizationStringBuffer = (unsigned char *)"\xbd\x93\xc6\xd5\x6b\x07\x7b\xf3\xca\x13\x0c\xc3\xef\xbf\xc7\x10";
	size_t personalizationStringBufferLength = 16;
	unsigned char* additionalInput1Buffer = (unsigned char *)"\xdf\xb1\xe7\x83\x82\xc8\xdb\xd7\xef\x1a\x20\x0b\x13\x67\x1a\xe2";
	size_t additionalInput1BufferLength = 16;
	unsigned char* entropyInputPR1Buffer = (unsigned char *)"\x34\x83\x2e\xc3\x2b\x10\x58\xc9\x8d\x72\xb0\xb6\x89\xa8\xda\xe2";
	size_t entropyInputPR1BufferLength = 16;
	unsigned char* additionalInput2Buffer = (unsigned char *)"\xca\x83\xd6\x45\x5e\x98\xcd\x09\xd6\x65\x86\xe2\x63\x92\x6d\xe6";
	size_t additionalInput2BufferLength = 16;
	unsigned char* entropyInputPR2Buffer = (unsigned char *)"\xbe\xe1\x92\xef\x26\xdd\xbb\x23\x6a\xf8\x29\xd0\xc7\xd8\x49\xb7";
	size_t entropyInputPR2BufferLength = 16;
	unsigned char* returnedBitsBuffer = (unsigned char *)"\x52\x58\xdd\xef\x4b\xda\x42\xed\x49\x9e\x57\xf1\x51\x74\xb0\x87";
	size_t returnedBitsBufferLength = 16;
	
	uint8_t resultBuffer[16];
	memset(resultBuffer, 0, 16);
    
    static struct ccdrbg_info info;
    
 	struct ccdrbg_nistctr_custom custom;
    
    
	const struct ccmode_ecb* ecbMode = ccaes_ecb_encrypt_mode();
	
    custom.ecb = ecbMode;
    custom.keylen = 16;
    // Here is an oddity.  In order to test the DRBG for CAVS the DRBG must NOT be in FIPS mode.  
    // You gotta love that!
    custom.strictFIPS = 0;
    custom.use_df = 1;
    
    ccdrbg_factory_nistctr(&info, &custom);
    
    uint8_t state[info.size];
    struct ccdrbg_state* rng = (struct ccdrbg_state *)state;
    int rc;
    
    rc = info.init(&info, rng, entropyInputBufferLength, entropyInputBuffer,
                   nonceBufferLength, nonceBuffer, personalizationStringBufferLength, personalizationStringBuffer);
    if (rc)
    {
        return result; 
    }
    
    rc = ccdrbg_reseed(&info, rng, entropyInputPR1BufferLength, entropyInputPR1Buffer,
                       additionalInput1BufferLength, additionalInput1Buffer);
    if (rc)
    {
        return result; 
    }
    
    rc = ccdrbg_generate(&info, rng, 16, resultBuffer, 0, NULL);
    if (rc)
    {
        return result; 
    }	
    
    rc = ccdrbg_reseed(&info, rng, 
                       entropyInputPR2BufferLength, entropyInputPR2Buffer,  
                       additionalInput2BufferLength, additionalInput2Buffer);
    if (rc)
    {
        return result; 
    }
    
    rc = ccdrbg_generate(&info, rng, 16, resultBuffer, 0, NULL);
    if (rc)
    {
        return result; 
    }
    
    result  = memcmp(resultBuffer, returnedBitsBuffer, returnedBitsBufferLength); 
    return result;		
}

static int RSA_POST()
{
	int result = -1;
	uint32_t uintEValue = 3;

	// xp1 = 1384167f9844865eae22cb3672
	unsigned char* xp1Data = (unsigned char*)"\x13\x84\x16\x7f\x98\x44\x86\x5e\xae\x22\xcb\x36\x72";
	size_t xp1DataSize =  13;

	// xp2 = 1a085b0b737f842a8a1f32b662
	unsigned char* xp2Data = (unsigned char*)"\x1a\x08\x5b\x0b\x73\x7f\x84\x2a\x8a\x1f\x32\xb6\x62";
	size_t xp2DataSize = 13;

	// Xp = beef5ad133e9a3955097c8d8b03bd50662b5f82b8e9c3eab5c8d9d3311c337ef7ce8ddfe902bd2235293d2bdf69353f944de0b46417cb2090c1e099206af1b4
	unsigned char* xpData = (unsigned char*)"\xbe\xef\x5a\xd1\x33\xe9\xa3\x95\x50\x97\xc8\xd8\xb0\x3b\xd5\x06\x62\xb5\xf8\x2b\x8e\x9c\x3e\xab\x5c\x8d\x9d\x33\x11\xc3\x37\xef\x7c\xe8\xdd\xfe\x90\x2b\xd2\x23\x52\x93\xd2\xbd\xf6\x93\x53\xf9\x44\xde\x0b\x46\x41\x7c\xb2\x09\x0c\x1e\x09\x92\x06\xaf\x1b\x04";
	size_t xpDataSize = 64;

	// xq1 = 17fa0d7d2189c759b0b8eb1d18
	unsigned char* xq1Data = (unsigned char*)"\x17\xfa\x0d\x7d\x21\x89\xc7\x59\xb0\xb8\xeb\x1d\x18";
	size_t xq1DataSize = 13;

	// xq2 = 17c8e735fb8d58e13a412ae214
	unsigned char* xq2Data = (unsigned char*)"\x17\xc8\xe7\x35\xfb\x8d\x58\xe1\x3a\x41\x2a\xe2\x14";
	size_t xq2DataSize = 13;

	// Xq = f2d7b992fb914cd677876bb3702b1441716ebd2b447c3a0500a6e0e0449feb1cbdec1d7eee96a88230224ef3f7c2c7b858cd63f1c86df0432798de6ffd41a12a
	unsigned char* xqData = (unsigned char*)"\xf2\xd7\xb9\x92\xfb\x91\x4c\xd6\x77\x87\x6b\xb3\x70\x2b\x14\x41\x71\x6e\xbd\x2b\x44\x7c\x3a\x05\x00\xa6\xe0\xe0\x44\x9f\xeb\x1c\xbd\xec\x1d\x7e\xee\x96\xa8\x82\x30\x22\x4e\xf3\xf7\xc2\xc7\xb8\x58\xcd\x63\xf1\xc8\x6d\xf0\x43\x27\x98\xde\x6f\xfd\x41\xa1\x2a";
	size_t xqDataSize = 64;


    cc_unit x_p1[ccn_nof_size(xp1DataSize)];
    cc_unit x_p2[ccn_nof_size(xp2DataSize)];
    cc_unit x_p[ccn_nof_size(xpDataSize)];
    cc_unit x_q1[ccn_nof_size(xq1DataSize)];
    cc_unit x_q2[ccn_nof_size(xq2DataSize)];
    cc_unit x_q[ccn_nof_size(xqDataSize)];
	cc_unit e_value[1];

    size_t nbits = xpDataSize * 8 + xqDataSize * 8; // or we'll add this as a parameter.  This appears to be correct for FIPS
    cc_size n = ccn_nof(nbits);

    e_value[0] = (cc_unit)uintEValue;

    if (0 != ccn_read_uint(ccn_nof_size(xp1DataSize), x_p1, xp1DataSize, xp1Data))
	{
		return result;
	}

	if (0 != ccn_read_uint(ccn_nof_size(xp2DataSize), x_p2, xp2DataSize, xp2Data))
	{
		return result;
	}

	if (0 != ccn_read_uint(ccn_nof_size(xpDataSize), x_p, xpDataSize, xpData))
	{
		return result;
	}

	if (0 != ccn_read_uint(ccn_nof_size(xq1DataSize), x_q1, xq1DataSize, xq1Data))
	{
		return result;
	}

	if (0 != ccn_read_uint(ccn_nof_size(xq2DataSize), x_q2, xq2DataSize, xq2Data))
	{
		return result;
	}

	if (0 != ccn_read_uint(ccn_nof_size(xqDataSize), x_q, xqDataSize, xqData))
	{
		return result;
	};

    cc_size np = n;
    cc_size nq = n;
    cc_size nm = n;
    cc_size nd = n;
    cc_unit p[n];
    cc_unit q[n];
    cc_unit m[n];
    cc_unit d[n];

    ccrsa_full_ctx_decl(ccn_sizeof_n(n), full_key);
    ccrsa_ctx_n(full_key) = n;


    if (0 != ccrsa_make_931_key(nbits, 1, e_value,
                                ccn_nof_size(xp1DataSize), x_p1,
                                ccn_nof_size(xp2DataSize), x_p2,
                                ccn_nof_size(xpDataSize), x_p,
                                ccn_nof_size(xq1DataSize), x_q1,
                                ccn_nof_size(xq2DataSize), x_q2, 
                                ccn_nof_size(xqDataSize), x_q,
                                full_key,
                                &np, p,
                                &nq, q,
                                &nm, m,
                                &nd, d))
	{
        ccrsa_full_ctx_clear(ccn_sizeof(nbits), full_key);
		return result;
	}

	ccrsa_full_ctx *fk = full_key;
	ccrsa_pub_ctx_t pub_key = ccrsa_ctx_public(fk);


	unsigned char fake_digest[20];
	memcpy(fake_digest, "ABCEDFGHIJKLMNOPRSTU", 20);
	uint8_t sig[(nbits+7)/8];
    size_t siglen=sizeof(sig);

	if (0 != ccrsa_sign_pkcs1v15(full_key, ccoid_sha1, CCSHA1_OUTPUT_SIZE, fake_digest,  &siglen, sig))
	{
		ccrsa_full_ctx_clear(ccn_sizeof(nbits), full_key);
		return result;
	}

	bool ok;
	if (0 != ccrsa_verify_pkcs1v15(pub_key, ccoid_sha1, CCSHA1_OUTPUT_SIZE, fake_digest, siglen, sig, &ok) || !ok)
	{
		ccrsa_full_ctx_clear(ccn_sizeof(nbits), full_key);
		return result;
	}

	return 0;		
}

int KEXT_FIPSPost(kmod_info_t* pkmod, void* d, int verbose)
{
	int result = 0;	
	verbose = 1;
	
/* --------------------------------------------------------------------------
	Integrity test: Tests to see if the module has been tampered.
   -------------------------------------------------------------------------- */
	result = Integrity_POST(pkmod, d, verbose);
	if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS integrity POST test failed\n");
		}
		
#ifndef FORCE_FAIL 
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS integrity POST test passed!\n");
	}	

/* --------------------------------------------------------------------------
	Test for AES in CBC mode
   -------------------------------------------------------------------------- */				
    result = AES_CBC_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS AES CBC POST test failed\n");
		}
#ifndef FORCE_FAIL 
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS AES CBC POST test passed!\n");
	}
	
 /* --------------------------------------------------------------------------
	Test for Triple DES in CBC mode
   -------------------------------------------------------------------------- */				
    result = TDES_CBC_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS TDES CBC POST test failed\n");
		}
#ifndef FORCE_FAIL 
		return result;
#endif
	}
	else if (verbose)
	{
		
		kprintf("corecrypto.kext FIPS TDES CBC POST test passed!\n");
	}

/* --------------------------------------------------------------------------
	Test for AES in ECB mode
   -------------------------------------------------------------------------- */				
	result = AES_ECB_POST();
	if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS AES ECB POST test failed\n");
		}
#ifndef FORCE_FAIL 
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS AES ECB POST test passed!\n");
	}

/* --------------------------------------------------------------------------
	Test for AES in XTS mode
   -------------------------------------------------------------------------- */				
	result = AES_XTS_POST();
	if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS AES XTS POST test failed\n");
		}
#ifndef FORCE_FAIL 
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS AES XTS POST test passed!\n");
	}	

 /* --------------------------------------------------------------------------
	Test for SHA digests
   -------------------------------------------------------------------------- */				
    result = SHA_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS SHA POST test failed\n");
		}
#ifndef FORCE_FAIL
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS SHA POST test passed!\n");
	}
        
 /* --------------------------------------------------------------------------
	Test for HMACs
   -------------------------------------------------------------------------- */				
    result = HMAC_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS HMAC POST test failed\n");
		}
#ifndef FORCE_FAIL
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS HMAC POST test passed!\n");
	}

/* --------------------------------------------------------------------------
	Test for ECDSA (Eliptical Curve)
   -------------------------------------------------------------------------- */				
    result = ECDSA_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS ECDSA POST test failed\n");
		}
#ifndef FORCE_FAIL
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS ECDSA POST test passed!\n");
	}

/* --------------------------------------------------------------------------
	Test for DRBG (Deterministic Random Bit Generator)
   -------------------------------------------------------------------------- */    
    result = DRBG_POST();
    if (result)
	{
		if (verbose)
		{
			kprintf("corecrypto.kext FIPS DRBG POST test failed\n");
		}
#ifndef FORCE_FAIL
		return result;
#endif
	}
	else if (verbose)
	{
		kprintf("corecrypto.kext FIPS DRBG POST test passed!\n");
	}
    
/* --------------------------------------------------------------------------
    Test for RSA
    -------------------------------------------------------------------------- */
    result = RSA_POST();
    if (result)
    {
        if (verbose)
		{
			kprintf("corecrypto.kext FIPS RSA POST test failed\n");
		}
#ifndef FORCE_FAIL
		return result;
#endif
    }
    else if (verbose)
    {
       kprintf("corecrypto.kext FIPS RSA POST test passed!\n"); 
    }

/* --------------------------------------------------------------------------
	If the code gets here all the tests have passed and the kext may set up
	the crypto KPIs
   -------------------------------------------------------------------------- */    	
	if (verbose)
	{
		kprintf("corecrypto.kext FIPS POST passed!\n");
	}
    return 0;
}
