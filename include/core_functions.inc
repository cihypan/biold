<?
function identifyNickServ(&$bot, $nick, $ident, $host, $dest, $text) {
  if ($nick == "NickServ" && $dest == ircConf::get("botNick") &&
  eregi("This nick is owned", $text)) {
    $bot->privMsg("NickServ", "identify ".ircConf::get("botNickServPass"));
  }
}

function secureOps(&$bot, $nick, $ident, $host, $channel, $user) {
  /** We have to ignore ops by the bot, she seems to have problems
      with autoOps, she deops straight after auto-op'ing. */
  if (in_array(ircConf::get("botNick"), array($nick, $user))) return;
  if (!ircConf::isUser($nick, $ident, $host)) { // be strict :P
    $bot->write("MODE ".ircConf::get("channel")." -oo $user $nick");
  }
}

function help_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 5))) == "!help") {
	$help_text = "[Help] Commands available are : !ticker, !ecn, !translate, !convert, !weather, !fweather, !forecast, !url, !ctof, !ftoc, !calc, !search, !last, !nlast, !urban, !riddle, !thes, !dict, !wiki, !ip, !topquotes, !botquotes, !rq, !stfu"; 
	$bot->privMsg($dest, $help_text);

	$help_text = "[Help Fantasy Stock Market Game]  !buy, !sell, !portfolio, !market, !stocks (msg only)";
	$bot->privMsg($dest, $help_text);

	$help_text = "[Help Trivia Game]  !trivia, !trivia score, !trivia categories";
	$bot->privMsg($dest, $help_text);

	$help_text = "[Help Karma]  !tk, !bk, !topkarma, !botkarma, !rk";
	$bot->privMsg($dest, $help_text);
	}
}

function help_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   // stock quotes... yay...
   if (trim(strToLower(substr($text, 0, 5))) == "!help")
        {
        $help_text = "[Help] Commands available are : !ecn, !ticker, !lticker, !translate, !convert, !weather, !fweather, !forecast, !url, !horoscope, !search, !last, !nlast, !gnews, !ugnews";

        $bot->privMsg($nick, $help_text);
        }
}

function checkMode(&$bot, $nick, $ident, $host, $channel, $modeData) {
  $bot->privMsg($channel, "received $modeData ($nick)");
}

function updateTopic(&$bot, $nick, $ident, $host, $channel, $topic) {
  /* insert code to update topic to db here */
  $bot->privMsg($channel, "$nick changed the topic to $topic");
}

function adminControl(&$bot, $nick, $ident, $host, $us, $text) {
  global $score_arr, $answered_trivia_arr;
  
  if (ircConf::isUser($nick, $ident, $host, "admin")) {
    if ($text == "quit") {
		$bot->close($nick." wants me to leave :-(");
		exit;
    } elseif ($text == "join") {
      		$bot->write("JOIN ".ircConf::get("channel"));
    } elseif (trim(strToLower(substr($text, 0, 13))) == "!trivia reset") {
    	
		$nick_to_reset = trim(substr($text, 14));
		trivia_score_reset($nick_to_reset);
		
		if (empty($nick_to_reset))
			$bot->privMsg($nick, "Okay, reset all trivia scores.");
		else
			$bot->privMsg($nick, "Okay, reset ".$nick_to_reset." score to 0.");
			
    } elseif (trim(strToLower(substr($text, 0, 5))) == "!help") {
    		$bot->privMsg($nick, "[Admin commands] join | quit | !trivia reset");
    } else {
      // $bot->privMsg($nick, "Valid commands:");
      // $bot->privMsg($nick, "join | quit");
    }
  }
}

function greetOnJoin(&$bot, $nick, $ident, $host, $channel) {
  if ($nick !== ircConf::get("botNick")) {
    if (ircConf::isUser($nick, $ident, $host)) {
      $bot->write("MODE $channel +o $nick");
    }
    $bot->privMsg($channel, "Hello $nick, welcome to $channel :)");
  }
}

function seen_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 9))) == "!seenlast") {
		// TODO: guess could do some stats on last 10 seen, most recent, oldest ?
		$db_query = "SELECT *, (unix_timestamp(now())-unix_timestamp(seen_date)) as secsbetween FROM seen ORDER BY seen_date desc LIMIT 0,10 ";
		$db_send_query = mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "seen_fn() ERROR: ".mysql_errno().":".mysql_error()); }
		
		if ($db_send_query > 0) {
			while ($row = mysql_fetch_object($db_send_query)) {
				$seen_name = $row->seen_name;
				$seen_ago = $row->secsbetween;
				$seen_ago_txt = time_to_string_short($seen_ago);

				$seen_last_txt = $seen_last_txt . $seen_name . " (" . $seen_ago_txt . ") | ";
				}
			$bot->privMsg($dest, $seen_last_txt);
			}
		}
	elseif (trim(strToLower(substr($text, 0, 5))) == "!seen") {
		$seen_who = trim(substr($text, 6));
		$seen_who_safe = mysql_real_escape_string($seen_who);
	
		$db_query = "SELECT *, (unix_timestamp(now())-unix_timestamp(seen_date)) as secsbetween FROM seen WHERE seen_name = '".$seen_who_safe."' "; 
		$db_send_query = @mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "seen_fn () ERROR: ".mysql_errno().":".mysql_error()); }
		if ($db_send_query > 0) {
			$seen_object = mysql_fetch_object($db_send_query);
			$seen_id = $seen_object->seen_id;
			$seen_date = $seen_object->seen_date;
			$seen_ident = $seen_object->seen_ident;
			$seen_saying = $seen_object->seen_saying;
			$seen_ago = $seen_object->secsbetween;
			$seen_ago_txt = time_to_string($seen_ago);
			if (strlen($seen_saying) > 1) {
				$seen_saying = "Saying : ".$seen_saying;
				}

			$bot->privMsg($dest, $seen_who." (".$seen_ident.") was here ".$seen_ago_txt." ago.".$seen_saying );
			}
		else
			{
			$bot->privMsg($dest, "I haven't seen ".$seen_who." :(");
			}
		}
}

function lastSeen(&$bot, $nick, $ident, $host, $channelOrQuit = null) {
	if ($nick == ircConf::get("botNick")) return;
	//  $bot->privMsg(ircConf::get("channel"), "Last seen for $nick ($channelOrQuit)");
	// print ("[DEBUG] entered lastSeen() nick [$nick] | channelOrQuit [$channelOrQuit]\n");

	// just in case, i just know ppl like zacs will try and break this :)
	//
	$name_safe = mysql_real_escape_string($nick);

	$nick_ident = $ident . "@" . $host;
	// sigh, just in case...
	$nick_ident = mysql_real_escape_string($nick_ident);

	$sql = "SELECT * FROM seen WHERE seen_name = '".$name_safe."'";
	// print ("[DEBUG] sending [$sql]\nnick_ident is [$nick_ident]\n");

	$db_query = mysql_query($sql);
	if (mysql_errno()) { $bot->privMsg($dest, "lastSeen (): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_query > 0) {
		if (mysql_num_rows ($db_query) > 0) {
			// print ("[DEBUG] updating entry in lastSeen\n");
			// update the information if already there 
			$seen_object = mysql_fetch_object($db_query);
			$seen_id = $seen_object->seen_id;
			$seen_nick = $seen_object->seen_name;
			$seen_date = $seen_object->seen_date;
			$seen_ident = $seen_object->seen_ident;
			// now that we have old info, lets update
			$sql = "UPDATE seen SET seen_date=NOW(), seen_ident='".$nick_ident."' WHERE seen_id='".$seen_id."' ";
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "lastSeen (): ERROR: ".mysql_errno().":".mysql_error()); }
			}
		else
			{
			// print ("[DEBUG] new entry in lastSeen\n");
			// new entry
			$sql = "INSERT INTO seen (seen_name, seen_date, seen_ident) VALUES ('".$name_safe."', NOW(), '".$nick_ident."' )";
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "lastSeen (): ERROR: ".mysql_errno().":".mysql_error()); }
			// $karma_id = mysql_insert_id();
			}
	}
}

function lastSeen_msg(&$bot, $nick, $ident, $host, $dest, $text) {
	if ($nick == ircConf::get("botNick")) return;
	//  $bot->privMsg(ircConf::get("channel"), "Last seen for $nick ($channelOrQuit)");
	// print ("[DEBUG] entered lastSeen() nick [$nick] | channelOrQuit [$channelOrQuit]\n");

	// just in case, i just know ppl like zacs will try and break this :)
	//
	$name_safe = mysql_real_escape_string($nick);

	$nick_ident = $ident . "@" . $host;
	// sigh, just in case...
	$nick_ident = mysql_real_escape_string($nick_ident);

	// must ;0
	$text_safe = mysql_real_escape_string($text);

	$sql = "SELECT * FROM seen WHERE seen_name = '".$name_safe."'";
	// print ("[DEBUG] sending [$sql]\nnick_ident is [$nick_ident]\n");

	$db_query = mysql_query($sql);
	if (mysql_errno()) { $bot->privMsg($dest, "lastSeen_msg (): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_query > 0) {
		if (mysql_num_rows ($db_query) > 0) {
			// print ("[DEBUG] updating entry in lastSeen\n");
			// update the information if already there 
			$seen_object = mysql_fetch_object($db_query);
			$seen_id = $seen_object->seen_id;
			$seen_nick = $seen_object->seen_name;
			$seen_date = $seen_object->seen_date;
			$seen_ident = $seen_object->seen_ident;
			$seen_saying = $seen_object->seen_saying;
			// now that we have old info, lets update
			$sql = "UPDATE seen SET seen_date=NOW(), seen_saying='".$text_safe."' WHERE seen_id='".$seen_id."' ";
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "lastSeen_msg (): ERROR: ".mysql_errno().":".mysql_error()); }
			}
		else
			{
			// print ("[DEBUG] new entry in lastSeen\n");
			// new entry
			$sql = "INSERT INTO seen (seen_name, seen_date, seen_ident, seen_saying) VALUES ('".$name_safe."', NOW(), '".$nick_ident."','".$text_safe."' )";
			$db_query = mysql_query($sql);
			if (mysql_errno()) { $bot->privMsg($dest, "lastSeen_msg (): ERROR: ".mysql_errno().":".mysql_error()); }
			// $karma_id = mysql_insert_id();
			}
	}
}
?>
