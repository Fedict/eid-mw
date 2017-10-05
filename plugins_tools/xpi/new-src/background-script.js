async function installPKCS11Module() {
  if(typeof browser.pkcs11 !== 'undefined') {
    var res;
    try {
      res = await browser.pkcs11.isModuleInstalled("beidpkcs11");
      console.log("module installed: ", res);
    } catch (err) {
      browser.notifications.onClicked.addListener(function(n) {
        browser.tabs.create({url: browser.i18n.getMessage("installUrl")});
      });
      browser.notifications.create({
        "type": "basic",
        "title": browser.i18n.getMessage("noMiddlewareFoundTitle"),
        "message": browser.i18n.getMessage("noMiddlewareFoundContent"),
      });
    }
    if(res) {
      return;
    }
    try {
      res = await browser.pkcs11.installModule("beidpkcs11", 0x1<<28);
      console.log("installModule result: ", res);
    } catch(err) {
      console.error("installModule error: ", err);
      browser.notifications.create({
        "type": "basic",
        "title": browser.i18n.getMessage("installFailedTitle"),
        "message": browser.i18n.getMessage("installFailedContent"),
      });
    }
  }
}

function connected(port) {
  async function messageReceived(msg) {
    var retval = {};
    if(typeof browser.pkcs11 === "undefined") {
      retval.supported = false;
    } else {
      retval.supported = true;
      if(msg.query === "get-status" || msg.query === "get-all") {
        try {
          retval.foundModule = await browser.pkcs11.isModuleInstalled("beidpkcs11");
        } catch (e) {
          retval.foundModule = false;
        }
      }
      if(msg.query === "get-slots" || msg.query === "get-all") {
        try {
          retval.slots = await browser.pkcs11.getModuleSlots("beidpkcs11");
        } catch(e) {
          retval.slots = null;
        }
      }
    }
    port.postMessage(retval);
  }
  port.onMessage.addListener(messageReceived);
}

installPKCS11Module();
browser.runtime.onConnect.addListener(connected);
