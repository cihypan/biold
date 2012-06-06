<?php
function get_stfu (&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 5))) == "!stfu") {
	$stfu_who = trim(substr($text, 6));

        $db_query = "SELECT stfu_id FROM stfu ORDER BY RAND() LIMIT 1"; 
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "get_stfu (): ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0) {
		$stfu_object = mysql_fetch_object($db_send_query);
		$stfu_id = $stfu_object->stfu_id;
		}

        $db_query = "SELECT stfu_text FROM stfu WHERE stfu_id=".$stfu_id;
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "get_stfu (): ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0) {
		$stfu_object = mysql_fetch_object($db_send_query);
		$stfu_text = $stfu_object->stfu_text;
		}

	$bot->privMsg($dest, "Yeah..".$stfu_who." ".$stfu_text);

	return;		
	}
}
?>
