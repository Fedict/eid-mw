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


#include <math.h>
#include <stdlib.h>
#include "ccstats.h"

static const int verbose=0;

// Assume list has not been sorted
void export_measurement_to_file(char *file_name,measurement_t *samples,size_t len){
#if CCN_OSX
    FILE *s_file=fopen(file_name, "w");
    for (size_t i=0;i<len;i+=2)
    {
        fprintf(s_file, "%zu, %llu, %llu\n", i,samples[i].timing,samples[i+1].timing);
    }
    fclose(s_file);
#else
    (void)samples;
    (void)file_name;
    (void) len;
#endif

}

struct units dur2units(double duration)
{
    if (duration > 1.0e9) {
        struct units u = { .name = "G", .scale = 1.0e-9 };
        return u;
    }
    
    if (duration > 1.0e6) {
        struct units u = { .name = "M", .scale = 1.0e-6 };
        return u;
    }
    if (duration > 1.0e3) {
        struct units u = { .name = "K", .scale = 1.0e-3 };
        return u;
    }
    if (duration > 1.0) {
        struct units u = { .name = " ", .scale = 1.0 };
        return u;
    }
    if (duration > 1.0e-3) {
        struct units u = { .name = "m", .scale = 1.0e3 };
        return u;
    }
    if (duration > 1.0e-6) {
        struct units u = { .name = "u", .scale = 1.0e6 };
        return u;
    }
    if (duration > 1.0e-9) {
        struct units u = { .name = "n", .scale = 1.0e9 };
        return u;
    }
    struct units u = { .name = "p", .scale = 1.0e12 };
    return u;
}

/* Knuth attributes this method to B.P. Welford, Technometrics, 4,(1962), 419-420.
 While the method of computing, the number of observations n; the sum of the xi's; and the sum of the squares of the xi's, is correct in theory and will often work well enough, it is extremely vulnerable to the effects of roundoff error in computer floating point operations. It is possible to end up taking the square root of a negative number! The problem, together with a better solution, is described in Donald Knuth's "The Art of Computer Programming, Volume 2: Seminumerical Algorithms", section 4.2.2. The solution is to compute mean and standard deviation using a recurrence relation, like this:
 
 M(1) = x(1), M(k) = M(k-1) + (x(k) - M(k-1)) / k
 S(1) = 0, S(k) = S(k-1) + (x(k) - M(k-1)) * (x(k) - M(k))
 
 for 2 <= k <= n, then
 
 sigma = sqrt(S(n) / (n - 1))
 
 */

#if PRINT_IN_FILE
#include <stdio.h>
#define STAT_FILENAME "corecrypto_test_stat_%.8x%.8x.csv"
#endif

void standard_deviation_init(struct standard_deviation *sd) {
#if PRINT_IN_FILE
    char filename[strlen(STAT_FILENAME)+2*sizeof(uint64_t)];
    sprintf(filename, STAT_FILENAME, (uint32_t)((uint64_t)sd>>32),(uint32_t)sd);
    sd->s_file=fopen(filename, "w");
#endif
    sd->M = 0;
    sd->S = 0.0;
    sd->k = 0;
};

void standard_deviation_add_first(struct standard_deviation *sd, double x) {
    sd->M = x;
    sd->S = 0.0;
    sd->k = 1;
};

#define CC_STAT_START_N 0
void standard_deviation_add(struct standard_deviation *sd, double x) {
    sd->k++;
    double M = sd->M;
    sd->M = M + ((x - M) / sd->k);
    sd->S += (x - M) * (x - sd->M);

#if PRINT_IN_FILE
    fprintf(sd->s_file, "%d, %f\n", sd->k,x);
#endif
};

double standard_deviation_sigma(const struct standard_deviation *sd) {
    return sqrt(sd->S / (sd->k - 1));
};

/* Variance estimator */
CC_INLINE double s_square(const struct standard_deviation *sd) {
    return (sd->S / (sd->k - 1));
};

CC_INLINE int compare_measurements(const void *a, const void *b) {
    uint64_t x = ((const measurement_t *)a)->timing;
    uint64_t y = ((const measurement_t *)b)->timing;
    return x < y ? -1 : x == y ? 0 : 1;
}

static void rank_average_samples(measurement_t *samples,size_t len) {
    double rank_avg=1;
    size_t i=0,j;

    // Now rank the items
    while (i<len)
    {
        j=i+1;
        rank_avg=i+1; // default rank of item at index i
        while ((j<len) && (samples[i].timing==samples[j].timing))
        {
            rank_avg+=(j+1); // default rank of item at index j
            j++;
        }
        rank_avg=rank_avg/(j-i); // avg rank for items between i and j (excluded)
        while(i<j)
        {
            samples[i++].rank=rank_avg;
        }
    }

}

static double compute_rank_sum(measurement_t *samples,size_t len,uint32_t group) {
    double rank_sum=0;
    size_t i=0;

    // Now rank the item
    for (i=0;i<len;i++)
    {
        if (samples[i].group==group)
        {
            rank_sum+=samples[i].rank;
        }
    }
    return rank_sum;
}

// Rank sum test allow to compute "offset" in distribution regardless (or at least very robust) to the
// distribution
//  Null hypothesis is same distribution, no offset.
int WilcoxonRankSumTest(measurement_t *samples,size_t len) {
    double rank_sum;
    double mean,variance;

    // First sort the samples
    qsort(samples, len, sizeof(measurement_t), compare_measurements);

    // Rank (average) the measurements
    rank_average_samples(samples,len);

    // Sum of the rank for the group 0
    rank_sum=compute_rank_sum(samples,len,0);

    // Wilcoxon distribution approximates to a normal distribution for large sample
    // Probability amounts to a Z probability
    // mean = n1*(n1+n2+1)/2, here we assume both groups have same kernel. len=n1+n2.
    // variance = sqrt(n1.n2.(n1+n2+1)/12)
    // z=(sum_rank-mean)/variance;
    // P(sum_rank>W) = P(Z>z)
    // Therefore we define W' and W'' such that
    //  P(abs(sum_rank-mean)/variance) = P(Z>abs(z))=2P(Z>z)
    // Using external tool we can show that
    //   abs(sum_rank-mean)>2variance => Probability of 0.0455

    mean=len*(len+1)/4;
    variance=sqrt(len*len*(len+1)/48);

    // If off the boundary, we reject the possibility of same distribution
    if (fabs(rank_sum-mean)>2*variance) {
        if (verbose) diag("Wilcoxon Rank Sum Test failed: Rank Sum %f, Mean %f, Ratio variance %f",rank_sum,mean,fabs(rank_sum-mean)/variance);
        return 1;
    }
    else if (verbose>1) {
        diag("Wilcoxon Rank Sum Test: Rank Sum %f, Mean %f, Ratio variance %f",rank_sum,mean,fabs(rank_sum-mean)/variance);
    }
    return 0;
}

static void compute_standard_deviation(measurement_t *samples,size_t len,uint32_t group, struct standard_deviation *X) {
    size_t i=0;
    standard_deviation_init(X);
    for (i=0;i<len;i++)
    {
        if (samples[i].group==group)
        {
            standard_deviation_add(X,samples[i].timing);
        }
    }
}

#define T_TEST_DISCARD_OUTLIERS

#ifdef T_TEST_DISCARD_OUTLIERS
/* Delete outlier value using Chauvenet's criterion */
/* Return the number of discarded samples */
static uint32_t stat_discard_outliers(measurement_t *samples,size_t len, uint32_t group,struct standard_deviation *sd) {
    uint32_t i;
    double Z,sigma,mean;
    mean=sd->M;
    unsigned int k=sd->k;
    sigma=standard_deviation_sigma(sd);

    // Reset input structure
    standard_deviation_init(sd);

    // P(Z>4)=1-0.5+0.49997=0.00003  (cf Z-score cumulative tables)
    for (i=0;i<len;i++)
    {
        if (samples[i].group == group )
        {
            Z=fabs(samples[i].timing - mean)/sigma;
            if ((Z<2) || ((sigma==0) && (samples[i].timing == mean)))
            {   // Take the sample into account
                standard_deviation_add(sd,samples[i].timing);
            }
            else
            {
                if (verbose>1) diag("Discard %d, group %d, Timing %llu, Mean %f, sigma %f",i, samples[i].group, samples[i].timing, mean, sigma);
            }
        }
    }
    return (k-sd->k);
}
#endif // T_TEST_DISCARD_OUTLIERS

/* Independent two-sample t-test
 Null hypothesis is equal mean, assume equal variance.
 Works for normal disbributions mostly
 */
#define T_TEST_REJECT 4
// #define T_TEST_DISCARD_OUTLIERS
int T_test_isRejected(measurement_t *samples,size_t len) {
    double t;
    struct standard_deviation X1;
    struct standard_deviation X2;

    // Statistics over the samples
    compute_standard_deviation(samples,len,0,&X1);
    compute_standard_deviation(samples,len,1,&X2);

#ifdef T_TEST_DISCARD_OUTLIERS
    // Remove outliers
    {
    uint32_t k1=stat_discard_outliers(samples,len,0,&X1);
    uint32_t k2=stat_discard_outliers(samples,len,1,&X2);
    if ((k1>0 || k2>0) && (verbose>=1)) {
        if (verbose) diag("Discarded outliers: Group1 %d, Group2 %d, over %lu samples",k1,k2,len);
    }
    }
#endif

    // Compute the t statistic
    t=(X1.M - X2.M) / (sqrt(s_square(&X1)/X1.k)+(s_square(&X2)/X2.k));
    t=fabs(t); // Take absolute value

    // Check against critical value
#if PRINT_IN_FILE
    fprintf(X1.s_file, "avg,, %f\n", X1.M);
    fprintf(X2.s_file, "avg,, %f\n", X2.M);
    fclose(X1.s_file);
    fclose(X2.s_file);
#endif
    if (t>T_TEST_REJECT) {
        if (verbose) diag("T test failed: T %f, Avg1 %f Sigma1 %f, Avg2 %f Sigma2 %f",t,
                          X1.M,standard_deviation_sigma(&X1),
                          X2.M,standard_deviation_sigma(&X2));
        return 1;
    }
    return 0;
}

