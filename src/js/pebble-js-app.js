//processes api requests
function HTTPGET(url) {
	var req = new XMLHttpRequest();
	req.open("GET", url, false);
	req.send(null);
	return req.responseText;
}

//gets quote from and sends it to watch
var getQuote = function() {
	var response = HTTPGET("http://www.iheartquotes.com/api/v1/random?max_characters=250");
	var quote = response;
	//outputs to console for testing
	console.log(""+quote);
	//connects to specific key for communication
	var dict = {"KEY_QUOTE" : quote};
	//sends data to watch for display
	Pebble.sendAppMessage(dict);
};

//allows javascript app to listen for initial request
Pebble.addEventListener("ready",
  function(e) {
	getQuote();
  }
);

//allows javascript app to listen for following requests
Pebble.addEventListener("appmessage",
  function(e) {
	//Watch wants new data!
	getQuote();
  }
);