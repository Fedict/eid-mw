# 
#  Copyright (c) 2010,2011,2015 Apple Inc. All rights reserved.
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

#!/bin/sh

perftests_all() {
    $ccperf
}

usage() {
    echo "Usage: `basename $0` [-a] [-n|-H] [-d|-i] [-u] [-b] [-s sdk] [-p]" 
    echo "    -a    Build native and device and run perf tests on both."
    echo "    -b    Build (and run unit tests)"
    echo "    -u    Run unit test (useful when not building)"
    echo "    -p    Run perf tests"
    echo "    -n    Native host"
    echo "    -i    iOS device"
    echo "    -s    sdk for ios device (implies -b -i)"
    echo "    -P devicePassword  - defaults to 'alpine'"
    echo "    -F passwordFile    - file containing device password"
    exit 2
}

# initialize default option values
do_build=0
do_ios=0
do_native=0
do_unit=0
do_perf=0
do_verbose=0
device_password_file=~/.devicepass
device_password=alpine
ios_sdk=iphoneos.internal
# now parse command line options
test $# -eq 0 && usage
args=`getopt abdHinps:t:uv $*`
test $? != 0 && usage
set -- $args
for i
do
    case "$i"
    in
    -a)
        do_ios=1
        do_native=1
        do_build=1
        do_perf=1
        shift;;
    -b)
        do_build=1
        shift;;
    -d|-i)
        do_ios=1
        shift;;
    -H|-n)
        do_native=1
        shift;;
    -p)
        do_perf=1
	shift;;
    -s)
        ios_sdk="$2"; shift;
        do_ios=1
        do_build=1
        shift;;
    -t)
        target="--target $2"
	shift;;
    -u)
        do_unit=1
        shift;;
    -v)
        do_verbose=1
        shift;;
    -P)
	device_password=$2;
	device_password_file="";
	shift 2;;
    -F)
	device_password_file="$2";
	device_password="";
	shift 2;;
    --)
        shift; break;;
    esac
done

device_password_file_option="";
if [ -r "$device_password_file" ]; then
    RSYNC_OPTS="--password-file $device_password_file"
    device_password="";
fi

if [ "$device_password" != "" ]; then
    export RSYNC_PASSWORD="$device_password"
fi

# Build phase
if [ $do_build -ne 0 -a $do_native -ne 0 ]; then
    echo "******** Native host build ******** "
    xcodebuild -configuration Release -target world build > /dev/null
fi

if [ $do_build -ne 0 -a $do_ios -ne 0 ]; then
    echo "******** iOS Internal build ******** "
    xcodebuild -configuration Release -sdk "$ios_sdk" -target world build > /dev/null
fi

# Get build dirs
ios_build_dir="`xcodebuild '@@@@$(BUILT_PRODUCTS_DIR)' -configuration Release -sdk "$ios_sdk" 2>&1 | grep '@@@@' | sed 's/^.*@@@@\(.*\).$/\1/' | uniq`"
ios_build_dir="`eval cd \"$ios_build_dir\" && pwd`"
echo "ios_build dir is \"$ios_build_dir\""

native_build_dir="`xcodebuild '@@@@$(BUILT_PRODUCTS_DIR)' -configuration Release 2>&1 | grep '@@@@' | sed 's/^.*@@@@\(.*\).$/\1/' | uniq`"
native_build_dir="`eval cd \"$native_build_dir\" && pwd`"
echo "native_build dir is \"$native_build_dir\""

print_size()
{
	size -m "$1" | awk '/\(for architecture (.*)\)/ { printf("(%s ", $4); } /Segment __(DATA|TEXT)/ { label=$2; } /^\ttotal/ { printf("%s %s ", label, $2); } END { printf("\n"); }'
}
# Size phase
if [ $do_build -ne 0 -a $do_native -ne 0 ]; then
    echo "ec_verify  only sizes \c"
    print_size "$native_build_dir/ec_verify"
    echo "rsa_verify only sizes \c"
    print_size "$native_build_dir/rsa_verify"
fi

if [ $do_build -ne 0 -a $do_ios -ne 0 ]; then
    echo "ec_verify  only sizes \c"
    print_size "$ios_build_dir/ec_verify"
    echo "rsa_verify only sizes \c"
    print_size "$ios_build_dir/rsa_verify"
fi

# Validation phase
if [ $do_unit -ne 0 -a $do_native -ne 0 ]; then
    echo "******** Native host unit tests ******** "
    arch=`arch`
    lipo -extract $arch -output /tmp/otest-$arch /Developer/Tools/otest
    DYLD_FRAMEWORK_PATH=/Developer/Library/Frameworks  OBJC_DISABLE_GC=YES /tmp/otest-$arch "$native_build_dir/validation.octest"
    #"$native_build_dir/CoreCrypto.app/Contents/MacOS/CoreCrypto"
    rm /tmp/otest-$arch
fi

if [ $do_unit -ne 0 -a $do_ios -ne 0 ]; then
    rsync $RSYNC_OPTS -rlpt "$ios_build_dir/validation.octest" rsync://root@localhost:10873/root/tmp > /dev/null
    echo "******** iOS Internal unit tests ******** "
    pe="`xcrun -sdk \"$ios_sdk\" -find PurpleExec`"
    $pe $target --stream --cmd /Developer/Tools/otest /tmp/validation.octest
fi

# Perf test phase
if [ $do_perf -ne 0 -a $do_native -ne 0 ]; then
    echo "******** Native host perf tests ******** "
    ccperf="$native_build_dir/ccperf"
    perftests_all
fi

if [ $do_perf -ne 0 -a $do_ios -ne 0 ]; then
    rsync $RSYNC_OPTS -rlpt "$ios_build_dir/ccperf" rsync://root@localhost:10873/root/tmp > /dev/null
    echo "******** iOS \"$ios_sdk\" Internal perf tests ******** "
    pe="`xcrun -sdk \"$ios_sdk\" -find PurpleExec`"
    ccperf="$pe $target --stream --cmd /tmp/ccperf"
    perftests_all
fi
