Feature: Website log on
  In order to log on a website with my eID
  As a Firefox user
  I want to install see a box to ask for my PIN
 
  Scenario: Warn when eID middleware is unavailable
    Given the eID middleware is not installed
      And The eID add-on is installed in Firefox 
    When I start Firefox
    Then I should see an alert warning that the middleware is unavailable

  Scenario: Hide warning about eID middleware unavailability
    Given the eID middleware is not installed
      And The eID add-on is installed in Firefox 
	  And The preference "showmodulenotfoundnotification" is not set
    When I start Firefox
     And I should see an alert warning that the middleware is unavailable
     And I click on the button "Don't show again"
    Then The alert should disappear 
     And I should never see the alert again 
    
  Scenario: Log on to a website with my eID
    Given My card reader is connected
      And I have Firefox running
      And The eID add-on is installed in Firefox 
      And My card is inserted in the reader
    When I go to "https://eidmw.yourict.net/test.html"
    And I select a certificate
	And I enter my PIN code "1234"
	Then I should see a web page with the text "success!"
