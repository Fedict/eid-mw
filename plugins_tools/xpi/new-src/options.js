async function findReaders() {
  let modnames = ["beidpkcs11", "beidpkcs11_32", "beidpkcs11_64", "beidpkcs11_alt"];
  let slots = undefined;
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
  } else {
    innerHTML = "(none)";
  }
  ul.innerHTML = innerHTML;
}
document.addEventListener("DOMContentLoaded", findReaders);
document.querySelector("#retry").addEventListener("click", findReaders);
