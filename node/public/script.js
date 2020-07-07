listOfMovie = [];
var hostIp = self.location.host
var wsHostIp = "ws://" + hostIp.split(':')[0] + ":8082"
console.log("host ip is : " + wsHostIp)

var port = new osc.WebSocketPort({
  url: wsHostIp
});

port.on("ready", function () {
  console.log("OSC ready");
  refreshPlayslit();
});

var createMovie = function (args) {

  var movie = {};

  movie.name = args[0];
  movie.index = args[1];
  return movie;

};

port.on("message", function (oscMessage) {

  switch (oscMessage.address) {
    case "/addMovie":
      console.log("message recu addMovie", oscMessage);
      var movie = createMovie(oscMessage.args);
      listOfMovie.push(movie);
      $("#list").append(htmlDivElement(movie));
      createPlayCallback(movie);

      break;
    case "/playIndex":
      console.log("message recu play Index", oscMessage);
      var idName = "#card" + String(oscMessage.args[0]);
      $("#list").children().each(function (index) {
        $(this).removeClass("bg-dark text-light");
      });
      $(idName).addClass("bg-dark text-light");

      break;

    case "/playPercentage":
      $("#timeline").val(oscMessage.args[0]);
      break;

    default:
      break;
  }
});

port.open();

var sendOscMessage = function (oscAddress, arg) {
  port.send({
    address: oscAddress,
    args: [arg]
  });

  console.log("message OSC envoyé");
};



$(document).ready(function () {

  $("#timeline").val(0);

  $("#playlistbutton").click(function () {
    refreshPlayslit();
  });


});

function htmlDivElement(movie) {

  var html = "";
  html = "<div class='container-xl card divFilm my-3' id='card" + String(movie.index) + "'>";
  html += " <div class='row'>"
  html += "<div class='col-5 text-center'>"
  html += "<img src='vignette/" + movie.index + ".jpg' class='vignette' alt='...'></img>";
  html += "</div><div class='col-4 text-center py-5'>"
  html += "  <h5 class='card-title'>"
  html += movie.name;
  html += "</h5></div>"
  html += "<div class='col-3 text-center py-5'>"
  html += "<button class='btn btn-primary btn-lg' id='movie" + String(movie.index) + "' >";
  html += "Lecture";
  html += "</button>";
  html += "</div>";
  html += "</div>";

  /*
  html += "<div class='divIndex'>";
  html += movie.index;
  html += "</div>";
  html += "<div class='divTitle' id='title" + String(movie.index) + "'>";
  html += movie.name;
  html += " ( " + movie.duration + " ) ";
  */

  return html;

}

function createPlayCallback(movie) {

  var buttonName = "#movie" + String(movie.index);
  console.log("create callback buttonName: " + movie.name);
  $(buttonName).click(function () {

    console.log("lecture du film : " + String(movie.index) + " : " + movie.name);
    sendOscMessage("/player/selectIndex", movie.index);
    //changer la chose pour que les films aient un indentifiant unique
    // qui ne soit pas basé sur l'index dans le tableau.
    $("#list").children().children("div").children("button").removeClass("btn-light");
    $("#list").children().children("div").children("button").addClass("btn-primary");


    $(this).addClass("btn-light");

  });

}

function refreshPlayslit() {
  console.log("Refresh playlist");
  $("#list").empty();
  listOfMovie = [];
  sendOscMessage("/player/refreshPlaylist", 1);
}



