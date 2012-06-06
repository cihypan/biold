<?php
function google_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 7))) == "!google") {
	if (strlen(trim($text)) < 8) 
		{
		$bot->privMsg($dest, "Google syntax : !google <somethin to google>");
		}
	else
		{
		$google_text = trim(substr($text, 8));
		$googlie = new Google;
		$google_out=$googlie->google_lookup($google_text);
		$bot->privMsg($nick, $google_out);
		}
	}
}

function google_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 7))) == "!google") {
        if (strlen(trim($text)) < 8)
                {
                $bot->privMsg($nick, "Google syntax : !google <somethin to google>");
                }
        else
                {
                $google_text = trim(substr($text, 8));
                $googlie = new Google;
                $google_out=$googlie->google_lookup($google_text);
                $bot->privMsg($nick, $google_out);
                }
        }
}

function google_calc_fn (&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 5))) == "!calc") {
		$calc_text = urlencode(trim(substr($text, 6)));
		$url = 'http://www.google.ca/search?source=ig&hl=en&q='.$calc_text.'&btnG=Google+Search&meta=';

		// NEED user agent now :(
		// $fp = fopen($url, "r");
		$fp = popen("wget -q -O - --user-agent='Gecko/2008052906 Firefox/3.0' '$url'","r");
		$result = '';

		while(!feof($fp)) {
			$result .= fread($fp, 1024);
		}

		//$calc_google_pattern ='/<div id=res class=med role=main>(.*)<h2 class=r style=\"font-size:138\%\"><b>(.*)<\/b><\/h2><div style/';
		$calc_google_pattern = '/<h2 class=r style="(.*)"><b>(.*)<\/b><\/h2>/is';
			if (@preg_match ($calc_google_pattern, $result, $calc_array))
			{
				$google_result = strip_tags ($calc_array[2]);
				$bot->privMsg($dest, $google_result);
			}
		}
}
?>
