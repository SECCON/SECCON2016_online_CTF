<?php
error_reporting(0);
define("ENC_KEY", "***censored***");
define("ENC_METHOD", "aes-128-cbc");

if (!extension_loaded('pdo_sqlite')) {
    header("Content-type: text/plain");
    echo "PDO Driver for SQLite is not installed.";
    exit;
}
if (!extension_loaded('openssl')) {
    header("Content-type: text/plain");
    echo "OpenSSL extension is not installed.";
    exit;
}

/*
Setup:
CREATE TABLE user (
username VARCHAR(255),
enc_password VARCHAR(255),
isadmin BOOLEAN
);
INSERT INTO user VALUES ("admin", "***censored***", 1);
*/

function auth($enc_password, $input) {
    $enc_password = base64_decode($enc_password);
    $iv = substr($enc_password, 0, 16);
    $c = substr($enc_password, 16);
    $password = openssl_decrypt($c, ENC_METHOD, ENC_KEY, OPENSSL_RAW_DATA, $iv);
    return $password == $input;
}

function mac($input) {
    $iv = str_repeat("\0", 16);
    $c = openssl_encrypt($input, ENC_METHOD, ENC_KEY, OPENSSL_RAW_DATA, $iv);
    return substr($c, -16);
}

function save_session() {
    global $SESSION;
    $j = serialize($SESSION);
    $u = $j . mac($j);
    setcookie("JSESSION", base64_encode($u));
}

function load_session() {
    global $SESSION;
    if (!isset($_COOKIE["JSESSION"]))
        return array();
    $u = base64_decode($_COOKIE["JSESSION"]);
    $j = substr($u, 0, -16);
    $t = substr($u, -16);
    if (mac($j) !== $t)
        return array(2);
    $SESSION = unserialize($j);
}

function _h($s) {
    return htmlspecialchars($s, ENT_QUOTES, "UTF-8");
}

function login_page($message = NULL) {
?><!doctype html>
<html>
<head><title>Login</title></head>
<body>
<?php
    if (isset($message)) {
        echo "  <div>" . _h($message) . "</div>\n";
    }
?>
  <form method="POST">
    <div>
      <label>username</label>
      <input type="text" name="username">
    </div>
    <div>
      <label>password</label>
      <input type="password" name="password">
    </div>
    <input type="submit" value="login"> 
  </form>
</body>
</html>
<?php
      exit;
}

function info_page() {
    global $SESSION;
?><!doctype html>
<html>
<head><title>Login</title></head>
<body>
<?php
    printf("Hello %s\n", _h($SESSION["name"]));
    if ($SESSION["isadmin"])
        include("../flag");
?>
<div><a href="logout.php">Log out</a></div>
</body>
</html>
<?php
      exit;
}

if (isset($_POST['username']) && isset($_POST['password'])) {
    $username = (string)$_POST['username'];
    $password = (string)$_POST['password'];
    $dbh = new PDO('sqlite:../db/users.db');
    $result = $dbh->query("SELECT username, enc_password from user WHERE username='{$username}'");
    if (!$result) {
        login_page("error");
        /* DEBUG 
        $info = $dbh->errorInfo();
        login_page($info[2]);
        //*/
    }
    $u = $result->fetch(PDO::FETCH_ASSOC);
    if ($u && auth($u["enc_password"], $password)) {
        $SESSION["name"] = $u['username'];
        $SESSION["isadmin"] = $u['isadmin'];
        save_session();
        info_page();
    }
    else {
        login_page("error");
    }
}
else {
    load_session();
    if (isset($SESSION["name"])) {
        info_page();
    }
    else {
        login_page();
    }
}
