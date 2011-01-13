/*
 * eID Middleware Project.
 * Copyright (C) 2008-2011 FedICT.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version
 * 3.0 as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, see
 * http://www.gnu.org/licenses/.
 */

if(!be) var be={};
if(!be.fedict) be.fedict={};
if(!be.fedict.belgiumeidpkcs11) be.fedict.belgiumeidpkcs11={};


be.fedict.belgiumeidpkcs11.getPrefs = function() {
    var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
    return prefs.getBranch("extensions.belgiumeid.");
  };
be.fedict.belgiumeidpkcs11.getPKCS11ModuleDB = function() {
    const nsPKCS11ModuleDB = "@mozilla.org/security/pkcs11moduledb;1";
    const nsIPKCS11ModuleDB = Components.interfaces.nsIPKCS11ModuleDB;
    return Components.classes[nsPKCS11ModuleDB].getService(nsIPKCS11ModuleDB);
  };
be.fedict.belgiumeidpkcs11.getModuleLocation = function() {
    return be.fedict.belgiumeidpkcs11.getPrefs().getCharPref("modulelocation");
  };
be.fedict.belgiumeidpkcs11.getModuleName = function() {
    return be.fedict.belgiumeidpkcs11.getPrefs().getCharPref("modulename");
  };
be.fedict.belgiumeidpkcs11.errorLog = function(aMessage) {
	var consoleService = Components.classes["@mozilla.org/consoleservice;1"]
                                 .getService(Components.interfaces.nsIConsoleService);
	consoleService.logStringMessage("Belgium eID: " + aMessage);
  };
be.fedict.belgiumeidpkcs11.notifyModuleNotFound = function() {
    var notificationBox = gBrowser.getNotificationBox();
	var strings = document.getElementById("belgiumeid-strings");

    var buttons = [{
         label: strings.getString("downloadmiddleware"),
         accessKey: strings.getString("downloadmiddleware.accessKey"),
         callback: function() { gBrowser.selectedTab = gBrowser.addTab(strings.getString("urldownloadmiddleware")); }
       },
       {
         label: strings.getString("donotshowagain"),
         accessKey: strings.getString("donotshowagain.accessKey"),
         callback: function () {  return be.fedict.belgiumeidpkcs11.shouldShowModuleNotFoundNotification(false);}
       }];
    const priority = notificationBox.PRIORITY_WARNING_MEDIUM;
    var not = notificationBox.appendNotification(strings.getString("modulenotfoundonsystem"), "belgiumeid",
                                       "chrome://browser/skin/Info.png",
                                       priority, buttons);
    not.persistence = 3;
  };
be.fedict.belgiumeidpkcs11.slotFound = function(modulename){
	// searches for module with the given name
	// and returns true if the module and a slot is found
	// otherwise, returns false
	var found;
	var belgiumEidPKCS11Module = be.fedict.belgiumeidpkcs11.findModuleByName(modulename)
	try {
	   slot = belgiumEidPKCS11Module.listSlots().currentItem();
	   found = true;
	} catch (e)
	{
		found = false;
	}
	return found;
  };
  
try {
  be.fedict.belgiumeidpkcs11.addModule = 
	Components.classes["@mozilla.org/security/pkcs11;1"].getService(Components.interfaces.nsIPKCS11).addModule;
}
catch (e) {
  // pre-firefox 3.5 interface
  be.fedict.belgiumeidpkcs11.addModule = 
    Components.classes["@mozilla.org/security/pkcs11;1"].getService(Components.interfaces.nsIDOMPkcs11).addmodule;
}
be.fedict.belgiumeidpkcs11.registerModule = function() {
    /* 
       Registers PKCS11 module
       returns
          true   if no modulelocation is found in the preferences
                 -or- if the module is already found
          false  if the registration failed
    */
    const PKCS11_PUB_READABLE_CERT_FLAG  =  0x1<<28; //Stored certs can be read off the token w/o logging in
    if (be.fedict.belgiumeidpkcs11.getModuleLocation() == "") {
		be.fedict.belgiumeidpkcs11.errorLog("belgiumeid.modulelocation is not set");
		return true; // unsupported platform
	}
    var installSucceeded = false;
	var moduleLocations = be.fedict.belgiumeidpkcs11.getModuleLocation().split(";");
	for (x in moduleLocations) {
		if (be.fedict.belgiumeidpkcs11.findModuleByLibName(moduleLocations[x]) == null) {
			// we can't install a module twice. 
			try {
				// PKCS11_PUB_READABLE_CERT_FLAG: we instruct FF not to ask for PIN when reading certificates
				// This only makes sense when the CKF_LOGIN_REQUIRED flag is set on the token in P11 module
				var modulename = be.fedict.belgiumeidpkcs11.getModuleName() + " - " + moduleLocations[x];
				be.fedict.belgiumeidpkcs11.addModule(modulename, moduleLocations[x], PKCS11_PUB_READABLE_CERT_FLAG,0);
				be.fedict.belgiumeidpkcs11.errorLog("Added PKCS11 module " + modulename + " Location: " + moduleLocations[x]);
				installSucceeded = true;
				break;
			} catch (e) {
				be.fedict.belgiumeidpkcs11.errorLog("Failed to load module " + moduleLocations[x] + 
						": Error " + e.name + ": " + e.message);
				continue;
			}
		} 
	}
	return installSucceeded;
  };
be.fedict.belgiumeidpkcs11.findModuleByName = function (modulename) {
    /* Looks in the list of registered PKCS11 modules for a module with a specified name
       return module if found
              null if not found
    */
    try {
      return be.fedict.belgiumeidpkcs11.getPKCS11ModuleDB().findModuleByName(modulename);
    } catch (e) {
		be.fedict.belgiumeidpkcs11.errorLog("module with name " + modulename + " not found");
      return null;
    }
  };
be.fedict.belgiumeidpkcs11.findModuleByLibName = function (modulelibname) {
    /* Looks in the list of registered PKCS11 modules for a module with a specified system path
       return module if found
              null if not found
    */
	const nsIPKCS11Module = Components.interfaces.nsIPKCS11Module;
    var modules = be.fedict.belgiumeidpkcs11.getPKCS11ModuleDB().listModules();
    try {
      modules.isDone();
    } catch (e) { 
      return null;
    }
    while (true) {
      var module = modules.currentItem().QueryInterface(nsIPKCS11Module);
      if (module) {
        if (module.libName == modulelibname) {
          return module;
        }
      }
      try {
        modules.next();
      } catch (e) { 
        break;
      }
    }
    return null;
  };
be.fedict.belgiumeidpkcs11.shouldShowModuleNotFoundNotification = function (shouldshow) {
    if (shouldshow != undefined) {
      be.fedict.belgiumeidpkcs11.getPrefs().setBoolPref("showmodulenotfoundnotification", shouldshow);
    }
    return be.fedict.belgiumeidpkcs11.getPrefs().getBoolPref("showmodulenotfoundnotification");
  };
be.fedict.belgiumeidpkcs11.setDefaultPrefs = function() {
    if (be.fedict.belgiumeidpkcs11.defaultPrefs.moduleLocation) {
		var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
		var defaultBelgiumeidPrefs = prefs.getDefaultBranch("extensions.belgiumeid.");
		defaultBelgiumeidPrefs.setCharPref("modulelocation",be.fedict.belgiumeidpkcs11.defaultPrefs.moduleLocation);
	}
  };

window.addEventListener("load", function(e) { 
  var beidPKCS11 = be.fedict.belgiumeidpkcs11;
	beidPKCS11.setDefaultPrefs();
	if (!beidPKCS11.registerModule()) {
	  if (beidPKCS11.shouldShowModuleNotFoundNotification()) {
	    beidPKCS11.notifyModuleNotFound();   
      }
    }
}, false);
