<?
$db_host="localhost";
$db_user="bot_db_user";
$db_pass="bot_db_user_password";
$db_db="bot_db";

$db=mysql_pconnect($db_host,$db_user,$db_pass) or die("unable to connect to database");
mysql_select_db($db_db, $db) or die("unable to select database $db_db");
?>
