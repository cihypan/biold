<?
function get_riddle (&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 14))) == "!riddle answer") {
	
	$last_id=get_last_riddle_id();
	
	$db_query = "SELECT * FROM brainteaser WHERE teaser_id=".$last_id;
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "answer_riddle(): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_send_query > 0) {
		$question_obj = mysql_fetch_object($db_send_query);

		$teaser_answer = $question_obj->teaser_answer;
		
		$teaser_answer = wordwrap($teaser_answer);
		
		$teaser_answer_array = explode ("\n", $teaser_answer);
		$teaser_answer_array[0] = "[Answer to last riddle ...]".$teaser_answer_array[0];
		
		for ($i=0;$i<=count($teaser_answer_array);$i++) {
			$bot->privMsg($dest, $teaser_answer_array[$i]);
			}		
		}
	return;		
	}
		
   if (trim(strToLower(substr($text, 0, 7))) == "!riddle") {
	$db_query = "SELECT teaser_id FROM brainteaser ORDER BY RAND() LIMIT 1";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "get_riddle(): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_send_query > 0) {
		$row_obj = mysql_fetch_object($db_send_query);
		$teaser_id = $row_obj->teaser_id;
		}
	
	$db_query = "SELECT * FROM brainteaser WHERE teaser_id=".$teaser_id;
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "get_riddle(): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_send_query > 0) {
		$question_obj = mysql_fetch_object($db_send_query);

		$teaser_question = $question_obj->teaser_question;
		last_riddle_id($teaser_id);
		
		$teaser_question = wordwrap ($teaser_question);
		
		$teaser_question_array = explode ("\n", $teaser_question);
		$teaser_question_array[0] = "[Riddle Me This ...]".$teaser_question_array[0];
		
		for ($i=0;$i<=count($teaser_question_array);$i++) {
			$bot->privMsg($dest, $teaser_question_array[$i]);
			}
		}		
	return;
	}	
}	

function last_riddle_id ($id) {
	$sql = "UPDATE teaser_current SET last_id=$id";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "last_riddle_id(): ERROR: ".mysql_errno().":".mysql_error(); }
}

function get_last_riddle_id () {
	$sql = "SELECT * FROM teaser_current";
       	$select = @mysql_query ($sql);
       	if (mysql_errno()) { echo "get_last_riddle_id(): ERROR: ".mysql_errno().":".mysql_error(); }
       	if ($select > 0) {
		$row = mysql_fetch_object($select);
		return $row->last_id;
	}
}
?>
