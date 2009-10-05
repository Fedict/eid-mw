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
  return null;
}

function doPrompt(msg) {
   var prompts = Components.classes["@mozilla.org/embedcomp/prompt-service;1"].getService(Components.interfaces.nsIPromptService);
   prompts.alert(window, null, msg);
}

function notify(msg) {
   var bundle_browser = document.getElementById("belgiumeid-strings");
   var notificationBox = gBrowser.getNotificationBox();
   var buttons = [{
         label: bundle_browser.getString("ok"),
         accessKey: bundle_browser.getString("ok.accessKey"),
         callback: function() {  }
       }];

   const priority = notificationBox.PRIORITY_WARNING_MEDIUM;
   var not = notificationBox.appendNotification(msg, "belgiumeid",
                                       "chrome://browser/skin/Info.png",
                                       priority, buttons);
   not.persistence = 3;
}
