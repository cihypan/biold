<?php
function htmlspecialchars_decode_php4($string, $quote_style = ENT_COMPAT) {
	return strtr($string, array_flip(get_html_translation_table(HTML_SPECIALCHARS, $quote_style)));
	}

function remove_unnecessary ($str) {
	// $str = str_replace(array("\n", "\r", "\t", "\o", "\x0B"), "", $str);
	$str = preg_replace ('/\s*/m', ' ', $str);
	return $str;
}

function array_trim($arr) {
	return array_map('trim', $arr);
}

function do_not_old_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 9))) == "!donotold") {
		$url = trim(substr($text, 10));
		$url_safe = mysql_real_escape_string($url);
	
		$db_query = "INSERT INTO do_not_old (url, url_date) VALUES ('".$url_safe."', now()) ";
		$db_send_query = @mysql_query ($db_query);
	
		if ($db_send_query > 0) {
			$bot->privMsg ($dest, "Added $url to 'do not old' list.");
			}
		else    
			{
			$bot->privMsg ($dest, "Failed to add $url :(");
			}
		return;
		}
	}


function old_release_fn(&$bot, $nick, $ident, $host, $dest, $text) {

        if ((strcasecmp($nick, "teebag") == 0)) 
                {
                return;
                }


	# some new additions to the releases
	# aPC KSi EGO XXL aAF BUTT WHOA FNT TLT WCR ESC SSR PH IND WC GTI
	# RTB ESC FM BUTT WHOA uF RNS EGO FNT KzT XXL

	# from stereo : 
	# [^\s]+[\-\.](2006|XviD|DVDR|XBOX|XBOX360)\-[^\s]+
	#

	$release='/[^\s]+[\-\.](2006|2007|2008|2009|2010|2011|2012|[Xx][Vv][Ii][Dd]|XBOX|XBOX360|[hx]264|[Ss][Cc][Rr][Ee][Ee][Nn][Ee][Rr]|DVD[Rr])\-[^\s]+/';

// 	$release='/[a-zA-Z0-9\-\._\/\\\]+((DVD[Rr])+|'
// 		.'([Ss][Cc][Rr][Ee][Ee][Nn][Ee][Rr])+|'
// 		.'([Rr][Nn][Ss])|'
// 		.'(aPC)|'
// 		.'(RTB)|'
// 		.'(ESC)|'
// 		.'(FM)|'
// 		.'(BUTT)|'
// 		.'(uF)|'
// 		.'(EGO)|'
// 		.'(FNT)|'
// 		.'(KzT)|'
// 		.'(XXL)|'
// 		.'(XViD)|'
// 		.'(XBOX)|'
// 		.'(XXL)|'
// 		.'(aAF)|'
// 		.'(RNS)|'
// 		.'(BUTT)|'
// 		.'(WHOA)|'
// 		.'(FNT)|'
// 		.'(TLT)|'
// 		.'(WCR)|'
// 		.'(ESC)|'
// 		.'(FM)|'
// 		.'(RELOADED)|'
// 		.'(IND)|'
// 		.'(WC)|'
// 		.'(GTI)|'
// 		.'([Vv][Cc][Dd])+|([Ss][Vv][Cc][Dd])+)/';

	 if (preg_match($release, $text, $match)) {
		include ("db.php");
		// print_r ($match);
		if (strlen($match[0]) > 10) {
		$release = mysql_real_escape_string($match[0]);
		$db_query = "SELECT *,(unix_timestamp(now())-unix_timestamp(timestamp)) as secsbetween FROM release_list WHERE release_list.release='".$release."'";
		$db_send_query = @mysql_query ($db_query);
		// if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
		if (mysql_errno()) { print  "ERROR: ".mysql_errno().":".mysql_error(); }
		if ($db_send_query > 0) {
			$num_rows = @mysql_num_rows($db_send_query);
			if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
			if ($num_rows > 0) {
				$row = mysql_fetch_object($db_send_query);
				$old_nick=$row->nick;
				$old_stamp=$row->timestamp;
				$old_secsbetween=$row->secsbetween;

				mysql_free_result($db_send_query);

				if ((strcasecmp ($old_nick, $nick) != 0) && (strcasecmp($old_nick, "teebag") != 0)) {
					$secsbetween=time_to_string($old_secsbetween);
	                                $bot->privMsg($dest, "OLD ! OLD ! OLD ! $release posted already by $old_nick on $old_stamp [$secsbetween ago]");
	                                $db_query = "INSERT INTO olded (nick, datestmp) VALUES ('".$nick."', now())";
	                                $db_send_query = @mysql_query ($db_query);
	                                if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                                	}
                        	}
                	else
                        	{
	                        // weeeee, its a new url...
				// $bot->privMsg($dest, "Adding URL [$url]");
				// .ircConf::get("channel2")
				$bot->privMsg("#sinned-old","[$nick] - $release"); // sloppy.  sorry. -sinned     
				$db_query = "INSERT INTO release_list (release_list.release, release_list.timestamp, release_list.nick) VALUES ('".$release."',now(),'".$nick."')";
				$db_send_query = @mysql_query ($db_query);
				if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                        	}
			}
		}
		}
}

function search_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 7))) == "!search")
        {
	include ("db.php");
        if (strlen(trim($text)) < 8)
                {
                $bot->privMsg($nick, "Search URL syntax : !search <part of url>");
		return;
                }

	$search_txt = trim(substr($text, 8));
	$search_txt = addslashes($search_txt);

	$db_query = "SELECT * FROM url_list WHERE url LIKE '%".$search_txt."%' ORDER BY timestamp DESC LIMIT 0,9";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

	if ($db_send_query > 0) {
		$num_rows = @mysql_num_rows($db_send_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

		if ($num_rows < 9) 
			{
			$results_num=$num_rows;
			}
		else
			{
			$results_num=9;
			}

		$counter=1;
		$bot->privMsg($nick, "Last ".$results_num." of ".$num_rows." matching URLs :");
		while ($row = mysql_fetch_object($db_send_query))
			{
			$id=$row->id;
			$url=$row->url;
			$nick_row=$row->nick;
			$stamp=$row->timestamp;
			
			//$msg=$stamp. " " . $id." : [".$nick_row."] - ".$url;
			$msg=$stamp. " : [".$nick_row."] - ".$url;
			$bot->privMsg($nick, $msg);
			$counter++;
			if ($counter > $results_num)
				{
				mysql_free_result($db_send_query);
				return;
				}
			}

		mysql_free_result($db_send_query);
		}
	}
}

function nlast_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 6))) == "!nlast")
        {
	$nick_to_last = trim(substr($text, 7));
	$nick_to_last = addslashes($nick_to_last);

	if (strlen($nick_to_last) == 0)
		{
		$bot->privMsg($nick, "Syntax : !nlast <nick>");
		}

	$db_query = "SELECT * FROM url_list WHERE nick='".$nick_to_last."' ORDER BY timestamp DESC LIMIT 0,5";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

	if ($db_send_query > 0) {
		$num_rows = @mysql_num_rows($db_send_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

		if ($num_rows == 0)
			{
			$bot->privMsg($nick, "No matching URLs found for ".$nick_to_last);
			mysql_free_result($db_send_query);
			return;
			}

		if ($num_rows > 5) 
			{
			$num_results = 5;
			}
		else
			{
			$num_results = $num_rows;
			}

		$bot->privMsg($nick, "Last ".$num_results." URLs from ".$nick_to_last." :");
		while ($row = mysql_fetch_object($db_send_query))
			{
			$id=$row->id;
			$url=$row->url;
			$nick_row=$row->nick;
			$stamp=$row->timestamp;
			
			$msg=$id." : [".$nick_row."] - ".$url;
			$bot->privMsg($nick, $msg);
			}

		mysql_free_result($db_send_query);
		}
	}
}

function last_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 5))) == "!last")
        {
	$last_num = trim(substr($text, 6));

	if (is_numeric($last_num))
		{
	// limit the max to 9 if its greater than 9?
	//	if ($last_num > 9) 
	//		{
	//		$last_num = 9;
	//		}
		}
	else
		{
		$last_num = 9;
		}

	$db_query = "SELECT * FROM url_list ORDER BY timestamp DESC LIMIT 0,".$last_num;
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

	if ($db_send_query > 0) {
		$num_rows = @mysql_num_rows($db_send_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

		$bot->privMsg($nick, "Last ".$last_num." URLs :");
		$url_count = 0;
		while ($row = mysql_fetch_object($db_send_query))
			{
			$id=$row->id;
			$url=$row->url;
			$nick_row=$row->nick;
			$stamp=$row->timestamp;
			
			$msg=$stamp." : [".$nick_row."] - ".$url;
			$bot->privMsg($nick, $msg);
			$url_count++;
			if ($url_count > 9)
				{ 
				// rate limit a bit if its higher number of urls
				sleep (1); 
				}
			}

		mysql_free_result($db_send_query);
		}
	}
}

function old_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  // bio additions... 
  if (trim(strToLower(substr($text, 0, 4))) == "!old") {
	return;
	}
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
  $urlpattern = '/((http|https):\/\/|www\.)' 
     .'[a-z0-9\-\._]+\/?[a-z0-9_\.\-\?\+\/~=&#;:,%@\{\}]*' 
     .'[a-z0-9\/]{1}/si'; 

  if (@preg_match ($urlpattern, $text, $urlmatch))
	{
	include ("db.php");
	//$bot->privMsg($dest, "Got URL [$urlmatch[0] [$nick > $text]]");
	$url = addslashes($urlmatch[0]);
	$url_encoded = urlencode($urlmatch[0]);
	// hack: my lame regexp skills amount to getting
	// the obviously wrong : www, wwww, wwwww, ... :)
	// so rather than actually fix my stupidity, i patch... /sigh
	if (strlen(trim(str_replace("w","",$url))) == 0) {
		return;}

	if (stristr($url, 'http')) { 
		$domain_url_extract_pattern = '/http:\/\/[^\/]*\//';
		if (@preg_match ($domain_url_extract_pattern, $url, $domain_match_array)) {
			$domain_txt = $domain_match_array[0] . " - ";
			// get rid of http:// ... /
			// used in do_not_old 
			$domain_txt_nohttp = str_replace ("http://", "", $domain_txt);
			$domain_txt_nohttp = str_replace ("/", "", $domain_txt_nohttp);
			}

		$title_pattern='/<title>(.*)<\/title>/is';

		// $fp = fopen($url, "rb");
		// plain fopen replaced by 'be' - browser emulator class 
	        $be = new BrowserEmulator();
	        $fp = $be->fopen($url);
		if ($fp) {
			// print ("[DEBUG] trying to get title from $url\n");
			for ($x = 1; $x < 60; $x++) {
				$result = '';
				$result = fread ($fp, 8192);
			
				if (@preg_match ($title_pattern, $result, $title_array_match)) {
					$title_txt = trim(html_entity_decode_utf8($title_array_match[1]));
					// print ("DEBUG : title_array_match\n");
					// print_r ($title_array_match);
					$title_txt = preg_replace ('/\s+/is', ' ', $title_txt);
					if (strlen($title_txt) > 300) {
						if (stristr ($title_txt, "</title>")) {
							// for some reason the above matches with </title> at end of string
							// and floods the bot off the irc :)
							// so this should cut it off
							$title_txt = substr ($title_txt, 0, stristr ($title_txt, "</title>"));
							}
						else
							{
							$title_txt = substr ($title_txt, 0, 300);
							}
						}

					$bot->privMsg($dest, $domain_txt . $title_txt);
					$bot->privMsg("#sinned-old",$title_txt); // sloppy.  sorry. -sinned
					// get out of the loop no need to read further
					$bytes_read = 8192 * $x;
					// print ("[DEBUG] read " . $bytes_read . " bytes before title tag\n");
					break;
					}
				}
			fclose ($fp);
			}
		else	
			{
			// do nothing...
			// $bot->privMsg($dest, "Failed to fetch open url :/");
			}
		}

	if (stristr($url, 'imdb.com')) {
		imdb_auto_lookup_fn(&$bot, $dest, $url);
		}

	$db_query = "SELECT *,(unix_timestamp(now())-unix_timestamp(timestamp)) as secsbetween FROM url_list WHERE url='".$url."'";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_send_query > 0)
		{
		$num_rows = @mysql_num_rows($db_send_query);
		if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
		if ($num_rows > 0)
			{
			while ($row = mysql_fetch_object($db_send_query)) 
				{
				$old_nick=$row->nick;
				$old_stamp=$row->timestamp;
				$old_secsbetween=$row->secsbetween;
				}
			mysql_free_result($db_send_query);
			if ((strcasecmp ($old_nick, $nick) != 0) && (strcasecmp($old_nick, "teebag") != 0))
				{
				$db_query = "SELECT * FROM do_not_old WHERE (url LIKE '%".$url."%') OR (url LIKE '%".$domain_txt_nohttp."%') OR (url = '".$domain_txt_nohttp."')";
				$db_send_query = @mysql_query ($db_query);
				$num_rows = @mysql_num_rows ($db_send_query);
				if ($num_rows > 0) {
					// found at least one url in the do_not_old list
					return;
					}
				$secsbetween=time_to_string($old_secsbetween);
				$bot->privMsg($dest, "OLD ! OLD ! OLD ! Original from $old_nick on $old_stamp [$secsbetween ago]");
				$db_query = "INSERT INTO olded (nick, datestmp) VALUES ('".$nick."', now())";
				$db_send_query = @mysql_query ($db_query);
				if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
				}
			}
		else
			{
			// weeeee, its a new url...
			// $bot->privMsg($dest, "Adding URL [$url]");
			
			// check for pdf/tiff/powerpoint
			if (strtolower(substr($url, -4)) == ".pdf" || strtolower(substr($url, -5)) == ".tiff") {
				$google_docs_reader_url = "http://docs.google.com/viewer?url=" . $url_encoded;
				}

			if (strlen($url) > 135 || isset($google_docs_reader_url)) {
				if (!empty($google_docs_reader_url)) {
					$fp = fopen("http://tinyurl.com/create.php?url=$google_docs_reader_url", "r");
					}
				else	{
					$fp = fopen("http://tinyurl.com/create.php?url=$url", "r");
					}

				$result = '';
				if ($fp) {
					while(!feof($fp)) {
				       		$result .= fread($fp, 1024);
						}
					}
				fclose($fp);

				$tiny_url_pattern = '/<blockquote><b>(.*)<\/b><br>/';

				if (@preg_match ($tiny_url_pattern, $result, $tiny_url_array)) {
					// print_r ($tiny_url_array);
					$bot->privMsg($dest, "[tinyurl] $tiny_url_array[1]");
					}
				}

			$bot->privMsg("#sinned-old","[$nick] - $url");   // sloppy.  sorry. -sinned
			$db_query = "INSERT INTO url_list (url, timestamp, nick) VALUES ('".$url."',now(),'".$nick."')";
			$db_send_query = @mysql_query ($db_query);
			if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
			}
		}
	}
}

function old_status_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 4))) == "!url")
	{
	global $start_time;

	// select nick,count(nick) as cn from url_list group by nick order by cn desc limit 0,10;
        $db_query = "SELECT nick,COUNT(nick) as cnick FROM url_list GROUP BY nick ORDER BY cnick DESC LIMIT 0,10";
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0)
		{
                $num_rows = @mysql_num_rows($db_send_query);
                if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                if ($num_rows > 0)
                        {
			$top_nicks = "[TOP 10 URL SUBMITTERS] :";
                        while ($row = mysql_fetch_object($db_send_query))
                                {
                                $nick=$row->nick;
                                $nick_count=$row->cnick;

				$top_nicks=$top_nicks." ".$nick." [".$nick_count."] |";
                                }
                        mysql_free_result($db_send_query);
			$bot->privMsg($dest, $top_nicks);
			}
		}

	// select nick,count(nick) as cnick from olded group by nick order by cnick desc limit 0,10;
        $db_query = "SELECT nick,COUNT(nick) as cnick FROM olded GROUP BY nick ORDER BY cnick DESC LIMIT 0,10";
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0)
		{
                $num_rows = @mysql_num_rows($db_send_query);
                if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                if ($num_rows > 0)
                        {
			$top_nicks = "[TOP 10 OLDED] :";
                        while ($row = mysql_fetch_object($db_send_query))
                                {
                                $nick=$row->nick;
                                $nick_count=$row->cnick;

				$top_nicks=$top_nicks." ".$nick." [".$nick_count."] |";
                                }
                        mysql_free_result($db_send_query);
			$bot->privMsg($dest, $top_nicks);
			}
		}

	$now_time = getmicrotime();
	$alive_time = abs($now_time - $start_time);
	$alive_time_string = time_to_string($alive_time);
	$bot->privMsg($dest, "I have been awake for $alive_time_string");
	}
}

function old_status_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 4))) == "!url")
        {
        // select nick,count(nick) as cn from url_list group by nick order by cn desc limit 0,10;
        $db_query = "SELECT nick,COUNT(nick) as cnick FROM url_list GROUP BY nick ORDER BY cnick DESC LIMIT 0,10";
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($nick, "ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0)
                {
                $num_rows = @mysql_num_rows($db_send_query);
                if (mysql_errno()) { $bot->privMsg($nick, "ERROR: ".mysql_errno().":".mysql_error()); }
                if ($num_rows > 0)
                        {
                        $top_nicks = "[TOP 10 URL SUBMITTERS] :";
                        while ($row = mysql_fetch_object($db_send_query))
                                {
                                $nick=$row->nick;
                                $nick_count=$row->cnick;

                                $top_nicks=$top_nicks." ".$nick." [".$nick_count."] |";
                                }
                        mysql_free_result($db_send_query);
                        $bot->privMsg($nick, $top_nicks);
                        }
                }

        // select nick,count(nick) as cnick from olded group by nick order by cnick desc limit 0,10;
        $db_query = "SELECT nick,COUNT(nick) as cnick FROM olded GROUP BY nick ORDER BY cnick DESC LIMIT 0,10";
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($nick, "ERROR: ".mysql_errno().":".mysql_error()); }
        if ($db_send_query > 0)
                {
                $num_rows = @mysql_num_rows($db_send_query);
                if (mysql_errno()) { $bot->privMsg($nick, "ERROR: ".mysql_errno().":".mysql_error()); }
                if ($num_rows > 0)
                        {
                        $top_nicks = "[TOP 10 OLDED] :";
                        while ($row = mysql_fetch_object($db_send_query))
                                {
                                $nick=$row->nick;
                                $nick_count=$row->cnick;

                                $top_nicks=$top_nicks." ".$nick." [".$nick_count."] |";
                                }
                        mysql_free_result($db_send_query);
                        $bot->privMsg($nick, $top_nicks);
                        }
                }
        }
}
?>
