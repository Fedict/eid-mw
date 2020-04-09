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


#include "cccycles.h"

#if (CC_CYCLE_SELECT==CC_CYCLE_KPERF_FIX)
/* Use fix register, shared with power management */

// Return the current value of the timer.
uint64_t KPC_ReadTime(void)
{
    static uint32_t init=0;
    uint64_t counters[64];

    if (init==0)
    {
        kpc_set_counting(KPC_CLASS_FIXED_MASK);
        kpc_set_thread_counting(KPC_CLASS_FIXED_MASK);
        init=1;
    }

    kpc_get_thread_counters(0, 64, counters);
#if defined(__arm64__) || defined(__arm__)
    return counters[0];
#else
    return counters[1];
#endif
}

#elif (CC_CYCLE_SELECT==CC_CYCLE_KPERF_CONFIG)
/* Use configurable counter */

#if defined(__arm64__) || defined(__arm__)
#define CORE_CYCLE 0x2
static  uint64_t config[] = {
    CORE_CYCLE,
    0,
    0,
    0,
    0,
    0
};
#else // Intel
#define IA32_EVENT_UNHALTED_CORE_UMASK     0x00
#define IA32_EVENT_UNHALTED_CORE_EVENT     0x3c
#define IA32_EVENT_INST_RET_UMASK     0x00
#define IA32_EVENT_INST_RET_EVENT     0xc0

#define IA32_EVTSEL_EVENT_MASK        (0xff)
#define IA32_EVTSEL_EVENT_SHIFT          (0)
#define IA32_EVTSEL_UMASK_MASK      (0xff00)
#define IA32_EVTSEL_UMASK_SHIFT          (8)
#define IA32_EVTSEL_USR_MASK       (0x10000)
#define IA32_EVTSEL_EN_MASK       (0x400000)

#define USER_CYCLES (IA32_EVENT_UNHALTED_CORE_EVENT << IA32_EVTSEL_EVENT_SHIFT) \
| (IA32_EVENT_UNHALTED_CORE_UMASK << IA32_EVTSEL_UMASK_SHIFT) \
| IA32_EVTSEL_USR_MASK \
| IA32_EVTSEL_EN_MASK
#define USER_INSTRS (IA32_EVENT_INST_RET_EVENT << IA32_EVTSEL_EVENT_SHIFT) \
| (IA32_EVENT_INST_RET_UMASK << IA32_EVTSEL_UMASK_SHIFT) \
| IA32_EVTSEL_USR_MASK \
| IA32_EVTSEL_EN_MASK

static uint64_t config[] = {USER_CYCLES,
    0, 0, 0, 0, 0, 0, 0};
#endif // Arm or Intel

// Return the current value of the timer.
uint64_t KPC_ReadTime(int *error)
{
    static uint64_t counters[64];
    bool init=false;
    int r;

    if (!init)
    {
        // Disable kpc whitelist to work with clpc. Otherwise kpc cycle counter always reports 0.
#ifdef __arm64__
        int disable = 1;
        r = sysctlbyname("kpc.disable_whitelist", NULL, 0, &disable, sizeof(disable));
        if (r) {
            printf("Failed to set kpc.disable_whitelist\n" );
            *error=-1;
        }
#endif // __arm64__

        /* program configurable counters */
        r = kpc_set_config(KPC_CLASS_CONFIGURABLE_MASK,
                           config);
        if (r) {
            printf("Failed to set config - Test must run as Root\n" );
            *error=-1;
        }
        kpc_set_counting(KPC_CLASS_CONFIGURABLE_MASK);
        kpc_set_thread_counting( KPC_CLASS_CONFIGURABLE_MASK);
    }
    r = kpc_get_thread_counters(0, 64, counters);
    if (r)
    {
        printf("Failed to read cycle counter\n" );
        *error=-1;
    }
    return counters[0];
}

#endif // Time source


