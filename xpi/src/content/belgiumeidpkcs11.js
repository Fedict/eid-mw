/*
 * eID Middleware Project.
 * Copyright (C) 2008-2010 FedICT.
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

function BELGIUMEIDPKCS11(){
  const nsPKCS11ContractID = "@mozilla.org/security/pkcs11;1";
  const nsPKCS11ModuleDB = "@mozilla.org/security/pkcs11moduledb;1";
  const nsIPKCS11ModuleDB = Components.interfaces.nsIPKCS11ModuleDB;
  const nsIPKCS11Module = Components.interfaces.nsIPKCS11Module;
  PKCS11_PUB_READABLE_CERT_FLAG  =  0x1<<28; //Stored certs can be read off the token w/o logging in

  // Private Members
  var strings = document.getElementById("belgiumeid-strings");

  // Private Functions
  function getPrefs () {
    var prefs = Components.classes["@mozilla.org/preferences-service;1"]
                    .getService(Components.interfaces.nsIPrefService);
    return prefs.getBranch("extensions.belgiumeid.");
  }
  function getPKCS11ModuleDB() {
    return Components.classes[nsPKCS11ModuleDB].getService(nsIPKCS11ModuleDB);
  }
  function getModuleLocation() {
    return getPrefs().getCharPref("modulelocation");
  }
  function getModuleName() {
    return getPrefs().getCharPref("modulename");
  }
  function fileExists(filename) {
    var file = Components.classes["@mozilla.org/file/local;1"].
                       createInstance(Components.interfaces.nsILocalFile);
    try {
      file.initWithPath(filename);
    } catch (e) {
      return false;
    }
    return file.exists();
  }
  function searchFile(filename) {
    // searches for a file at certain folders.
    // returns "" if file not found
    // returns full path if file found
    if (fileExists(filename)) {
      return filename;
    }
  
    //Windows System32
    try {
      var winpath = Components.classes["@mozilla.org/file/directory_service;1"].
                       getService(Components.interfaces.nsIProperties).
                       get("WinD", Components.interfaces.nsIFile);
      winpath.append("system32");
      winpath.append(filename);
      if (winpath.exists()) return winpath.path;
    } catch (e) {
    }
    return null;
  }
  function errorLog(aMessage) {
	var consoleService = Components.classes["@mozilla.org/consoleservice;1"]
                                 .getService(Components.interfaces.nsIConsoleService);
	consoleService.logStringMessage("Belgium eID: " + aMessage);
  }
  
  // Public Functions
  this.notifyModuleNotFound = function() {
    var notificationBox = gBrowser.getNotificationBox();
    var buttons = [{
         label: strings.getString("downloadmiddleware"),
         accessKey: strings.getString("downloadmiddleware.accessKey"),
         callback: function() { gBrowser.selectedTab = gBrowser.addTab(strings.getString("urldownloadmiddleware")); }
       },
       {
         label: strings.getString("donotshowagain"),
         accessKey: strings.getString("donotshowagain.accessKey"),
         callback: function () { beidPKCS11.shouldShowModuleNotFoundNotification(false); }
       }];
    const priority = notificationBox.PRIORITY_WARNING_MEDIUM;
    var not = notificationBox.appendNotification(strings.getString("modulenotfoundonsystem"), "belgiumeid",
                                       "chrome://browser/skin/Info.png",
                                       priority, buttons);
    not.persistence = 3;
  } 
  this.registerModule = function() {
    /* 
       Registers PKCS11 module
       returns
          true   if no modulelocation is found in the preferences
                 -or- if the module is already found
          false  if the registration failed
    */

    if (getModuleLocation() == "") {
		errorLog("belgiumeid.modulelocation is not set");
		return true; // unsupported platform
	}
    

    if (this.findModuleByName(getModuleName()) == null) {
		// no installed module found - we will try to install the module
		var moduleLocations = getModuleLocation().split(";");

		for (x in moduleLocations) {
			try {
				pkcs11module = Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIPKCS11);
				try {
					// PKCS11_PUB_READABLE_CERT_FLAG: we instruct FF not to ask for PIN when reading certificates
					// This only makes sense when the CKF_LOGIN_REQUIRED flag is set on the token in P11 module
					pkcs11module.addModule(getModuleName(), moduleLocations[x], PKCS11_PUB_READABLE_CERT_FLAG,0);
					errorLog("added PKCS11 module " + getModuleName() + "Location: " + moduleLocations[x]);
					return true;
				} catch (e) {
				    errorLog("failed to load module" + moduleLocations[x]);
					continue; 
				}
			} catch (e) {
				try {
					pkcs11module = Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIDOMPkcs11);
					try {
						// PKCS11_PUB_READABLE_CERT_FLAG: we instruct FF not to ask for PIN when reading certificates
						// This only makes sense when the CKF_LOGIN_REQUIRED flag is set on the token in P11 module
						pkcs11module.addmodule(getModuleName(), moduleLocations[x], PKCS11_PUB_READABLE_CERT_FLAG,0);
						errorLog("added PKCS11 module " + getModuleName() + "Location: " + moduleLocations[x]);
						return true;
					} catch (e) {
						errorLog("failed to load module" + moduleLocations[x]);
						// module not found, try the next
						continue; 
					}
				} catch (f) {
					return false; 
				}
			}
		}
    }
    return true;
  }
  this.unregisterModule = function (modulename) {
    /* Unregisters module from PKCS11 module list */
    if (this.findModuleByName(modulename) != null) {
      try {
        Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIPKCS11).deleteModule(modulename);
      } catch (e) {
        try {
          pkcs11module = Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIDOMPkcs11).deletemodule(modulename);
        } catch (f) {
           // error log "failed to delete module"
           return false; 
        }
      }
    }
    return true;
  }

  this.findModuleByName = function (modulename) {
    /* Looks in the list of registered PKCS11 modules for a module with a specified name
       return module if found
              null if not found
    */
    try {
      return getPKCS11ModuleDB().findModuleByName(modulename);
    } catch (e) {
		errorLog("module with name " + modulename + " not found");
      return null;
    }
  }
  this.findModuleByLibName = function (modulelibname) {
    /* Looks in the list of registered PKCS11 modules for a module with a specified system path
       return module if found
              null if not found
    */
    var modules = getPKCS11ModuleDB().listModules();
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
  }
  this.removeOldModuleIfNotAvailable = function () {
    /* When 
        pkcs11 module is registered in Firefox
		 and it is an old module
		 and the libary does not exist on the system
       Then
         remove module from PKCS11 module DB	
	*/
	var oldLibraries = [
		"/usr/local/lib/beid-pkcs11.bundle",                                    // OSX default
		"/usr/local/lib/beid-pkcs11.bundle/Contents/MacOS/libbeidpkcs11.dylib", // OSX alternative
		"/usr/local/lib/libbeidpkcs11.so"                                       // Linux default
																				// For now, the windows library location does not change
		];
    var module = this.findModuleByName(getModuleName());
    if (module != null) {
	    // there is already a module with the name we use
		if (oldLibraries.indexOf(module.libName) != -1) {
			// the registered module is using an old library
			if (!fileExists(module.libName))
			errorLog("PKCS11 library for " + getModuleName() + " is unavailable. We delete the module");
			this.unregisterModule(module.name);
		}
    }
  }

  this.shouldShowModuleNotFoundNotification = function (shouldshow) {
    if (shouldshow != undefined) {
      getPrefs().setBoolPref("showmodulenotfoundnotification", shouldshow);
    }
    return getPrefs().getBoolPref("showmodulenotfoundnotification");
  }

  // initialization code
  this.initialized = true;
  this.removeOldModuleIfNotAvailable();
  if (!this.registerModule()) {
	if (this.shouldShowModuleNotFoundNotification()) {
		this.notifyModuleNotFound();   
    }
 }
};

window.addEventListener("load", function(e) { BELGIUMEIDPKCS11(e); }, false);
