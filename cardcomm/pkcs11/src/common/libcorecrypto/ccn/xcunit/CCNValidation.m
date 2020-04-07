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


#import "CCNValidation.h"
#import "ccn_unit.h"

#define GI_MODG_ENABLE  1

#include <corecrypto/ccrng.h>
#include <corecrypto/ccrng_system.h>
#include <corecrypto/cczp.h>
#include <corecrypto/cczp_priv.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define LOOPS_DEF		3
#define MAX_SIZE_DEF	3 * 4096 / 8
#define LOOP_NOTIFY		100

/* tweak these to focus on a specific test */
#define DVT_BASIC_ENABLE		1		/* comp, mulg, addg, subg */
#define DVT_SHIFT_ENABLE		0		/* shift l/r */
#define DVT_MOD_ENABLE			1		/* mod */
#define DVT_GCD_ENABLE			0		/* gcd */
#define DVT_PRIME_ENABLE		0		/* prime test */
#define DVT_INVMOD_ENABLE		0		/* ginverseMod */
#define DVT_DIVMOD_ENABLE		0		/* divmodg_via_recip */

@implementation CCNValidation

/*
 * ...min <= return <= max
 */

/* Global RNG, intialized in pthread_once */
static pthread_once_t kinitRand = PTHREAD_ONCE_INIT;
static struct ccrng_system_state system_rng;
static struct ccrng_state *rng=(struct ccrng_state *)&system_rng;


static void initRand(void) {
    /* Initialize the random number generator */
    ccrng_system_init(&system_rng);
}

static uint32_t genRand(uint32_t min, uint32_t max)
{
	uint32_t r;
	uint8_t c[4];

    pthread_once(&kinitRand, initRand);

    if (max == min) {			/* avoid % 1 ! */
		return max;
	}

    ccrng_generate(rng, 4, c);

	r = (((uint32_t)c[0] << 24) |
         ((uint32_t)c[1] << 16) |
         ((uint32_t)c[2] << 8) |
         c[3]);
	return min + (r % (max - min + 1));
}

/*
 * Fill buffer with random data, random size from 1 to maxByteSize.
 * Returns size of random data generated.
 */
#define MIN_SIZE		1

static cc_size fillData(cc_size maxByteSize, uint8_t *data)
{
	cc_size		size;

    pthread_once(&kinitRand, initRand);

	size = maxByteSize; //genRand(MIN_SIZE, maxByteSize);
    ccrng_generate(rng, size, data);
	return size;
}

/*
 * create a giant with random size and data. *Buf is mallocd and
 * uninitialized and will change here.
 * Giants with values of zero  or one will never be returned - too
 * many corner cases involving those numbers.
 */
static cc_size genGiant(cc_size count, cc_unit *r, size_t maxBytes, uint8_t *buf)
{
    assert(maxBytes <= ccn_sizeof_n(count));
	for(;;) {
		cc_size size = fillData(maxBytes, buf);

        if (ccn_read_uint(count, r, size, buf)) {
			/* should never happen */
			printf("***genGiant overflow\n");
			exit(1);
		}
        cc_unit one[count];
        ccn_seti(count, one, 1);
        /* If r is less or equal to 1 we don't return it. */
        if (ccn_cmp(count, r, one) <= 0)
            continue;

		return count;
	}
}

/*
 * Individual tests. API is identical for all tests.
 *
 * g1, g2 : giants with random data, size, and sign. Tests do not modify
 *           these.
 * scr1, scr2 : scratch giants, big enough for all conceivable ops. Can
 *           be modified at will.
 * Return : 0 for sucess, 1 on error.
 */
#if DVT_BASIC_ENABLE

-(void) compTst:(cc_size) count : (const cc_unit *) g1 : (cc_unit *) scr1 : (cc_unit *) scr2
{
    ccn_set(count, scr1, g1);
    ccn_set(count, scr2, scr1);
    XCAssertCCNEquals(count, g1, scr2,
                      @"scr1 = g1; scr2 = scr1; g1 == scr2");
}

-(void) addSubTst:(cc_size) count : (const cc_unit *) g1 : (const cc_unit *) g2 : (cc_unit *) scr1
{
    ccn_add(count, scr1, g1, g2);     // scr1 := g1 + g2
    ccn_sub(count, scr1, scr1, g1);   // scr1 := g1 + g2 - g1 =? g2
    XCAssertCCNEquals(count, g2, scr1, @"g2 == g1+g2-g1\ng1 = %@", ccn_string(count, g1));
}

/* Largest random uint32_t integer to use a multiplier */
#define LARGEST_MUL 0xffff

-(void) mulTst:(cc_size) count : (const cc_unit *) g1 : (cc_unit *) scr1 : (cc_unit *) scr2
{
	uint32_t randInt = genRand(1, LARGEST_MUL);
	uint32_t i;

    ccn_seti(2 * count, scr1, randInt);    // scr1 := randInt
    ccn_mul(count, scr2, g1, scr1); // scr2 := g1 * randInt

	/* now do the same thing with multiple adds */
    ccn_seti(2 * count, scr1, 0);       // scr1 := 0
    cc_unit carry = 0;
	for(i = 0; i<randInt; i++) {
		carry += ccn_add(count, scr1, scr1, g1);
	}
    scr1[count] = carry;
    XCAssertCCNEquals(2 * count, scr2, scr1,
                      @"g1*rand (via mul) == g1*rand (via add)\ng1 = %@\nrand = %lu",
                      ccn_string(count, g1), randInt);
}

-(void) squareTst:(cc_size) count : (const cc_unit *) g1 : (cc_unit *) scr1 : (cc_unit *) scr2
{
    ccn_mul(count, scr1, g1, g1);
    ccn_sqr(count, scr2, g1);
    XCAssertCCNEquals(count, scr1, scr2, @"g1*g1 == g1^2\ng1 = %@",
                      ccn_string(count, g1));
}

-(void) mulOnesTst:(cc_size) count : (const cc_unit *) g1 : (cc_unit *) scr1 : (cc_unit *) scr2
{
    cc_unit gOnes[count];

	/* set up gOnes with all ones data */
	for (cc_size i = 0; i < count; ++i) {
		gOnes[i] = (cc_unit)(-1);
	}

    ccn_mul(count, scr1, gOnes, g1);
    ccn_mul(count, scr2, g1, gOnes);
    XCAssertCCNEquals(count, scr1, scr2, @"g1*allones == allones*g1\ng1 = %@",
                      ccn_string(count, g1));
}

#endif	/* DVT_BASIC_ENABLE */

#if		DVT_SHIFT_ENABLE

#define MIN_SHIFT	1

-(void) lshiftTst:(cc_size) count : (const cc_unit *) g1 : (const cc_unit *) g2 : (cc_unit *) scr1 : (cc_unit *) scr2
{
	/* can only shift the size of an lgiant, which could fill up a bgiant */
	int maxShift = GI_MAX_PRIME_SIZE_BITS;
	int shiftCnt = genRand(MIN_SHIFT, maxShift);
	bgiant scr3;
	int rtn = 0;

	localBigGiantAlloc(&scr3);
	gtog(g1, scr1);						// scr1 := g1
	gshiftleft(shiftCnt, scr1);			// scr1 := (g1 << shiftCnt)

	gtog(g1, scr2);						// scr2 := g1
	if(shiftCnt <= 14) {
	    int multInt = (1 << shiftCnt);
	    int_to_giant(multInt, &scr3.g);	// scr3 := (1 << shiftCnt)
	}
	else {
	    int_to_giant(1, &scr3.g);		// scr3 := 1;
	    gshiftleft(shiftCnt, &scr3.g);	// scr3 := (1 << shiftCnt)
	}
	mulg(&scr3.g, scr2);				// scr2 := g1 * (1 << shiftCnt);
	if(gcompg(scr1, scr2)) {
		printf("shiftCnt %d 0x%x\n", shiftCnt, shiftCnt);
		printf("g1   : "); printGiantHex(g1);
		printf("scr1 : "); printGiantHex(scr1);
		printf("scr2 : "); printGiantHex(scr2);
		printf("gshiftleft error\n");
		rtn = testError();
	}
	return rtn;
}

/* clear a bit in a giant */
static void clearBit(cc_size count,
                     giant g,
                     unsigned bitPos)
{
    gi_uint16 nDex = bitPos >> CCN_LOG2_BITS_PER_UNIT;
    giantDigit bitMask = 1 << (bitPos & (GIANT_BITS_PER_DIGIT - 1));

	if(nDex > (g->sign - 1)) {
		printf("***clearBit screwup, nDex %u sign %u\n", nDex, g->sign);
		return;
	}
	g->n[nDex] &= ~bitMask;
}

-(void) rshiftTst:(cc_size) count : (const cc_unit *) g1 : (const cc_unit *) g2 : (cc_unit *) scr1 : (cc_unit *) scr2
{

	unsigned maxShift = bitlen(g1) - 1;
	unsigned shiftCnt;
	lgiant scr3;
	int rtn = 0;
	unsigned dex;

	localGiantAlloc(&scr3);

	/* special case, can't have g1 = 1 */
	if(maxShift == 0) {
#if	GIANTS_DEBUG
		printf("...rshiftTst: tweaking g1 = 1\n");
#endif
		g1->n[0] = 2;
		shiftCnt = 1;
	}
	else {
		shiftCnt = genRand(1, maxShift);
	}

	/* first do the shift right */
	gtog(g1, scr1);						// scr1 := g1
	gshiftright(shiftCnt, scr1);		// scr1 := (g1 >> shiftCnt)

	/* now shift left the result by the same amount */
	gtog(scr1, scr2);					// scr2 := (g1 >> shiftCnt)
	gshiftleft(shiftCnt, scr2);			// scr2 := ((g1 >> shiftCnt) << shiftCnt

	/* get a copy of original giant a mask off the bits which should be full of
	 * zeroes now */
	gtog(g1, &scr3.g);
	for(dex=0; dex<shiftCnt; dex++) {
		clearBit(count, &scr3.g, dex);
	}

	if(gcompg(scr2, &scr3.g)) {
		printf("shiftCnt %u 0x%x\n", shiftCnt, shiftCnt);
		printf("g1   : "); printGiantHex(g1);
		printf("scr1 : "); printGiantHex(scr1);
		printf("scr2 : "); printGiantHex(scr2);
		printf("scr3 : "); printGiantHex(&scr3.g);
		printf("gshiftright error\n");
		rtn = testError();
	}
	return rtn;
}

-(void) rshiftwordTst:(cc_size) count : (const cc_unit *) g1 : (const cc_unit *) g2 : (cc_unit *) scr1 : (cc_unit *) scr2
{

	unsigned maxShiftWords = g1->sign;
	unsigned shiftCntWords;
	unsigned shiftCntBits;

	int rtn = 0;

	shiftCntWords = genRand(1, maxShiftWords);
	shiftCntBits = shiftCntWords * GIANT_BITS_PER_DIGIT;

	/* shift right by words --> scr1 */
	gtog(g1, scr1);
	gshifltwordsright(shiftCntWords, scr1);

	/* shift right by bits --> scr2 */
	gtog(g1, scr2);
	gshiftright(shiftCntBits, scr2);

	/*
	 * this assumes of course that we have a reliable gshiftright(), which
	 * was in fact tested in rshiftTest().
	 */
	if(gcompg(scr1, scr2)) {
		printf("shiftCntWords %u 0x%x\n", shiftCntWords, shiftCntWords);
		printf("g1   : "); printGiantHex(g1);
		printf("scr1 : "); printGiantHex(scr1);
		printf("scr2 : "); printGiantHex(scr2);
		printf("gshifltwordsright error\n");
		rtn = testError();
	}
	return rtn;
}

#endif	/* DVT_SHIFT_ENABLE */

#if		GI_MODG_ENABLE && DVT_MOD_ENABLE

#define ccn_mod(count, n, a, d) ([self ccn_mod: count : n : a : d])

-(void) ccn_mod:(cc_size) count : (cc_unit *) n : (const cc_unit *) a : (const cc_unit *) d
{
    struct cczp *zp = alloca(cczp_size(ccn_sizeof_n(count)));

    CCZP_N(zp) = count;
    ccn_set(count, CCZP_PRIME(zp), d);
    cczp_init(zp);

    CC_DECL_WORKSPACE(ws,CCZP_MOD_WORKSPACE_N(count));
    cczp_mod(zp, n,a , ws );
    CC_FREE_WORKSPACE(ws);
}

#define ccn_modn(count, n, count_a, a, d) ([self ccn_modn: count : n : count_a : a : d])

-(void) ccn_modn:(cc_size) count : (cc_unit *) n : (cc_size) count_a :(const cc_unit *) a : (const cc_unit *) d
{
    struct cczp *zp = alloca(cczp_size(ccn_sizeof_n(count)));

    CCZP_N(zp) = count;
    ccn_set(count, CCZP_PRIME(zp), d);
    cczp_init(zp);

    cczp_modn(zp, n, count_a,a);
}

#define LARGEST_MOD_MUL	0x40

-(void) modTst:(cc_size) count : (const cc_unit *) g1 : (const cc_unit *) g2 : (cc_unit *) scr1 : (cc_unit *) scr2
{
    cc_unit *scr3 = alloca(ccn_sizeof_n(2 * count)),
        *scr4 = alloca(ccn_sizeof_n(2 * count));
    const cc_unit *modulus;
    const cc_unit *factor;
	//int randInt = genRand(1, LARGEST_MOD_MUL);
    //ccn_seti(count, scr1, randInt); // scr1 = rand
    uint8_t buf[ccn_sizeof_n(count)];
    genGiant(2*count, scr1, ccn_sizeof_n(count) - genRand(0, CCN_UNIT_SIZE - 1), buf);
    if (ccn_cmp(count,g2,g1)>=0)
    {
        modulus=g2;
        factor=g1;
    }
    else
    {
        modulus=g1;
        factor=g2;
    }

	/* current ccn_mod can't deal with g mod 1 */
    if (ccn_bitlen(count, modulus) == 1) {
		printf("..modTst: tweaking g1 = 1\n");
		scr2[0] = 0x54;
	}
	/* end ccn_mod workaround */

	/* this will only work if randInt < g1 */
    if (ccn_cmp(count, scr1, modulus) >= 0) {
	    ccn_modn(count, scr1, 2*count,scr1, modulus);			// scr1 = rand mod g1
        XCTAssertTrue(ccn_cmp(count, scr1, modulus) < 0,
                     @"\nscr1 = %@\nmod = %@", ccn_string(count, scr1),
                     ccn_string(count, modulus));
	}
    else
    {
        ccn_mod(count, scr2, scr1, modulus);			// scr1 = rand mod g1
        XCTAssertTrue(ccn_cmp(count, scr1, scr2) == 0,
                     @"\nscr1 = %@\nmod = %@", ccn_string(count, scr1),
                     ccn_string(count, modulus));
        ccn_modn(count, scr2, 2*count,scr1, modulus);			// scr1 = rand mod g1
        XCTAssertTrue(ccn_cmp(count, scr1, scr2) == 0,
                     @"\nscr1 = %@\nmod = %@", ccn_string(count, scr1),
                     ccn_string(count, modulus));
    }

    ccn_mul(count, scr3, factor, modulus);              // scr3 = g2 * g1
    if (ccn_add(count, scr3, scr3, scr1))               // scr3 = g2 * g1 + rand
        ccn_add1(count, scr3 + count, scr3 + count, 1); // propagate carry in scr3

	ccn_mod(count, scr4, scr3, modulus);                // scr4 := scr3 mod g1 =? rand
    XCAssertCCNEquals(count, scr1, scr4, @"rand != (g2 * g1 + rand) mod g1\ng1: %@\ng2: %@\nscr3: %@\nscr4:%@", ccn_string(count, g1), ccn_string(count, g2), ccn_string(2 * count, scr3), ccn_string(2 * count, scr4));

    ccn_modn(count, scr4, 2*count,scr3, modulus);                // scr3 := scr3 mod g1 =? rand
    XCAssertCCNEquals(count, scr1, scr4, @"rand != (g2 * g1 + rand) mod g1\ng1: %@\ng2: %@\nscr3: %@\nscr4:%@", ccn_string(count, g1), ccn_string(count, g2), ccn_string(2 * count, scr3), ccn_string(2 * count, scr4));
}

#endif	/* GI_MODG_ENABLE && DVT_MOD_ENABLE */

#if		GI_MODG_ENABLE && DVT_GCD_ENABLE

#define SHOW_GCDTEST	0

-(void) gcdTst:(cc_size) count : (giant) g1 : (giant) g2 : (giant) scr1 : (giant) scr2
{
	/*
	 * This test's algorithm results in a gcd op on (g1*randInt, g2*randInt).
	 * This can overflow an lgiant, so we have to truncate g1 and g2 to ensure
	 * we're within bounds.
	 */
#define MAX_GCD_TRUNC_SIZE	(MAX_PRIME_SIZE_DIGITS - 3)
	lgiant g1Trunc;
	lgiant g2Trunc;
	bgiant existGcd;
	uint32_t randInt = genRand(2, 0x100000);
	bgiant expectGcd;
	bgiant g1Mul;
	bgiant g2Mul;
	bgiant gotGcd;
	int ourRtn = 0;

	localGiantAlloc(&g1Trunc);
	localGiantAlloc(&g2Trunc);
	gtog(g1, &g1Trunc.g);
	gtog(g2, &g2Trunc.g);
	if(g1Trunc.g.sign > MAX_GCD_TRUNC_SIZE) {
		g1Trunc.g.sign = MAX_GCD_TRUNC_SIZE;
		gtrimSign(&g1Trunc.g);
	}
	if(g2Trunc.g.sign > MAX_GCD_TRUNC_SIZE) {
		g2Trunc.g.sign = MAX_GCD_TRUNC_SIZE;
		gtrimSign(&g2Trunc.g);
	}

	localBigGiantAlloc(&existGcd);
	localBigGiantAlloc(&g1Mul);
	localBigGiantAlloc(&g2Mul);
	localBigGiantAlloc(&expectGcd);
	localBigGiantAlloc(&gotGcd);

	/* first see if g1 and g2 already have a gcd other than 1 */
	gtog(&g1Trunc.g, &existGcd.g);
	nbgcdg(&g2Trunc.g, &existGcd.g);
	if(isone(&existGcd.g)) {
		/*
		 * g1Trunc, g2Trunc are apparently relatively prime.
		 * Quick check: are they both even?
		 */
		if( ((g1Trunc.g.n[0] & 1) == 0) && ((g2Trunc.g.n[0] & 1) == 0) ) {
			printf("***HEY! WE've got two even numbers whose gcd is 1!\n");
			printf("g1        : "); printGiant(&g1Trunc.g);
			printf("g2        : "); printGiant(&g2Trunc.g);
			return testError();
		}
	}

	/* random multiplier */
	int_to_giant(randInt, scr1);

	/*
	 * Expected gcd after we multiply g1Trunc and g2Trunc by randInt is
	 * existGcd * randInt
	 */
	mulg_common(scr1, &existGcd.g, &expectGcd.g);

	/* multiply both reference giants by randInt */
	mulg_common(&g1Trunc.g, scr1, &g1Mul.g);
	mulg_common(&g2Trunc.g, scr1, &g2Mul.g);

	/* gcd of resulting giants should be expectedGcd */
	gtog(&g2Mul.g, &gotGcd.g);
	nbgcdg(&g1Mul.g, &gotGcd.g);
	if(gcompg(&gotGcd.g, &expectGcd.g)) {
		ourRtn = 1;
		printf("****gcompg failure\n");
	}
	if(ourRtn || SHOW_GCDTEST) {
		printf("----------- \n");
		printf("g1        : "); printGiant(&g1Trunc.g);
		printf("g2        : "); printGiant(&g2Trunc.g);
		printf("existGcd  : "); printGiant(&existGcd.g);
		printf("randInt   : "); printGiant(scr1);
		printf("g1Mul     : "); printGiant(&g1Mul.g);
		printf("g2Mul     : "); printGiant(&g2Mul.g);
		printf("expectGcd : "); printGiant(&expectGcd.g);
		printf("gotGcd    : "); printGiant(&gotGcd.g);
		printf("----------- \n");
	}
	if(ourRtn) {
		return testError();
	}
	return 0;
}

#endif	/* GI_MODG_ENABLE && DVT_GCD_ENABLE */

#if GI_PRIME_TEST_ENABLE && DVT_PRIME_ENABLE

/*
 * isGiantPrime() test - easy - just multiply g1 * g2 and make sure it's
 * not reported as prime. However we have to cut the sizes of g1 and
 * g2 in half - can't do isGiantPrime on a number bigger than
 * GIANT_LITTLE_DIGITS.
 */
#define PRIME_DEPTH_DEF		8

-(void) primeTst:(cc_size) count : (giant) g1 : (giant) g2 : (giant) scr1 : (giant) scr2
{
	lgiant t1;
	lgiant t2;

	localGiantAlloc(&t1);
	localGiantAlloc(&t2);

	gtog(g1, &t1.g);
	gtog(g2, &t2.g);
	if(t1.g.sign > (GIANT_LITTLE_DIGITS / 2)) {
		t1.g.sign /= 2;
	}
	if(t2.g.sign > (GIANT_LITTLE_DIGITS / 2)) {
		t2.g.sign /= 2;
	}

	mulg_common(&t1.g, &t2.g, scr1);
	if(isGiantPrime(scr1, PRIME_DEPTH_DEF)) {
		printf("***isGiantPrime error: composite number reported as prime for "
               "depth %u\n", PRIME_DEPTH_DEF);
		printf("t1   : "); printGiant(&t1.g);
		printf("t2   : "); printGiant(&t2.g);
		printf("scr1 : "); printGiant(scr2);
		return testError();
	}
	return 0;
}

/*
 * Highest known prime in giantPrime.c - we have to keep this in sync if
 * SIZEOF_PRIME_TABLE changes.
 */
#define SIEVE_MAX	1619

-(void) primeSieveTst:(cc_size) count : (giant) g1 : (giant) g2 : (giant) scr1 : (giant) scr2
{
	/*
	 * multiply g1 by a random r, 2<=r<=SIEVE_MAX. Ensure that
	 * checkPrimeCandidate reports the result as SIEVE_COMPOSITE or
	 * SIEVE_CANDIDATE.
	 */
	unsigned r = genRand(2, SIEVE_MAX);
	int rtn;

	int_to_giant(r, scr1);
	mulg_common(g1, scr1, scr2);
	rtn = checkPrimeCandidate(scr2);
	switch(rtn) {
		case SIEVE_COMPOSITE:
		case SIEVE_CANDIDATE:
			return 0;
		default:
			break;
	}
	printf("***checkPrimeCandidate returned %d for known composite\n", rtn);
	printf("r      : %u\n", r);
	printf("g1     : "); printGiant(g1);
	printf("r * g1 : "); printGiant(scr2);
	return testError();
}

#endif	/* GI_PRIME_TEST_ENABLE && DVT_PRIME_ENABLE */

#if		DVT_INVMOD_ENABLE && GI_MODG_ENABLE && GI_INVERSE_MOD_ENABLE

/* cook up a global prime modulus for ginverseMod(), just once then it's const */

lgiant primeModulus;
lgiant primeModulusRecip;

static GIReturn genPrimeModulus(cc_size count,
                                unsigned giantBytes,
                                int quiet)
{
	localGiantAlloc(&primeModulus);
	localGiantAlloc(&primeModulusRecip);

	GIReturn grtn;

	if(!quiet) {
		printf("...generating %u bit prime for ginverseModTest...\n", giantBytes * 8);
	}
	grtn = genPrimeGiant(&primeModulus.g, giantBytes * 8, PRIME_DEPTH_DEFAULT);
	if(grtn) {
		printf("***Error (%d) generating primeModulus for ginverseMod() test\n",
               (int)grtn);
		return grtn;
	}

	make_recip(&primeModulus.g, &primeModulusRecip.g);
	return GR_Success;
}

-(void) ginverseModTst:(cc_size) count : (giant) g1 : (giant) g2 : (giant) scr1 : (giant) scr2
{
	lgiant workG1;		/* working, possibly modified g1 */

	localGiantAlloc(&workG1);
	gtog(g1, &workG1.g);

	/* first ensure g1 < primeModulus */
	modg_via_recip(&primeModulus.g, &primeModulusRecip.g, &workG1.g);

	/* scr1 := g1^(-1) mod primeModulus */
	gtog(&workG1.g, scr1);
	ginverseMod(&primeModulus.g, &primeModulusRecip.g, scr1);

	/* scr2 := g1 * scr1 mod primeModulus, should equal 1 (one) */
	gtog(&workG1.g, scr2);
	rmulg(scr1, scr2, &primeModulus.g, &primeModulusRecip.g);

	if(!isone(scr2)) {
		printf("***ginverseMod failure, g1 * (g1^(-1) mod m) mod m != 1 ***\n");
		printf("----------- \n");
		printf("g1                   : "); printGiant(&workG1.g);
		printf("modulus              : "); printGiant(&primeModulus.g);
		printf("modulusRecip         : "); printGiant(&primeModulusRecip.g);
		printf("g1^(-1) mod m        : "); printGiant(scr1);
		printf("g1 * (g1^(-1)) mod m : "); printGiant(scr2);
		printf("----------- \n");
		return testError();
	}
	return 0;
}

#endif	/* DVT_INVMOD_ENABLE && GI_MODG_ENABLE && GI_INVERSE_MOD_ENABLE */

#if		DVT_DIVMOD_ENABLE && GI_MODG_ENABLE

/*
 * Test divmodg_via_recip().
 *
 * Cook up third random giant g3 such that g3 < g2;
 * giant product = (g1 * g2) + g3;
 * use divmodg_via_recip to get
 *    giant gmod = product mod g2;
 *    giant gdiv = product div g2;
 * Ensure that gmod==g3 and gdiv==g1;
 */
-(void) divModTst:(cc_size) count : (giant) g1 : (giant) g2 : (giant) scr1 : (giant) scr2
{
	lgiant g3;
	uint8_t buf[MAX_PRIME_SIZE_BYTES];
	tgiant product;
	giant gmod = scr1;
	giant gdiv = scr2;
	lgiant recip;
	int gotErr = 0;

	if(isone(g2)) {
		/* well shoot. This ain't gonna work. */
		printf("divModTst: g2==1; skipping this test.\n");
		return 0;
	}

	/* generate random g3 < g2 */
	localGiantAlloc(&g3);
	genGiant(count, &g3.g, g2->sign * GIANT_BYTES_PER_DIGIT, buf);
	if(gcompg(g2, &g3.g) <= 0) {
		lgiant oneG;

		/* g3 := g2 - 1 */
		localGiantAlloc(&oneG);
		int_to_giant(1, &oneG.g);
		gtog(g2, &g3.g);
		normal_subg(&oneG.g, &g3.g);
	}

	/* product := (g1 * g2) + g3 */
	localTriGiantAlloc(&product);
	mulg_common(g1, g2, &product.g);
	addg(&g3.g, &product.g);

	/*
	 * gmod = product mod g2;
	 * gdiv = product div g2;
	 */
	localGiantAlloc(&recip);
	make_recip(g2, &recip.g);
	gtog(&product.g, gmod);
	divmodg_via_recip(g2, &recip.g, gmod, gdiv);

	/* Ensure that gmod==g3 and gdiv==g1 */
	if(gcompg(&g3.g, gmod) != 0) {
		printf("***Error in mod portion of divmodg_via_recip\n");
		gotErr = 1;
	}
	if(gcompg(g1, gdiv) != 0) {
		printf("***Error in div portion of divmodg_via_recip\n");
		gotErr = 1;
	}
	if(gotErr) {
		printf("----------- \n");
		printf("g1                   : "); printGiant(g1);
		printf("g2                   : "); printGiant(g2);
		printf("g3                   : "); printGiant(&g3.g);
		printf("(g1 * g2) + g3       : "); printGiant(&product.g);
		printf("product mod g2       : "); printGiant(gmod);
		printf("product div g2       : "); printGiant(gdiv);
		printf("----------- \n");
		return testError();
	}
	return 0;
}

#endif	/* DVT_DIVMOD_ENABLE && GI_MODG_ENABLE */

static void testStartBanner(
                            char *testName,
                            int argc,
                            char **argv)
{
	int i;
	printf("Starting %s; args: ", testName);
	for(i=1; i<argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n");
}

-(void)main: (int)argc argv: (char **)argv
{
	int				arg;
	char			*argp;
	uint8_t         *buf;
	int				loop;

	int				loops = LOOPS_DEF;
	unsigned		maxByteSize = MAX_SIZE_DEF;
    cc_size maxCount = ccn_nof_size(maxByteSize);
	int				verbose = 0;
	int				quiet = 1;
	int				doPrimeMod = 1;

	for(arg=1; arg<argc; arg++) {
		argp = argv[arg];
		switch(argp[0]) {
		    case 'x':
			maxByteSize = (unsigned)atoi(&argp[2]);
				break;
		    case 'l':
			loops = atoi(&argp[2]);
				break;
			case 'k':
				doPrimeMod = 0;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'q':
				quiet = 1;
				break;
		    case 'h':
		    default:
			//usage(argv);
                break;
		}
	}
	buf = malloc(maxByteSize);

	/* Scratch giants, big enough for anything. */
    cc_unit g1[maxCount], g2[maxCount],
        scr1[2 * maxCount], scr2[2 * maxCount];

    if (argc > 1)
        testStartBanner("MPNValidation", argc, argv);

#if		DVT_INVMOD_ENABLE && GI_MODG_ENABLE && GI_INVERSE_MOD_ENABLE
	if (doPrimeMod) {
		/* one-time only init for this guy... */
		if (genPrimeModulus(maxCount, maxByteSize, quiet)) {
            free(buf);
			exit(1);
		}
	}
#endif

	for (loop=0; loop<loops; loop++) {

	    if (!quiet && ((loop % LOOP_NOTIFY) == 0)) {
		printf("..loop %d\n", loop);
	    }

        cc_size size = genRand(MIN_SIZE, maxByteSize);
        cc_size count = ccn_nof_size(size);
	    genGiant(count, g1, size, buf);
	    genGiant(count, g2, size, buf);

#if DVT_BASIC_ENABLE
		if (verbose) printf("...compTest\n");
	    [self compTst: count : g1 : scr1 : scr2];

		if (verbose) printf("...addSubTest\n");
	    [self addSubTst: count : g1 : g2 : scr1];

		if (verbose) printf("...mulTest\n");
	    [self mulTst: count : g1 : scr1 : scr2];

		if (verbose) printf("...squareTest\n");
	    [self squareTst: count : g1 : scr1 : scr2];

		if (verbose) printf("...mulOnesTest\n");
	    [self mulOnesTst: count : g1 : scr1 : scr2];
#endif	/* DVT_BASIC_ENABLE */

#if DVT_SHIFT_ENABLE
		if (verbose) printf("...lshiftTest\n");
	    [self lshiftTst: count : g1 : g2 : scr1 : scr2];

		if (verbose) printf("...rshiftTest\n");
	    [self rshiftTst: count : g1 : g2 : scr1 : scr2];

		if (verbose) printf("...rshiftwordTest\n");
	    [self rshiftwordTst: count : g1 : g2 : scr1 : scr2];
#endif /* DVT_SHIFT_ENABLE */

		if (doPrimeMod) {
#if GI_MODG_ENABLE && DVT_MOD_ENABLE
			if (verbose) printf("...modTest\n");
            [self modTst: count : g1 : g2 : scr1 : scr2];
#endif	/* DVT_MOD_ENABLE */

#if		GI_MODG_ENABLE && DVT_GCD_ENABLE
			if (verbose) printf("...gcdTest\n");
            [self gcdTst: count : &g1.g : &g2.g : &scr1.g : &scr2.g];
#endif	/* DVT_GCD_ENABLE */

#if GI_PRIME_TEST_ENABLE && DVT_PRIME_ENABLE
			if (verbose) printf("...primeTest\n");
            [self primeTst: count : &g1.g : &g2.g : &scr1.g : &scr2.g];

			if (verbose) printf("...primeSieveTest\n");
            [self primeSieveTst: count : &g1.g : &g2.g : &scr1.g : &scr2.g];
#endif	/* GI_PRIME_TEST_ENABLE && DVT_PRIME_ENABLE */

#if		DVT_INVMOD_ENABLE && GI_MODG_ENABLE && GI_INVERSE_MOD_ENABLE
			if (verbose) printf("...ginverseModTest\n");
            [self ginverseModTst: count : &g1.g : &g2.g : &scr1.g : &scr2.g];
#endif	/* DVT_INVMOD_ENABLE && GI_MODG_ENABLE */

#if		DVT_DIVMOD_ENABLE && GI_MODG_ENABLE
			if (verbose) printf("...divModTest\n");
            [self divModTst: count : &g1.g : &g2.g : &scr1.g : &scr2.g];
#endif	/* DVT_DIVMOD_ENABLE && GI_MODG_ENABLE */
		} /* doPrimeMod */
	}

	if (!quiet) printf("...giantDvt complete\n");

    free(buf);
}

- (void) testMod {
    [self main: 0 argv: NULL];
}

@end
