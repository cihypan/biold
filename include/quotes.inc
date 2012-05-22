<?
function addquote_fn (&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 9))) == "!addquote") {
		if (strlen(trim($text)) < 9) {
			$bot->privMsg($nick, "Add quote to the database: !addquote <nick> <quote>");
			return;
			}
	
		$quote_txt = trim(substr($text, 10));

		$quote_nick_array = split (" ", $quote_txt);
		$quote_nick = $quote_nick_array[0];
		$quote_nick = str_replace ("<", "", $quote_nick);
		$quote_nick = str_replace (">", "", $quote_nick);

		$quote_nick_array[0] = "";
		$quote_text = implode(" ", $quote_nick_array);

		print ("DEBUG : [ $quote_nick ] [ $quote_text ] \n");

		$quote_nick = mysql_escape_string($quote_nick);
		$quote_text = mysql_escape_string($quote_text);
		$db_query = "INSERT INTO quotes (quote_nick, quote_text) VALUES ('".$quote_nick."','".$quote_text."') ";
		$db_send_query = @mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

		if ($db_send_query > 0) {
			$bot->privMsg($dest, "Quote added.");
			}
		else
			{
			$bot->privMsg($dest, "ERROR : Quote not added... something failed");
			}

		}
	}

function randomquote_fn (&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 3))) == "!rq") {

		$quote_key = mysql_escape_string(trim(substr($text, 4)));
		if (strlen($quote_key) > 0) {
			$where_text = "WHERE quote_text LIKE '%".$quote_key."%' ";
			}
		else
			{
			$where_text = "";
			}

		$db_query = "SELECT quote_id FROM quotes ".$where_text." ORDER BY RAND() LIMIT 1";
		$db_send_query = @mysql_query ($db_query);
		// if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
		if (mysql_errno()) { $bot->privMsg($dest,"Nothing found matching $quote_key"); }

		if ($db_send_query > 0) {
			$quote_id_row = mysql_fetch_object ($db_send_query);
			$quote_id = $quote_id_row->quote_id;
			
			$db_query = "SELECT * FROM quotes WHERE quote_id=".$quote_id;
			$db_send_query = mysql_query($db_query);
			// if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
			if (mysql_errno()) { $bot->privMsg($dest,"Nothing found matching $quote_key"); }
			$quote_row = mysql_fetch_object ($db_send_query);
			$quote_nick = $quote_row->quote_nick;
			$quote_text = trim($quote_row->quote_text);
			
			$bot->privMsg($dest, "<".$quote_nick."> ".$quote_text);

			}
	
		}
	}

function topquotes_fn (&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 10))) == "!topquotes") {
	
		$db_query = "SELECT quote_nick, COUNT(quote_nick) AS nick_count FROM quotes GROUP BY quote_nick ORDER BY nick_count DESC LIMIT 0,10";

		$db_send_query = mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

		if ($db_send_query > 0) {
			$num_rows = @mysql_num_rows($db_send_query);
			if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
			if ($num_rows > 0) {
				$top_nicks = "[TOP 10 QUOTES BY NICK] :";
				while ($row = mysql_fetch_object($db_send_query)) {
					$nick=$row->quote_nick;
					$nick_count=$row->nick_count;
					
					$top_nicks=$top_nicks." ".$nick." [".$nick_count."] |";
					}

				mysql_free_result($db_send_query);
				$bot->privMsg($dest, $top_nicks);
				}
			}
		}
	}
?>
