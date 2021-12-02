async function findReaders() {
  let modnames = ["beidpkcs11", "beidpkcs11_32", "beidpkcs11_64", "beidpkcs11_alt"];
  let slots = undefined;
  var modfound = document.querySelector("#middlewarefound");
  var modvers = document.querySelector("#middlewareversion");
  for(modname of modnames) {
    try {
      slots = await browser.pkcs11.getModuleSlots(modname);
      break;
    } catch(e) {
    }
  }
  var ul = document.querySelector("#readerlist");
  ul.innerHTML = "";
  var innerHTML = "";
  if(slots !== undefined) {
    for(slot of slots) {
      innerHTML += "<li>" + slot.name;
      if(slot.token !== null) {
        innerHTML += ": eID found";
      } else {
        innerHTML += ": <strong>no</strong> eID found";
      }
      innerHTML += "</li>";
    }
    modfound.innerHTML = "yes";
    let versionInfo = await browser.storage.managed.get("versionInfo");
    versionInfo = versionInfo.versionInfo;
    modvers.innerHTML = "" + versionInfo.major + "." + versionInfo.minor + "." + versionInfo.patch;
  } else {
    innerHTML = "(none)";
    modfound.innerHTML = "no";
    modvers.innerHTML = "(not found)";
  }
  ul.innerHTML = innerHTML;
}
document.addEventListener("DOMContentLoaded", findReaders);
document.querySelector("#retry").addEventListener("click", findReaders);
