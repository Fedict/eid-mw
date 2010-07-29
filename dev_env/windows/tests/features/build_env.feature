Feature: Windows build environment
  In order to develop on the eID Middleware on Windows
  As a developer
  I want to install a Windows build environment
 
  Scenario: Check out from trunk and compile
    Given I am on a clean Windows system
    When I run the build_env installer script http://eid-mw.googlecode.com/svn/trunk/dev_env/windows/scripts/build_env.ps1
    Then I should be able to check out from trunk
     And I should be able to autoreconf
     And I should be able to configure and make for platform i686-w64-mingw32
     And I should be able to configure and make for platform x86_64-w64-mingw32