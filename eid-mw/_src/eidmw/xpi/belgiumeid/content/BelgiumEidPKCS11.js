/*
 * eID Middleware Project.
 * Copyright (C) 2008-2009 FedICT.
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

const nsPKCS11ContractID = "@mozilla.org/security/pkcs11;1";
const nsPKCS11ModuleDB = "@mozilla.org/security/pkcs11moduledb;1";
const nsIPKCS11ModuleDB = Components.interfaces.nsIPKCS11ModuleDB;
const nsIPKCS11Module = Components.interfaces.nsIPKCS11Module;

function BelgiumEidPKCS11() {
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
    if (getModuleLocation() == "") return true; // unsupported platform
    if (this.findModuleByName(getModuleName()) == null && this.findModuleByLibName(getModuleLocation()) == null) {
      // no installed module found - we will try to install the module
      try {
        pkcs11module = Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIPKCS11);
        try {
          pkcs11module.addModule(getModuleName(), getModuleLocation(), 0,0);
        } catch (e) {
          return false; 
        }
      } catch (e) {
        try {
          pkcs11module = Components.classes[nsPKCS11ContractID].getService(Components.interfaces.nsIDOMPkcs11);
          try {
            pkcs11module.addmodule(getModuleName(), getModuleLocation(), 0,0);
          } catch (e) {
            return false; 
          }
        } catch (f) {
          return false; 
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
  this.pkcs11ModuleAvailable = function (modulelocation) {
    /* Checks if the pkcs11 Module is available on the system
       arguments
          modulelocation: location of the PKCS11 module on the system; if null: the module location in the preferences
       returns true  if module found
               false if module not found
    */ 
    if (modulelocation == null) modulelocation = getModuleLocation();
    return searchFile(modulelocation) != null;
  }
  this.removeModuleIfNotAvailable = function () {
    /* If 
        * pkcs11 module is not available on the system
        * pkcs11 module is registered in Firefox
       Then
         remove module from PKCS11 module DB
    */
    if (!this.pkcs11ModuleAvailable(getModuleLocation())) {
      
      var module = this.findModuleByLibName(getModuleLocation());
      if (module != null) {
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
}


