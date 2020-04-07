# 
#  Copyright (c) 2015 Apple Inc. All rights reserved.
#  
#  corecrypto Internal Use License Agreement
#  
#  IMPORTANT:  This Apple corecrypto software is supplied to you by Apple Inc. ("Apple")
#  in consideration of your agreement to the following terms, and your download or use
#  of this Apple software constitutes acceptance of these terms.  If you do not agree
#  with these terms, please do not download or use this Apple software.
#  
#  1.	As used in this Agreement, the term "Apple Software" collectively means and
#  includes all of the Apple corecrypto materials provided by Apple here, including
#  but not limited to the Apple corecrypto software, frameworks, libraries, documentation
#  and other Apple-created materials. In consideration of your agreement to abide by the
#  following terms, conditioned upon your compliance with these terms and subject to
#  these terms, Apple grants you, for a period of ninety (90) days from the date you
#  download the Apple Software, a limited, non-exclusive, non-sublicensable license
#  under Apple’s copyrights in the Apple Software to make a reasonable number of copies
#  of, compile, and run the Apple Software internally within your organization only on
#  devices and computers you own or control, for the sole purpose of verifying the
#  security characteristics and correct functioning of the Apple Software; provided
#  that you must retain this notice and the following text and disclaimers in all
#  copies of the Apple Software that you make. You may not, directly or indirectly,
#  redistribute the Apple Software or any portions thereof. The Apple Software is only
#  licensed and intended for use as expressly stated above and may not be used for other
#  purposes or in other contexts without Apple's prior written permission.  Except as
#  expressly stated in this notice, no other rights or licenses, express or implied, are
#  granted by Apple herein.
#  
#  2.	The Apple Software is provided by Apple on an "AS IS" basis.  APPLE MAKES NO
#  WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED WARRANTIES
#  OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, REGARDING
#  THE APPLE SOFTWARE OR ITS USE AND OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS,
#  SYSTEMS, OR SERVICES. APPLE DOES NOT WARRANT THAT THE APPLE SOFTWARE WILL MEET YOUR
#  REQUIREMENTS, THAT THE OPERATION OF THE APPLE SOFTWARE WILL BE UNINTERRUPTED OR
#  ERROR-FREE, THAT DEFECTS IN THE APPLE SOFTWARE WILL BE CORRECTED, OR THAT THE APPLE
#  SOFTWARE WILL BE COMPATIBLE WITH FUTURE APPLE PRODUCTS, SOFTWARE OR SERVICES. NO ORAL
#  OR WRITTEN INFORMATION OR ADVICE GIVEN BY APPLE OR AN APPLE AUTHORIZED REPRESENTATIVE
#  WILL CREATE A WARRANTY. 
#  
#  3.	IN NO EVENT SHALL APPLE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL
#  OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
#  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING
#  IN ANY WAY OUT OF THE USE, REPRODUCTION, COMPILATION OR OPERATION OF THE APPLE
#  SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING
#  NEGLIGENCE), STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#  
#  4.	This Agreement is effective until terminated. Your rights under this Agreement will
#  terminate automatically without notice from Apple if you fail to comply with any term(s)
#  of this Agreement.  Upon termination, you agree to cease all use of the Apple Software
#  and destroy all copies, full or partial, of the Apple Software. This Agreement will be
#  governed and construed in accordance with the laws of the State of California, without
#  regard to its choice of law rules.
#  
#  You may report security issues about Apple products to product-security@apple.com,
#  as described here:  https://www.apple.com/support/security/.  Non-security bugs and
#  enhancement requests can be made via https://bugreport.apple.com as described
#  here: https://developer.apple.com/bug-reporting/
#  
#  EA1350
#  10/5/15
# 
from aetypes import Enum


sRSAmodulus = "# RSA modulus n:"
sMessage = "# Message to be signed:"
sQinv = "# CRT coefficient qInv:"
sSig = "# Signature:"
sSalt = "# Salt:"
tags = [(sRSAmodulus, ".modulus"),
        ("# RSA public exponent e:", ".e"),
        ("# RSA private exponent d:", ".d"),
        ("# Prime p:", ".p"),
        ("# Prime q:", ".q"),
        ("# p\'s CRT exponent dP:", ".dp"),
        ("# q\'s CRT exponent dQ:", ".dq"),
        (sQinv, ".qinv"),
        (sMessage, ".msg"),
        (sSalt, ".salt"),
        ("# Signature:", ".sig")
        ]


class state(Enum):
    read = 1
    number = 2


class data_state(Enum):
    param_start = 1
    ex_start = 2
    param_end = 3
    ex_end = 5
    param = 6
    example = 7


st = state.read
data_st = data_state.param_start
value = ""
curr_tag = tags[0]
prev_tag = curr_tag
n_mod = 0
n_msg = 0
fin = open("rsassa-pss-test.txt", "r")
fot = open("rsa-pss-test.inc", "w")

for line in fin:
    if st == state.read:
        for tag in tags:
            if (tag[0].lower() in line.lower()):
                st = state.number
                value = ""
                curr_tag = tag

elif st == state.number:
    l = line.strip()
        if l:
            value = value + "\\x"+l.strip().replace(" ", "\\x")
    else:
        
        value = value.strip()
            a = curr_tag[1] + "=\"" + value + "\""
            b = curr_tag[1] + "_len=" + str(len(value)/4)
            st = state.read
            
            p = a + ", " + b
            
            if curr_tag[0] == sRSAmodulus:
                n_mod+=1
                print "number of messages = ", n_msg
                n_msg =0;
                if prev_tag[0] == sSig:
                    p = "}},\n{" + p + ","
                else:
                    p = "{" + p + ","
            elif curr_tag[0] == sQinv:
                p = "\n"+ p + ","
            elif curr_tag[0] == sMessage:
                n_msg +=1
                if prev_tag[0] == sQinv:
                    p = "\n\t\t{{" + p + ","
                elif prev_tag[0] == sSig:
                    p = ",\n\t\t{" + p + ","
        elif curr_tag[0] == sSig:
            p = "\n\t\t"+p + "\n\t\t}"
            elif curr_tag[0] == sSalt:
                p = "\n\t\t"+p+","

else:
    p = "\n"+p + ","
        
        prev_tag = curr_tag
            fot.write(p)

fot.write("}}")
fot.write("\n\nnumber of modulus="+str(n_mod))
print "number of modulus = ", n_mod
fot.close()
fin.close()
