//
//  PKCS11Tests.m
//  PKCS11Tests
//
//  Created by buildslave on 28/09/17.
//

#import <XCTest/XCTest.h>
#include "unix.h"
#include "pkcs11.h"
#include "testlib.h"

@interface PKCS11Tests : XCTestCase

@end

@implementation PKCS11Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

-(void)testInitFinalize {
	XCTAssert(init_finalize()==TEST_RV_OK);
}
-(void)testGetinfo {
	XCTAssert(getinfo()==TEST_RV_OK);
}
-(void)testFunclist {
	XCTAssert(funclist()==TEST_RV_OK);
}
-(void)testSlotlist {
	XCTAssert(slotlist()==TEST_RV_OK);
}
-(void)testSlotinfo {
	XCTAssert(slotinfo()==TEST_RV_OK);
}
-(void)testTkinfo {
	XCTAssert(tkinfo()==TEST_RV_OK);
}
/*addTest(double_init)
addTest(fork_init)
addTest(slotevent)
addTest(mechlist)
addTest(mechinfo)
addTest(sessions)
addTest(sessions_nocard)
addTest(sessioninfo)
addTest(slogin)
addTest(nonsensible)
addTest(objects)
addTest(readdata)
addTest(readdata_sequence)
addTest(digest)
addTest(threads)
addTest(sign)
addTest(sign_state)
addTest(decode_photo)
addTest(ordering)
addTest(wrong_init)*/

@end
