/* ***** BEGIN LICENSE BLOCK *****
 *   Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Belgium eID.
 *
 * The Initial Developer of the Original Code is
 * Koen De Causmaecker.
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 * 
 * ***** END LICENSE BLOCK ***** */

const nsPKCS11ContractID = "@mozilla.org/security/pkcs11;1";
const nsPKCS11ModuleDB = "@mozilla.org/security/pkcs11moduledb;1";
const nsIPKCS11ModuleDB = Components.interfaces.nsIPKCS11ModuleDB;
const nsIPKCS11Module = Components.interfaces.nsIPKCS11Module;

function BelgiumEidPKCS11() {
  // Private Members

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
  this.registerModule = function() {
    if (getModuleLocation() == "") return; 
    
    if (this.findModuleByName(getModuleName()) == null) {
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
       returns true  if module found
               false if module not found
    */ 
    return searchFile(modulelocation) != "";
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
}

// Helper Functions
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
  return "";
}

function doPrompt(msg) {
   var prompts = Components.classes["@mozilla.org/embedcomp/prompt-service;1"].getService(Components.interfaces.nsIPromptService);
   prompts.alert(window, null, msg);
}

var belgiumeid = {
  onLoad: function() {
    // initialization code
    this.initialized = true;
    this.strings = document.getElementById("belgiumeid-strings");
    
    beidPKCS11 = new BelgiumEidPKCS11();
    beidPKCS11.removeModuleIfNotAvailable();
    beidPKCS11.registerModule();
    
  },
};
window.addEventListener("load", function(e) { belgiumeid.onLoad(e); }, false);


