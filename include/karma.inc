<?php
function karma_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if ((strcasecmp($nick, "teebag") == 0)) {
		return;
	}

	// get ++ / -- pattern
	// $karma_pattern = '/(?:\+\+|--)/';
	$karma_pattern = '/([^\s]+)(\+\+|--)/';

	if (@preg_match ($karma_pattern, $text, $karma_hit)) {
		include ("db.php");
		// echo "found karma \n";
		// print_r ($karma_hit);

		// debug dump of $karma_hit from 'test--' as input
		//Array 
		//(
		//    [0] => test--
		//    [1] => test
		//    [2] => --
		//)

		$karma_term = $karma_hit[1];
		$karma = $karma_hit[2];

		// remove any additional -- or ++
		$karma_term = str_replace ("+", "", $karma_term);
		$karma_term = str_replace ("-", "", $karma_term);

		// check if its the person karmaing themselves
		if (strcasecmp($karma_term, $nick)==0) {
			$bot->privMsg($nick, "so .. yeah... like... you cant karma yourself, and .. stuff");
			return;
			}

		// get current karma / add if new
		$karma_term = mysql_real_escape_string($karma_term);
		$sql = "SELECT * FROM karma WHERE karma_word = '".$karma_term."'";
		$db_query = mysql_query($sql);
		if (mysql_errno()) { $bot->privMsg($dest, "karma_fn (): ERROR: ".mysql_errno().":".mysql_error()); }
		if ($db_query > 0) {
			if (mysql_num_rows ($db_query) > 0) {
				// retrieve old karma
				$karma_object = mysql_fetch_object($db_query);
				$karma_id = $karma_object->karma_id;
				$karma_word = $karma_object->karma_word;
				$karma_value = $karma_object->karma_value;
				}
			else
				{
				// new entry
				$sql = "INSERT INTO karma (karma_word, karma_value, karma_date) VALUES ('".$karma_term."', 0, NOW())";
				$db_query = mysql_query($sql);
				if (mysql_errno()) { $bot->privMsg($dest, "karma_fn (): ERROR: ".mysql_errno().":".mysql_error()); }
				$karma_id = mysql_insert_id();
				}
		}

		//   -update karma
		if ($karma == "--") {
			$sql = "UPDATE karma SET karma_value = karma_value-1, karma_date = NOW() WHERE karma_id=".$karma_id;
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "karma_fn (): ERROR: ".mysql_errno().":".mysql_error()); }
			}
		else
			{
			$sql = "UPDATE karma SET karma_value = karma_value+1, karma_date = NOW() WHERE karma_id=".$karma_id;
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "karma_fn (): ERROR: ".mysql_errno().":".mysql_error()); }
			}

		}
	// no match found for karma, return
	return;
	} // karma_fn

function karma_stats_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 9))) == "!topkarma") {
		include ("db.php");
		$sql = "SELECT * FROM karma ORDER BY karma_value DESC LIMIT 0,10";
		$db_query = mysql_query($sql);
		$top_10 = "[TOP 10 Karma] ";
		while ($row = mysql_fetch_object($db_query)) {
			$top_10 = $top_10 . " " . $row->karma_value . ":" . $row->karma_word . " | ";
			}
		$bot->privMsg($dest, $top_10);
		}
	else if (trim(strToLower(substr($text, 0, 3))) == "!tk") {
		include ("db.php");
		$sql = "SELECT * FROM karma WHERE ((to_days(NOW()) - to_days(karma_date)) < 30) and (karma_date <> '0000-00-00') ORDER BY karma_value DESC LIMIT 0,20";
		$db_query = mysql_query($sql);
		$top_10 = "[TOP Karma (last 30 days)] ";
		while ($row = mysql_fetch_object($db_query)) {
			$top_10 = $top_10 . " " . $row->karma_value . ":" . $row->karma_word . " | ";
			}
		$bot->privMsg($dest, $top_10);
		}
	else if (trim(strToLower(substr($text, 0, 3))) == "!bk") {
		include ("db.php");
		$sql = "SELECT * FROM karma WHERE ((to_days(NOW()) - to_days(karma_date)) < 30) and (karma_date <> '0000-00-00') ORDER BY karma_value ASC LIMIT 0,20";
		$db_query = mysql_query($sql);
		$top_10 = "[BOTTOM Karma (last 30 days)] ";
		while ($row = mysql_fetch_object($db_query)) {
			$top_10 = $top_10 . " " . $row->karma_value . ":" . $row->karma_word . " | ";
			}
		$bot->privMsg($dest, $top_10);
		}
	else if (trim(strToLower(substr($text, 0, 9))) == "!botkarma") {
		include ("db.php");
		$sql = "SELECT * FROM karma ORDER BY karma_value ASC LIMIT 0,10";
		$db_query = mysql_query($sql);
		$top_10 = "[BOTTOM 10 Karma] ";
		while ($row = mysql_fetch_object($db_query)) {
			$top_10 = $top_10 . " " . $row->karma_value . ":" . $row->karma_word . " | ";
			}
		$bot->privMsg($dest, $top_10);
		}
	else if (trim(strToLower(substr($text, 0, 6))) == "!karma") {
		include ("db.php");
		$karma_term = trim(substr($text, 7));
		$karma_term = mysql_real_escape_string($karma_term);
		$sql = "SELECT * FROM karma WHERE karma_word = '".$karma_term."'";
		$db_query = mysql_query($sql);
		if (mysql_num_rows ($db_query) == 0) {
			$bot->privMsg ($dest, $karma_term . " has 0 karma.");
			}
		else
			{
			$karma_row = mysql_fetch_object($db_query); 
			$bot->privMsg($dest, $karma_term . " has " . $karma_row->karma_value . " karma."); 
			}
		}
	else if (trim(strToLower(substr($text, 0, 3))) == "!rk") {
		include ("db.php");
		$karma_term = mysql_real_escape_string(trim(substr($text, 4)));
		$sql = "SELECT * FROM karma WHERE karma_word LIKE '%".$karma_term."%' ORDER BY karma_value DESC";
		$db_query = mysql_query($sql);
		$rk_list = "";
		while (($row = mysql_fetch_object($db_query)) && (strlen($rk_list) < 400)) {
			$rk_list = $rk_list . " " . $row->karma_word . ":" . $row->karma_value . " | ";
			}
		$bot->privMsg($dest, $rk_list);
		}
	else
		return;
	}
?>
