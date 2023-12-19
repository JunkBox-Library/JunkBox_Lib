<?php

define ('GEO_TOP_PATH',     dirname(__FILE__));
define ('GEO_BASE_NAME',    basename(GEO_TOP_PATH));

define ('GEO_DATA_PATH',    '/home/apache/gsigo/hokusou');      // データのディレクトリ

define ('GEO_MAP_WIDTH',    '256');     // 一区画のデータの横幅のポイント数
define ('GEO_MAP_HEIGHT',   '256');     // 一区画のデータの縦のポイント数
define ('GEO_SEAMAP_DATA', '-9999');    // マップ上での海や川，池などの標高*10

// MAP DATA
define ('GEO_HEAD_LINE',    '0');       // ヘッダの行数．読み飛ばす．
define ('GEO_DATA_HEAD',    '0');       // 先頭データの開始位置（バイト-1）
define ('GEO_DATA_LEN',     '5');       // 一個のデータの長さ
define ('GEO_DATA_MAG',     '0.2');     // 高さの調整．1/5
define ('GEO_MAP_FAC',      '10');      // データの標高の倍率．この倍率でファイルに保存されている．
define ('GEO_MAP_XYRATE',   '1');       // データの縦横比．

// OpenSim
define ('GEO_SEA_DEEP',    '-40');      // 海の深さ．GEO_SEAMAP_DATA の変換先 (m)
define ('GEO_NOMAP_DATA',  '-40');      // マップデータのない場所の代替標高 (m)
define ('GEO_SEA_LEVEL',    '20');      // OpenSimの seaレベルのデフォルト (m)

?>
