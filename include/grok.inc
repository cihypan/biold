<?
function grok_urban_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 6))) == "!urban")
        {
        if (strlen(trim($text)) < 7)
                {
                $bot->privMsg($dest, "URBAN syntax : !urban <something>");
                }
        else
                {
		$grokarg = sanitize_system_string(trim(substr($text, 7)));

		$grok_pipe=popen("/home/biold/biold/grok/grok /home/biold/biold/grok/urban.grok $grokarg", "r");
		while ($s = fgets($grok_pipe,1024)) {
		   	$grok_out.=$s;
			}
		$grok_out=html_entity_decode($grok_out);

		$pattern="/<br.*>/i";
		$grok_out=preg_replace($pattern," ",$grok_out);
		$grok_out=strip_tags($grok_out);

		$bot->privMsg($dest, $grok_out);
		}
	}
}

function grok_horoscope_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if ((strcasecmp($nick, "teebag") == 0)) {
		return;
		}
	if (trim(strToLower(substr($text, 0, 10))) == "!horoscope") {
		if (strlen(trim($text)) < 11) {
			$bot->privMsg($dest, "HOROSCOPE syntax : !horoscope <zodiac>");
		} else {
			$grokarg = sanitize_system_string(trim(substr($text, 10)));
	
			$grok_pipe=popen("/home/biold/biold/grok/grok /home/biold/biold/grok/scope.grok $grokarg", "r");
			while ($s = fgets($grok_pipe,1024)) {
				$grok_out.=$s;
				}

		$grok_out=html_entity_decode($grok_out);
	
		$bot->privMsg($nick, $grok_out);
			}
		}
	}
?>
