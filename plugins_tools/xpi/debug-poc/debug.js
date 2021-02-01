function postmsg() {
  $('#testResult').html("<div class='row'><div class='col-xs-11'>eID add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-remove text-danger'></span></div></div><div class='jumbotron'><h1>Waiting for add-on...</h1><p>This should not take more than a few seconds. If it does, that means the add-on is not installed. In that case, the solution is to <a href='https://addons.mozilla.org/en-US/firefox/addon/belgium-eid/'>install the add-on</a>.</p></div>");
  if(typeof(be) !== 'undefined') {
    if(typeof(be.fedict) !== 'undefined') {
      if(typeof(be.fedict.belgiumeid) !== 'undefined') {
        if(be.fedict.belgiumeid.exists) {
          $('#testResult').html("<div class='row'><div class='col-xs-11'>eID add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-ok text-success'></span></div></div><div class='row'><div class='col-xs-11'>XPCOM API add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-ok text-warning'></span></div></div><div class='jumbotron'><h1>XPCOM add-on detected</h1><p>It appears you are using the older version of the eID add-on. If you are running Firefox 56 or below, then this is the right thing to do. However, that does mean we cannot detect whether the rest of the configuration is working properly.</p><p>Some common things to test:</p><ul><li>Is the middleware installed?</li><li>Is your cardreader installed?</li><li>Did you place the card correctly in the reader?</li></ul>");
        }
      }
    }
    $('#command').text('Redo the test');
  } else {
    $('#command').text("working...");
    window.postMessage({
      target: "belgiumeid@eid.belgium.be",
      query: "get-all"
    }, "*");
  }
}

$(document).ready(postmsg());

window.addEventListener("message", function(m) {
  if(m.source == window &&
     m.data &&
     m.data.target === "page-script") {
    $('#testResult').html("<div class='row'><div class='col-xs-11'>eID add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-ok text-success'></span></div></div><div class='row'><div class='col-xs-11'>XPCOM API add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-remove text-success'></span></div></div>");
    $("<div class='row'><div class='col-xs-11'>browser.pkcs11 API add-on installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-ok text-success'></span></div></div>").appendTo("#testResult");
    $("<div class='row'><div class='col-xs-11'>browser.pkcs11 API supported</div><div class='col-xs-1'><span class='glyphicon glyphicon-" + (m.data.message.supported ? "ok text-success" : "remove text-danger") + "'></span></div></div>").appendTo("#testResult");
    if(m.data.message.supported) {
      var slots = 0;
      $("<div class='row'><div class='col-xs-11'>eID Software installed</div><div class='col-xs-1'><span class='glyphicon glyphicon-" + (m.data.message.foundModule ? "ok text-success" : "remove text-danger") + "'></span></div></div>").appendTo("#testResult");
      if(m.data.message.slots !== null) {
        slots = m.data.message.slots.length;
        $("<div class='row'><div class='col-xs-11'>Found at least one cardreader</div><div class='col-xs-1'><span class='glyphicon glyphicon-" + (slots > 0 ? "ok text-success" : "remove text-danger") + "'></span></div></div>").appendTo("#testResult");
        if(slots > 0) {
          var foundAnyToken = false;
          for (let slot of m.data.message.slots) {
            if(slot.name !== String.raw`\\PnP\Notification`) {
              let hasToken = (slot.token !== null ? "ok text-success" : (slots > 1 ? "remove text-warning" : "remove text-danger"));
              if (hasToken === "ok text-success") {
                foundAnyToken = true;
              }
              $(`<div class='row'><div class='col-xs-11'>Found a card in card reader named ${slot.name}?</div><div class='col-xs-1'><span class='glyphicon glyphicon-${hasToken}'></span></div></div>`).appendTo("#testResult");
            }
          }
          if(slots > 1) {
            $("<div class='row'><div class='col-xs-11'>Found any card at all?</div><div class='col-xs-1'><span class='glyphicon glyphicon-" + (foundAnyToken ? "ok text-success" : "remove text-danger") + "'></span></div></div>").appendTo("#testResult");
          }
          if(!foundAnyToken) {
            $("<div class='jumbotron'><h1>No card found</h1><p>Solution: ensure the card is in the reader, and that it is properly seated.</p></div>").appendTo("#testResult");
          } else {
            $("<div class='jumbotron'><h1>Should work</h1><p>Software installed, eID card found. Did you enter the correct PIN code?</p></div>").appendTo("#testResult");
          }
        } else {
          $("<div class='jumbotron'><h1>Card reader not found</h1><p>Solution: Install the drivers of your card reader, and make sure it is properly connected. If that's all fine, try restarting Firefox.</p></div>");
        }
      } else {
        $("<div class='jumbotron'><h1>eID software not installed</h1><p>Solution: <a href='https://eid.belgium.be/en'>Install the eID Software</a></p></div>").appendTo("#testResult");
      }
    } else {
      $("<div class='jumbotron'><h1>browser.pkcs11 API not supported</h1><p>It appears you are using Firefox 57. In that version of Firefox, you must <a href='foo'>manually configure the eID software</a>. Alternatively, you may wait until Firefox 58 will be released, at which point this add-on will configure it for you.</p></div>").appendTo("#testResult");
    }
    $("#command").text("Redo the test");
  }
});
