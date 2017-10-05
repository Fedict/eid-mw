var backgroundPort;
var backgroundConnected = false;

window.addEventListener("message", function(e) {
  if (e.source == window &&
      e.data &&
      e.data.target == "belgiumeid@eid.belgium.be") {
    if (!e.data.query ||
        Object.prototype.toString.call(e.data.query) !== "[object String]") {
      return;
    }
    if(!backgroundConnected) {
      backgroundPort = browser.runtime.connect({name: "port-from-cs"});
      backgroundConnected = true;
      backgroundPort.onMessage.addListener(function(m) {
        window.postMessage({
          target: "page-script",
          message: m,
        }, "*");
      });
    }
    backgroundPort.postMessage({query: e.data.query});
  }
});
