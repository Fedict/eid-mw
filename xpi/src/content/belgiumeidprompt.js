function belgiumeidprompt() {

  var infobody = document.getElementById("info.body");

  // only change labels when asked for a BELPIC PIN
  if (infobody.textContent.search("BELPIC") != -1) {
    // look for main window to get the localisation strings
    var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"]
                   .getService(Components.interfaces.nsIWindowMediator);
    var mainWindow = wm.getMostRecentWindow("navigator:browser");
    var strings = mainWindow.document.getElementById("belgiumeid-strings");


    var infoicon = document.getElementById("info.icon");
    var commondialog = document.getElementById("commonDialog");
    
    // set icon
    infoicon.setAttribute("src", "chrome://belgiumeid/skin/icon40x40.png");
    // replace "Enter master password" label with a more explanatory label
    infobody.textContent = strings.getString("enterpin");
    // replace dialog title
    commondialog.setAttribute("title",  strings.getString("enterpin"));
  }
}

// add on load event handler
window.addEventListener("load", function() { belgiumeidprompt(); }, false);
