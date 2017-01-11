<?php
define("ENC_KEY", "!Wnkgjka\xa7-HJ*>U!");
define("ENC_METHOD", "aes-128-cbc");

$password = "AHei9wa4nlywtio";
$iv = "Jo\x91Gwe&J\xf2>G#\xdd.X[";

$enc_password = $iv . openssl_encrypt($password, ENC_METHOD, ENC_KEY, true, $iv);
$enc_password = base64_encode($enc_password);
echo $enc_password . "\n";
