Given /^the eID middleware is not installed$/ do
#  pending # express the regexp above with the code you wish you had
 # require 'ruby-wmi'
 # apps = WMI::Win32_Products.find(:all)
  
 # apps.each do |app|
 #   app.properties_.each do |p|
 #     puts "#{p.name}: #{app[p.name]}"
 #   end
 # end
   File.exists?("c:\\windows\\system32\\beidpkcs11.dll").should == false
end

Given /^The eID add\-on is installed in Firefox$/ do
  #pending # express the regexp above with the code you wish you had
  checkinstall = "var gExtensionManager = Components.classes['@mozilla.org/extensions/manager;1']"
  checkinstall += ".getService(Components.interfaces.nsIExtensionManager);"
  checkinstall += "gExtensionManager.getItemForID('belgiumeid@eid.belgium.be').id;"
  @browser.execute_script(checkinstall).should == "belgiumeid@eid.belgium.be"
end
					
When /^I start Firefox$/ do
   @browser.close_all
   @browser = nil
   @browser ||= FireWatir::Firefox.new(:profile => "xpitest", :waitTime => 3)  
   @browser.should_not == nil
end

Then /^I should see an alert warning that the middleware is unavailable$/ do
  checknotification = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator(null);
  var message = "";
  while (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	try { 
  		thismessage = w.document.getElementById('content')
  			.getNotificationBox().getNotificationWithValue('belgiumeid').label; 
  		message = thismessage;
  	} catch(e) { }
  } print(message);
EOS
  @browser.execute_script(checknotification).should == "The configuration of the Belgian electronic identity card has failed. Is the eID Middleware installed?"
end

When /^I click on the button "([^\"]*)"$/ do |label|
  clickbutton  = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator(null);
  var message = "";
  while (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	try { 
  		var anonymousnodes = w.document.getAnonymousNodes(w.document.getElementById('content'));
		var buttonfound = false;
  		for (i=0; i < anonymousnodes.length; i++) {
			var buttons = anonymousnodes.item(i).getElementsByTagName("button");
			for (j=0; j<buttons.length; j++) {
				if (buttons.item(i).label === "#{label}") { 
					buttons.item(i).click();
					buttonfound = true;
					break;
				}
			}
			if (buttonfound)
				break;
		}
		if (buttonfound)
			break;
  	} catch(e) { }
  }
  if (buttonfound)
		print("button found");
  else
		print("button not found");
EOS
  @browser.execute_script(clickbutton).should == "button found"
end

Then /^The alert should disappear$/ do

  checknotification = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator(null);
  var message="";
  while (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	try { 
  		thismessage = w.document.getElementById('content')
  			.getNotificationBox().getNotificationWithValue('belgiumeid').label; 
  		message = thismessage;
  	} catch(e) { }
  } print(message);
EOS
  @browser.execute_script(checknotification).should_not == "The configuration of the Belgian electronic identity card has failed. Is the eID Middleware installed?"

end

Then /^I should never see the alert again$/ do
   @browser.close
   @browser = nil
   @browser ||= FireWatir::Firefox.new(:profile => "xpitest", :waitTime => 3)  
   
  checknotification = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator(null);
  var message = "";
  while (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	try { 
  		thismessage = w.document.getElementById('content')
  			.getNotificationBox().getNotificationWithValue('belgiumeid').label; 
  		message = thismessage;
  	} catch(e) { }
  } print(message);
EOS
  @browser.execute_script(checknotification).should_not == "The configuration of the Belgian electronic identity card has failed. Is the eID Middleware installed?"


end

Given /^My card reader is connected$/ do
  context = Smartcard::PCSC::Context.new(:system)
  readers = context.readers
  context.release
  readers.length.should > 0
end

Given /^I have Firefox running$/ do
  @browser ||= FireWatir::Firefox.new(:profile => "xpitest", :waitTime => 3)  
  @browser.should_not == nil
end

Given /^My card is inserted in the reader$/ do
  context = Smartcard::PCSC::Context.new(:system)
  readers = context.readers
  while reader = readers.pop
	card = context.card(reader, :shared)
  end
end

Given /^The preference "([^\"]*)" is not set$/ do |setting|
  checknotification = <<EOS
  var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
  prefs.getBranch("extensions.belgiumeid.").clearUserPref("#{setting}");
EOS
  @browser.execute_script(checknotification)
end

When /^I go to "([^\"]*)"$/ do |url|
  goto  = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator("navigator:browser");

  if (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	w.openURL("#{url}");
  }
EOS
  @browser.execute_script(goto)
  sleep(5) # as we don't want to wait for the page being loaded, as the logon is part of the pageload
end

When /^I select a certificate$/ do
  findbelpictoken  = <<EOS
  var windowManager = Components.classes['@mozilla.org/appshell/window-mediator;1']
  	.getService(Components.interfaces.nsIWindowMediator);
  windows = windowManager.getEnumerator(null);
  var message = "";
  var belpictokenfound = false;

  while (windows.hasMoreElements()) {
  	var w = windows.getNext();
  	try { 
  		var anonymousnodes = w.document.getAnonymousNodes(w.document.getElementById('nicknames'));
		belpictokenfound = false;
  		for (i=0; i < anonymousnodes.length; i++) {
			var menuitems = w.document.getElementsByTagName("menuitem");
			for (j=0; j<menuitems.length; j++) {
				if (menuitems.item(i).label.indexOf("BELPIC") == 0) { 
					belpictokenfound = true;
					w.document.getElementById("certAuthAsk").acceptDialog();
					break;
				}
			}
			if (belpictokenfound)
				break;
		}
		if (belpictokenfound)
			break;
  	} catch(e) { }
  }
  if (belpictokenfound)
		print("belpictoken found");
  else
		print("belpictoken not found");
EOS
  @browser.execute_script(findbelpictoken).should == "belpictoken found" 
  sleep (2)
end

When /^I enter my PIN code "([^\"]*)"$/ do |pin|
  fnFindWindowEx = Win32API.new('user32.dll','FindWindowEx', ['l', 'l', 'p', 'p'], 'l')
  fnSendMessage  = Win32API.new('user32.dll','SendMessage', ['l', 'l', 'l', 'p'], 'l')
  fnSetActiveWindow = Win32API.new('user32.dll','SetActiveWindow', ['l'], 'l')
  hwndPIN = fnFindWindowEx.call(0, 0, nil, "Entrez PIN")
  if (hwndPIN == 0) then
	hwndPIN = fnFindWindowEx.call(0, 0, nil, "Geef PIN")
	if (hwndPIN == 0) then
      hwndPIN = fnFindWindowEx.call(0, 0, nil, "Enter PIN")
	end
  end
  hwndPIN.should_not == 0
  # set pin
  hwndPINField = fnFindWindowEx.call(hwndPIN, 0, "EDIT", "")
  hwndPINField.should_not == 0
  fnSendMessage.call(hwndPINField, 0x000c, 0, pin)  # WM_SETTEXT: 0x000c
  
  # click OK
  hwndPINOK = fnFindWindowEx.call(hwndPIN, 0, "BUTTON", "&OK")
  hwndPINOK.should_not == 0
  fnSetActiveWindow.call(hwndPIN)
  fnSendMessage.call(hwndPINOK, 0x00F5, 0, 0)  # BM_CLICK: 0x00F5
  
end

Then /^I should see a web page with the text "([^\"]*)"$/ do |text|
  sleep(3)    # we wait for the page being loaded
  @browser.text.should == text
  
end
