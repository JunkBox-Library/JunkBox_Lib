<?php

//require_once './config.php';
//require_once '../func.php';


// default
$lat = 36.0;
$lon = 140.0;

$width  = 32;
$height = 32;
$center = 0;


if (isset($_GET["lat"])) {
    $lat = $_GET["lat"];
}
if (isset($_GET["lon"])) {
    $lon = $_GET["lon"];
}

if (isset($_GET["width"])) {
    $width = $_GET["width"];
}
if (isset($_GET["height"])) {
    $height = $_GET["height"];
}
if (isset($_GET["center"])) {
    $center = $_GET["center"];
}

if (!is_numeric($lat)) error_proc("Lat Start");
if (!is_numeric($lon)) error_proc("Lon Start");
if (!is_numeric($width)) error_proc("Width");
if (!is_numeric($height)) error_proc("Height");
if (!is_numeric($center)) error_proc("Center");


//
header("Content-type: image/jpeg");
header("Transfer-Encoding: chunked");
header("Connection: close");

//
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$result = socket_connect($socket, "202.26.159.140", 8080);

//error_log("GET $lats $lons $late $lone");
$com = "GET $lon $lat $width $height $center";
socket_write($socket, $com, strlen($com));

while($out = socket_read($socket, 4096)) {
    error_log($out);
    echo $out;
}
?>
