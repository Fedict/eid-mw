var modname = "beidpkcs11";
var oldmodname = "beidpkcs11_old";
async function installPKCS11Module() {
  if(typeof browser.pkcs11 !== 'undefined') {
    var res;
    try {
      res = await browser.pkcs11.isModuleInstalled(oldmodname);
      console.log("old module installed: ", res);
      if(res) {
        browser.pkcs11.uninstallModule(oldmodname);
        console.log("removed old module");
      }
    } catch (err) {
      console.log("could not check for/remove old module: ");
      console.log(err);
    }
    try {
      var platform = await browser.runtime.getPlatformInfo();
      if(platform.os === "win") {
        if(platform.arch === "x86-32") {
          modname = "beidpkcs11_32";
        } else {
          modname = "beidpkcs11_64";
        }
      }
      res = await browser.pkcs11.isModuleInstalled(modname);
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
      return;
    }
    if(res) {
      return;
    }
    try {
      res = await browser.pkcs11.installModule(modname, 0x1<<28);
      console.log("installModule result: ", res);
    } catch(err) {
      console.error("installModule error: ", err);
      browser.pkcs11.isModuleInstalled("beidp11kit").then(() => {
        modname = "beidp11kit";
        console.log("found p11-kit-proxy module, assuming BeID installed through there");
      }).catch(() => {
        browser.notifications.create({
          "type": "basic",
          "title": browser.i18n.getMessage("installFailedTitle"),
          "message": browser.i18n.getMessage("installFailedContent"),
        })
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
          retval.foundModule = await browser.pkcs11.isModuleInstalled(modname);
        } catch (e) {
          retval.foundModule = false;
        }
      }
      if(msg.query === "get-slots" || msg.query === "get-all") {
        try {
          var slots = await browser.pkcs11.getModuleSlots(modname);
          retval.slots = [];
          for(slot of slots) {
            if(slot.token === null || slot.token.name === "BELPIC") {
              retval.slots.push(slot);
            }
          }
          if(retval.slots.length === 0) {
            retval.slots = null;
          }
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
