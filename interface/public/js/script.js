// Function to motor direction
$(document).ready(function() {

  // Set pin 6 as output
  $.get('/wildfire/mode/6/o');

  // Handle buttons
  $("#1").click(function() {
    $.get('/wildfire/digital/6/1');
  });

  $("#2").click(function() {
    $.get('/wildfire/digital/6/0');
  });

});