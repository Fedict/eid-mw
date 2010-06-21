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
