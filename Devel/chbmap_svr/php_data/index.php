<?php

require_once './config.php';
//require_once '../func.php';


$size = 256;
$name = "";
$mgnf = 1.0;
$seal = GEO_SEA_LEVEL;
$rate = GEO_MAP_XYRATE;


if (isset($_GET["size"])) {
    $size = $_GET["size"];
}
if (isset($_GET["name"])) {
    $name = $_GET["name"];
}
if (isset($_GET["mag"])) {
    $mgnf = $_GET["mag"];
}
if (isset($_GET["sea"])) {
    $seal = $_GET["sea"];
}
if (isset($_GET["rate"])) {
    $rate = $_GET["rate"];
}

$deg = explode("x", $name);
$lat = $deg[0];
$lon = $deg[1];


if (!ctype_digit($size)) error_proc("size.");
if (!ctype_digit($seal)) error_proc("sea level.");
if (!is_numeric ($mgnf)) error_proc("magnification.");
if (!is_numeric ($rate)) error_proc("X Y rate.");
if (!is_numeric ($lat))  error_proc("latitude.");
if (!is_numeric ($lon))  error_proc("longitude.");
if ($size<=0)   $size = 256;
if ($mgnf<=0.0) $mgnf = 1.0;
if ($rate<=0.0) $rate = GEO_MAP_XYRATE;

$mgnf = $mgnf*GEO_DATA_MAG;
$hsize = $wsize = $size;
if ($rate!=1.0) {
    $fsize = $wsize*$rate;
    $hsize = (int)$fsize;
    if ($fsize-$hsize>0) $hsize++;
}


//
$socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
$result = socket_connect($socket, "202.26.159.140", 8081);

$com = "GET $lon $lat $hsize $wsize";
socket_write($socket, $com, strlen($com));

while($ldat = socket_read($socket, 4*$size)) {
    $num = unpack("f".$size, $ldat);
    for ($i=1; $i<=$size; $i++) {
        $h = $num[$i];
        if ($h==GEO_SEAMAP_DATA) $h = GEO_SEA_DEEP*$mgnf + $seal;   // ³¤¤Ï GEO_SEA_DEEP ¶Ñ°ì
        else $h = $h/GEO_MAP_FAC*$mgnf + $seal;
        print(" ".$h);
    }
    print("\n");
}

?>
