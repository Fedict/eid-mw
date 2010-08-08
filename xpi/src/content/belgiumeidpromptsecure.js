function belgiumeidpromptsecure() {

  var tokenName = document.getElementById("tokenName");

  // only change labels when asked for a BELPIC PIN
  //if (tokenName.value === "BELPIC") {
    // look for main window to get the localisation strings
    var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"]
                   .getService(Components.interfaces.nsIWindowMediator);
    var mainWindow = wm.getMostRecentWindow("navigator:browser");
    var strings = mainWindow.document.getElementById("belgiumeid-strings");
    var protectedAuth = document.getElementById("protectedAuth");

    // hide the default elements
    for (i=0; i < protectedAuth.childNodes.length; i++) {
      protectedAuth.childNodes[i].setAttribute("hidden", true);
    }  

    var hbox = document.createElement("hbox");
    protectedAuth.appendChild(hbox);
    hbox.setAttribute("style", "margin: 5px; min-width: 35em; max-width: 70em;");

    // set icon
    var icon = document.createElement("image");
    icon.setAttribute("src", "chrome://belgiumeid/skin/icon40x40.png");
    icon.setAttribute("class", "spaced");
    hbox.appendChild(icon);

    // add help text
    var vbox = document.createElement("vbox");
    hbox.appendChild(vbox);
    var enterpinsecure = document.createElement("description");
    enterpinsecure.textContent = strings.getString("enterpinsecure");
    vbox.appendChild(enterpinsecure);
    var enterpinsecuredescription = document.createElement("description");
    enterpinsecuredescription.textContent = strings.getString("enterpinsecuredescription");
    vbox.appendChild(enterpinsecuredescription);
    
    // chang dialog title
    protectedAuth.setAttribute("title",  strings.getString("enterpinsecuretitle"));
  //}
}

// add on load event handler
window.addEventListener("load", function() { belgiumeidpromptsecure(); }, false);
