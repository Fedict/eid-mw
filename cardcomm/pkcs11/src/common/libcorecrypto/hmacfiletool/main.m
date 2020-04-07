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


#import <Foundation/Foundation.h>

#import <mach/mach_types.h>
#import <mach/kmod.h>
#import <mach-o/loader.h>
#import <mach/machine.h>
#import <mach-o/fat.h>
#import <mach-o/swap.h>
#import <corecrypto/ccdigest.h>
#import <corecrypto/ccsha2.h>
#import <corecrypto/cchmac.h>


/* ==========================================================================
	The HMAcFileTool is run as part of the creation of the corecrypto
	kext for BOTH OSX and iOS.  The tool itself is always an OSX command
	line tool that runs during the build process for creating the kext.

	When building for iOS I need to be able to distinguish between 
	different CPU and CPU subtypes. So the correct "slice" can be chosen
	and written out so that when the device is run the correct slice is
	matched and the HMACs will be the same.  

	Unfortunatley, the machine.h file that used to build the tool will 
	always be the OSX file and that file does NOT include the ARM types
	needed to determine the corrent slice name.  So I have copied the 
	necessary defines from the iOS machine.h file and I check for them
	here and if necessary define them.  This of course is not the best
	way to go but for now it is what needs to be done.
   ========================================================================== */
#ifndef CPU_ARCH_ABI64
#define CPU_ARCH_ABI64	0x01000000
#endif

#ifndef CPU_TYPE_ARM
#define CPU_TYPE_ARM	((cpu_type_t) 12)
#endif

#ifndef CPU_SUBTYPE_ARM_V7S
#define CPU_SUBTYPE_ARM_V7S		((cpu_subtype_t) 11)
#endif

#ifndef CPU_SUBTYPE_ARM_V7K
#define CPU_SUBTYPE_ARM_V7K		((cpu_subtype_t) 12)
#endif

#ifndef CPU_TYPE_ARM64
#define CPU_TYPE_ARM64  (CPU_TYPE_ARM | CPU_ARCH_ABI64)
#endif

#ifndef CPU_SUBTYPE_ARM64_ALL
#define CPU_SUBTYPE_ARM64_ALL           ((cpu_subtype_t) 0)
#endif

#ifndef CPU_SUBTYPE_ARM64_V8
#define CPU_SUBTYPE_ARM64_V8            ((cpu_subtype_t) 1)
#endif

/* ==========================================================================
	End stolen defines from the iOS machine.h file
   ========================================================================== */

static NSString* kAppleTextHashesKey = @"AppleTextHashes";
//#define VERBOSE_LOGGING 1

static char byteMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static int byteMapLen = sizeof(byteMap);

/* Utility function to convert nibbles (4 bit values) into a hex character representation */
static char
nibbleToChar(uint8_t nibble)
{
	if(nibble < byteMapLen) return byteMap[nibble];
	return '*';
}


static unsigned char* bytesToHexString(const unsigned char* bytes, unsigned long byteLength, unsigned char* buffer, unsigned long bufferLength)
{
	printf("bytesToHexString called with bytelength of %lu and bufferLength of %lu\n", byteLength, bufferLength);
	unsigned long i;
	
	// Ensure the output buffer is big enough
	if (byteLength > ((bufferLength - 1)/2))
	{
        printf("Bailing on bytesToHexString buffer mismatch\n");
		return NULL;
	}
    
    printf("clearing the output buffer\n");
	// clear out the output buffer
	memset(buffer, 0, bufferLength);
	
    printf("Looping through the characters\n");
	// loop through the bytes and fill in the output buffer
	for(i = 0; i < byteLength; i++) 
    {
		buffer[i*2] = nibbleToChar(bytes[i] >> 4);
		buffer[i*2+1] = nibbleToChar(bytes[i] & 0x0f);
	}
	
	// return the buffer pointer
	printf("bytesToHexString returning pointer\n");
	return buffer;
}

static NSString* get_cpu_key(cpu_type_t cpuType, cpu_subtype_t cpusubtype)
{
	NSString* result = nil;
#ifdef VERBOSE_LOGGING
	NSLog(@"In get_cpu_key: cpuType = %d cpusubtype = %d", cpuType, cpuType);
#endif
    
    switch (cpuType)
    {
        case CPU_TYPE_X86_64:
        {
#ifdef VERBOSE_LOGGING
            NSLog(@"In get_cpu_key: setting to x86_64");
#endif
            result =  @"x86_64";
        }
        break;
            
        case CPU_TYPE_I386:
        {
#ifdef VERBOSE_LOGGING
            NSLog(@"In get_cpu_key: setting to I386");
#endif
            result =  @"i386";
        }
        break;
    
        case CPU_TYPE_ARM:
        {
            switch (cpusubtype)
            {
                case CPU_SUBTYPE_ARM_V7:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"In get_cpu_key: setting to armv7");
#endif
                    result =  @"armv7";
                }
                break;
                    
                case CPU_SUBTYPE_ARM_V7S:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"In get_cpu_key: setting to armv7s");
#endif
                    result = @"armv7s";
                }
                break;

				case CPU_SUBTYPE_ARM_V7K:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"In get_cpu_key: setting to armv7k");
#endif
                    result = @"armv7k";
                }
				break;
					
                default:
                {
                    
#ifdef VERBOSE_LOGGING
                    NSLog(@"Unknown cpusubtype for CPU_TYPE_ARM");
#endif
                }
                break;
            }
        }
        break;
    
        case CPU_TYPE_ARM64:
        {
            switch(cpusubtype)
            {
                case CPU_SUBTYPE_ARM64_ALL:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"In get_cpu_key: setting to arm64 for subtype ALL");
#endif
                    result = @"arm64";
                }
                break;
                    
                case CPU_SUBTYPE_ARM64_V8:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"In get_cpu_key: setting to arm64 for subtype v8");
#endif
                    // Need to ensure the slice name is the same
                    result = @"arm64";
                }
                break;
                    
                default:
                {
#ifdef VERBOSE_LOGGING
                    NSLog(@"Unknown cpusubtype for CPU_TYPE_ARM64");
#endif
                }
                break;
            }
        }
    
        default:
        {
#ifdef VERBOSE_LOGGING
            NSLog(@"Unknown cpuType");
#endif
        }
        break;
    }
    
	return result;	
}

static BOOL get_hmac_value(const struct mach_header* pmach_header, unsigned char* hmac_buffer, NSString** keyValue)
{
    unsigned int iCnt;
	unsigned int jCnt;
    
	BOOL result = NO; 
	
	if (NULL == pmach_header || NULL == hmac_buffer || NULL == keyValue)
	{
		return result;
	}
    
    *keyValue = NULL;
	
	*keyValue = get_cpu_key(pmach_header->cputype, (pmach_header->cpusubtype /*& ~CPU_SUBTYPE_MASK*/));
    
    if (NULL == *keyValue)
    {
        return result;
    }
    
	const struct load_command* pLoadCommand = NULL;

	uint32_t num_load_commands = 0;
    printf("pmach_header->magic = %x\n", pmach_header->magic);
    
	if ((pmach_header->magic == MH_MAGIC_64) || (pmach_header->magic == MH_CIGAM_64))
	{
		const struct mach_header_64* pmach64_header = (const struct mach_header_64*)pmach_header;
		num_load_commands = pmach64_header->ncmds;		
		pLoadCommand = (const struct load_command*)(((const unsigned char*)pmach_header) + sizeof(struct mach_header_64));
	}
	else if ((pmach_header->magic == MH_MAGIC) || (pmach_header->magic == MH_CIGAM))
	{
		num_load_commands = pmach_header->ncmds;
		pLoadCommand = (const struct load_command*)(((const unsigned char*)pmach_header) + sizeof(struct mach_header));
	}

	if (NULL == pLoadCommand)
	{
        return false;
	}

    const struct ccdigest_info* di = &ccsha256_ltc_di; //ccsha256_di();
    unsigned char hmac_key = 0;
    cchmac_ctx_decl(di->state_size, di->block_size, ctx);
    cchmac_init(di, ctx, 1, &hmac_key);
	int hashCreated = 0;

    const struct segment_command* pSniffPtr = (const struct segment_command*)pLoadCommand;

    for (iCnt = 0; iCnt < num_load_commands; iCnt++)
    {
        // The struct segment_command and the struct segment_command_64  have the same
        // first three fields so sniff the name by casting to a struct segment_command 

        if (strncmp("__TEXT", pSniffPtr->segname, strlen("__TEXT")))
        {
            // These are not the droids we are looking for
            if (LC_SEGMENT_64 == pSniffPtr->cmd)
            {
                const struct segment_command_64* pSegmentPtr = (const struct segment_command_64*)pSniffPtr;
                pSniffPtr = (const struct segment_command*)(((const unsigned char *)pSegmentPtr) + pSegmentPtr->cmdsize);
            }
            else if (LC_SEGMENT == pSniffPtr->cmd)
            {
                pSniffPtr = (const struct segment_command*)(((const unsigned char *)pSniffPtr) + pSniffPtr->cmdsize);
            }

            continue; 
        }

		// Deal with a 64 bit segment
        if (LC_SEGMENT_64 == pLoadCommand->cmd)
        {
            const struct segment_command_64* pSegmentPtr = NULL;

            // This is a 64 bit load segment command
            pSegmentPtr = (const struct segment_command_64*)pSniffPtr;
            unsigned int numSections = (unsigned int)pSegmentPtr->nsects;
            const struct section_64* pSectionPtr = (const struct section_64*)(((const unsigned char*)pSegmentPtr) + sizeof(struct segment_command_64));
            int texttextsectionprocessed = 0;

            // Need to find the __text __TEXT section
            for (jCnt = 0; jCnt < numSections; jCnt++)
            {
                if ( !strcmp(pSectionPtr->sectname, "__text") && !strcmp(pSectionPtr->segname, "__TEXT"))
                {
                    // Found it
                    const unsigned char* pSectionData = (((const unsigned char*)pmach_header) + pSectionPtr->offset);


                    cchmac_update(di, ctx, (unsigned long)pSectionPtr->size, pSectionData);	
					hashCreated = 1;

                    printf("Found the __TEXT, __text and hashed it\n");
                    //for (int lCnt = 0; lCnt < 0x7FFFFF; lCnt++) ;
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
		else if (LC_SEGMENT == pLoadCommand->cmd)
		{
			const struct segment_command* pSegmentPtr = NULL;

            // This is a 32 bit load segment command
            pSegmentPtr = (const struct segment_command*)pLoadCommand;
            unsigned int numSections = (unsigned int)pSegmentPtr->nsects;
            const struct section* pSectionPtr = (const struct section*)(((const unsigned char*)pSegmentPtr) + sizeof(struct segment_command));
			int texttextsectionprocessed = 0;

            // Need to find the __text __TEXT section
            for (jCnt = 0; jCnt < numSections; jCnt++)
			{
				if ( !strcmp(pSectionPtr->sectname, "__text") && !strcmp(pSectionPtr->segname, "__TEXT"))
				{
					// Found it
					const unsigned char* pSectionData = (((const unsigned char*)pmach_header) + pSectionPtr->offset);

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
    }

	if (hashCreated) 
	{
		cchmac_final(di, ctx, hmac_buffer);
		result = YES;
	}  

	return result;
}

static BOOL StringStartsWithDash(NSString *tempStr)
{
	const char* cStr  = [tempStr UTF8String];
	return (*cStr == '-');
}

static void usage(const char* programName)
{
	printf("%s usage:\n", programName);
	printf(" [-h, --help]          \tPrint out this help message\n");
    printf(" [-v, --verbose]       \tPrint verbose logging\n");
	printf(" [-i, --input]         \tSpecify the file that will be have a HMAC created\n");
	printf(" [-f, --file]		   \tSpecifiy the file that will be written out with the generated HMAC\n");
	printf(" [-p, --plist]         \tSpecify the plist file that will received the HMAC data\n");
	printf("\n");
}

//#define HARDCODE 1

int main (int argc, const char * argv[])
{

    @autoreleasepool 
	{
		const char*		programName = argv[0];
		BOOL 			verbose = NO;
		NSString* 		inputPath  = nil;
		NSString*		plistPath = nil;
		NSString*		filePath = nil;
		NSFileManager*	fileManager = [NSFileManager defaultManager];
        BOOL            isDir = NO;
		BOOL			isForKext;
        NSError* 		error = nil;

#ifdef HARDCODE
        
        const char* myArgv[] = 
        {
            "foo",
            "-i",
            "/usr/lib/system/libcorecrypto.dylib",
            "-f",
            "/tmp/corecrypto.dst/var/db/FIPS/fips_data",
            "-v"
        }; 
        
        int myArgc = (sizeof(myArgv) / sizeof(const char*));
        
        argc = myArgc;
        argv = myArgv;
        
        
#endif // HARDCODE

		for (int iCnt = 1; iCnt < argc; iCnt++)
		{
			const char* arg = argv[iCnt];

			if (!strcmp(arg, "-h") || !strcmp(arg, "--help"))
			{
				return -1;
			}
			else if (!strcmp(arg, "-v") || !strcmp(arg, "--verbose"))
			{
				verbose = YES;
			}
			else if (!strcmp(arg, "-i") || !strcmp(arg, "--input"))
	        {
	            if ((iCnt + 1) == argc)
	            {
	                return -1;
	            }
            
	            inputPath = [NSString stringWithUTF8String:argv[iCnt + 1]];
	            if (nil == inputPath || StringStartsWithDash(inputPath))
	            {
	                return -1;
	            }
            
	            iCnt++;
            
	            inputPath = [inputPath stringByExpandingTildeInPath];
            
	            if (![fileManager fileExistsAtPath:inputPath isDirectory:&isDir] || isDir)
	            {
	                return -1;
	            }			
	        }
			else if (!strcmp(arg, "-p") || !strcmp(arg, "--plist"))
	        {
	            if ((iCnt + 1) == argc)
	            {
	                return -1;
	            }
            
	            plistPath = [NSString stringWithUTF8String:argv[iCnt + 1]];
	            if (nil == plistPath || StringStartsWithDash(plistPath))
	            {
	                return -1;
	            }
            
	            iCnt++;
            
	            plistPath = [plistPath stringByExpandingTildeInPath];
            
	            if (![fileManager fileExistsAtPath:plistPath isDirectory:&isDir] || isDir)
	            {
	                return -1;
	            }			
	        }
			else if (!strcmp(arg, "-f") || !strcmp(arg, "--file"))
			{
				if ((iCnt + 1) == argc)
	            {
	                return -1;
	            }
	
				filePath = [NSString stringWithUTF8String:argv[iCnt + 1]];
	            if (nil == filePath || StringStartsWithDash(filePath))
	            {
	                return -1;
	            }
            
	            iCnt++;
            
	            filePath = [filePath stringByExpandingTildeInPath];		
			}
            else if (!strcmp(arg, "-a") || !strcmp(arg, "--arch"))
            {
                
                if ((iCnt + 1) == argc)
	            {
	                return -1;
	            }
                
                iCnt++;  // ignore
            }
            
		}
	
		// Make sure we have what is needed
		if (nil == inputPath)
		{
			if (verbose)
			{
                fprintf(stderr, "No input specified\n");
				usage(programName);
			}
            return -1;
		}
		
		// Only one of either -p or -f can be used
		if (nil == filePath && nil == plistPath)
		{
			if (verbose)
			{
                fprintf(stderr, "Only one of either -p or -f can be used\n");
				usage(programName);
			}
            return -1;
		}
		
		if (nil != filePath && nil != plistPath)
		{
			if (verbose)
			{
                fprintf(stderr, "Only one of filePath or plist can be used\n");
				usage(programName);
			}
            return -1;
		}
		
		isForKext  = (nil != plistPath);
		
		
		
		// First generate the HMAC
		
        NSData* input_data = [NSData dataWithContentsOfFile:inputPath options:NSDataReadingMappedIfSafe error:&error];
		if (error != nil || nil == input_data)
		{
			if (verbose)
			{
				fprintf(stderr, "Could not read the kext file at %s\n", [inputPath UTF8String]);
				usage(programName);
			}
			
            return -1;
		}
        
        size_t file_length = [input_data length];
        unsigned char* pData = (unsigned char *)malloc(file_length);
        memcpy(pData, [input_data bytes], [input_data length]);
		
		NSMutableDictionary* recordsToWrite = [NSMutableDictionary dictionary];
		size_t sha256DigestBufferLength = 32;
	    unsigned char hmac_buffer[sha256DigestBufferLength];
		NSData* hmacValue = nil;
		NSString* keyStr = nil;	    
		
		// Look to see if this is a FAT mach file.  While the 
        struct fat_header fHeader;
        memset(&fHeader, 0, sizeof(fHeader));
        memcpy(&fHeader, (struct fat_header*)pData, sizeof(fHeader));
        
        struct fat_arch fArch;
        memset(&fArch, 0, sizeof(fArch));
        
		if (FAT_MAGIC == fHeader.magic || FAT_CIGAM == fHeader.magic)
		{
#ifdef VERBOSE_LOGGING
			NSLog(@"This MACH file is FAT");
#endif
			// This is a FAT mach header
			// Loop through the archs
            
            // Swap the fat_header
            swap_fat_header(&fHeader, NXHostByteOrder());
			uint32_t arch_cnt;
            size_t fat_header_size = sizeof(struct fat_header);
            size_t fat_arch_size = sizeof(struct fat_arch);
            
			            
			
#ifdef VERBOSE_LOGGING
			NSLog(@"FAT: There are %d archs in the file", fHeader.nfat_arch);
#endif
			for (arch_cnt = 0; arch_cnt < fHeader.nfat_arch; arch_cnt++)
			{
#ifdef VERBOSE_LOGGING
				NSLog(@"FAT: Processing arch %d", arch_cnt);
#endif
                
                size_t arch_offset = fat_header_size +  (fat_arch_size *  arch_cnt);
                
                struct fat_arch* arch_struct = (struct fat_arch*)(pData + arch_offset);
                memset(&fArch, 0, fat_arch_size);
                memcpy(&fArch, arch_struct, fat_arch_size);
               
                swap_fat_arch(&fArch, 1, NXHostByteOrder());
                
				struct mach_header* pmach_header = (struct mach_header*)(((unsigned char *)pData) + fArch.offset);
                                  
				memset(hmac_buffer, 0, sha256DigestBufferLength);
				if (!get_hmac_value(pmach_header, hmac_buffer, &keyStr))
				{
					if (verbose)
					{
						fprintf(stderr, "Could not create the HMAC(1) for the file %s\n", [inputPath UTF8String]);
						// This really isn't a usage problem - commenting out: usage(programName);
					}
                    free (pData);
		            return -1;
				}
				
				hmacValue = [NSData dataWithBytes:hmac_buffer length:sha256DigestBufferLength];
				[recordsToWrite setObject:hmacValue forKey:keyStr];
			}
		}
		else
		{
#ifdef VERBOSE_LOGGING
				NSLog(@"This is NOT a FAT MACH file. Processing a single arch");
#endif
			// This is not a FAT mach header
			const struct mach_header* pmach_header = (const struct mach_header*)[input_data bytes];
			memset(hmac_buffer, 0, sha256DigestBufferLength);
			if (!get_hmac_value(pmach_header, hmac_buffer, &keyStr))
			{
				if (verbose)
				{
					fprintf(stderr, "Could not create the HMAC(2) for the file %s\n", [inputPath UTF8String]);
                    // This really isn't a usage problem - commenting out: usage(programName);
				}
                free (pData);
	            return -1;
			}
			
			hmacValue = [NSData dataWithBytes:hmac_buffer length:sha256DigestBufferLength];
			[recordsToWrite setObject:hmacValue forKey:keyStr];
		}
		
        if (isForKext)
		{
			NSMutableDictionary* kext_plist = [NSMutableDictionary dictionaryWithContentsOfFile:plistPath];
			if (nil == kext_plist)
			{
				if (verbose)
				{
					fprintf(stderr,"No plist was found at %s\n", [plistPath UTF8String]);
					usage(programName);
				}
	            free (pData);
	            return -1;
			}

	        NSMutableDictionary* hash_dict = [NSMutableDictionary dictionary];
			NSArray* hashKeys = [recordsToWrite allKeys];
	        for (NSString* dictKeyStr in hashKeys)
			{
#ifdef VERBOSE_LOGGING
				NSLog(@"Setting the hash data for the %@ arch", dictKeyStr);
#endif
				NSData* hash_data = [recordsToWrite objectForKey:dictKeyStr];
				[hash_dict setObject:hash_data forKey:dictKeyStr];
			}

	        [kext_plist setObject:hash_dict forKey:kAppleTextHashesKey];
			[kext_plist writeToFile:plistPath atomically:TRUE];
		}
		else
		{
			// This is the OSX user space HMAC
			// First get the parent directory
			NSString* parent_dir = [filePath stringByDeletingLastPathComponent];
			
			if (![fileManager fileExistsAtPath:parent_dir isDirectory:&isDir] || !isDir)
            {
                // Need to create the directory
				if (![fileManager createDirectoryAtPath:parent_dir withIntermediateDirectories:YES attributes:nil error:&error])
				{
					if (verbose)
					{
						fprintf(stderr,"Error creating the output directory %s\n", [[error localizedDescription] UTF8String]);
                        // This really isn't a usage problem - commenting out: usage(programName);
					}
					free (pData);
		            return -1;
				}
            }
			
			NSMutableData* file_data = [NSMutableData data];
			NSArray* hashKeys = [recordsToWrite allKeys];
	        for (NSString* dictKeyStr in hashKeys)
			{
#ifdef VERBOSE_LOGGING
				NSLog(@"Setting the hash data for the %@ arch", dictKeyStr);
#endif


				NSData* hash_data = [recordsToWrite objectForKey:dictKeyStr];
				unsigned char temp_buffer[256];
				memset(temp_buffer, 0, 256);
				// bytesToHexString(unsigned char* bytes, unsigned long byteLength, unsigned char* buffer, unsigned long bufferLength)
				bytesToHexString([hash_data bytes], [hash_data length], (unsigned char*)temp_buffer, 256);
				
				NSString* hash_data_hex_str = [NSString stringWithUTF8String:(const char *)temp_buffer];
#ifdef VERBOSE_LOGGING
				NSLog(@"key = %@ hash_data_hex_str is %@", dictKeyStr, hash_data_hex_str);
#endif				
				NSString* output_str = [NSString stringWithFormat:@"%@:%@\n", dictKeyStr, hash_data_hex_str];
				NSData* output_str_data = [output_str dataUsingEncoding:NSUTF8StringEncoding];
				[file_data appendData:output_str_data];
			}
			
			if (![file_data writeToFile:filePath options:0 error:&error])
			{
				if (verbose)
				{
					fprintf(stderr,"Error writing out the HMAC data %s\n", [[error localizedDescription] UTF8String]);
                    // This really isn't a usage problem - commenting out: usage(programName);
				}
				
				free (pData);
	            return -1;
			}
						
		}
		
        free (pData);
    }
    
    return 0;
}

