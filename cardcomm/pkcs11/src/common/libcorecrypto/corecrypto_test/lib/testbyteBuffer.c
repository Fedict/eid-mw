/*
 * Copyright (c) 2012,2014,2015 Apple Inc. All rights reserved.
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


#include <assert.h>
#include "testbyteBuffer.h"

void printBytes(uint8_t *buff, size_t len, char *name, int spaced)
{
	size_t i;
	printf("Dumping %d bytes from %s\n", (int) len, name);
	for(i=0; i<len; i++) {
		if(spaced && i > 0 && !(i%8)) putchar(' ');
		if(spaced && i > 0 && !(i%64)) putchar('\n');
		printf("%02x", buff[i]);
	}
	putchar('\n');
}

void printByteBuffer(byteBuffer bb, char *name)
{
    printBytes(bb->bytes, bb->len, name, 1);
}

void printByteBufferAsCharAssignment(byteBuffer bb, char *varname)
{
    printf("\t%s = \"", varname);
    for(size_t i = 0; i<bb->len; i++) printf("%02x", bb->bytes[i]);
    printf("\";\n");
}


byteBuffer
mallocByteBuffer(size_t len)
{
	byteBuffer retval;
	if((retval = (byteBuffer) malloc(sizeof(byteBufferStruct) + len + 1)) == NULL) return NULL;
    retval->len = len;
    retval->bytes = (uint8_t *) (retval + 1) ; /* just past the byteBuffer in malloc'ed space */
    return retval;
}

/* utility function to convert hex character representation to their nibble (4 bit) values */
static uint8_t
nibbleFromChar(char c)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	return 255;
}

/* Convert a string of characters representing a hex buffer into a series of bytes of that real value */
byteBuffer
hexStringToBytes(const char *inhex)
{
	byteBuffer retval;
	const uint8_t *p;
	int len,inhex_len, i;
    
    if(!inhex) inhex = "";
    inhex_len=(int)strlen(inhex);
    assert((inhex_len & 1)==0); // needs to be even
	len = inhex_len / 2;
	if((retval = mallocByteBuffer(len)) == NULL) return NULL;
    
	for(i=0, p = (const uint8_t *) inhex; i<len; i++) {
        retval->bytes[i] = (nibbleFromChar(*p) << 4) | nibbleFromChar(*(p+1));
        p += 2;
	}
    retval->bytes[len] = 0;
	return retval;
}


byteBuffer
bytesToBytes(void *bytes, size_t len)
{
    byteBuffer retval = mallocByteBuffer(len);
    if(retval && bytes) memcpy(retval->bytes, bytes, len);
    return retval;
}

int
bytesAreEqual(byteBuffer b1, byteBuffer b2)
{
    if(b1->len != b2->len) return 0;
    return (memcmp(b1->bytes, b2->bytes, b1->len) == 0);
}


static char byteMap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
static int byteMapLen = sizeof(byteMap);

/* Utility function to convert nibbles (4 bit values) into a hex character representation */
static char
nibbleToChar(uint8_t nibble)
{
	if(nibble < byteMapLen) return byteMap[nibble];
	return '*';
}

/* Convert a buffer of binary values into a hex string representation */
char
*bytesToHexString(byteBuffer bb)
{
	char *retval;
	size_t i;
	
	retval = malloc(bb->len*2 + 1);
	for(i=0; i<bb->len; i++) {
		retval[i*2] = nibbleToChar(bb->bytes[i] >> 4);
		retval[i*2+1] = nibbleToChar(bb->bytes[i] & 0x0f);
	}
    retval[bb->len*2] = 0;
	return retval;
}

char
*bytesToHexStringWithSpaces(byteBuffer bb, int breaks)
{
	char *retval;
	size_t i, j;
    
    if(breaks == 0) {
        return bytesToHexString(bb);
    }
	
    breaks /= 2;
	retval = malloc(bb->len*2 + 1 + (bb->len*2 / breaks) + 10);
	for(i=0, j=0; i<bb->len; i++, j+=2) {
		retval[j] = nibbleToChar(bb->bytes[i] >> 4);
		retval[j+1] = nibbleToChar(bb->bytes[i] & 0x0f);
        if(((i+1) % breaks) == 0) {
            retval[j+2] = ' ';
            retval[j+3] = 0;
            j++;
        }
	}
	return retval;
}


