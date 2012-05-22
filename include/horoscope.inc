<?
function horoscope_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 10))) == "!horoscope") {
	if (strlen(trim($text)) < 11) 
		{
		$bot->privMsg($dest, "Horoscope syntax : !horoscope <zodiac sign>");
		}
	else
		{
		$zodiac = trim(substr($text, 11));

	        $d = new horoscope;
	        $horoscope_arr = $d -> get_all();

	        // print_r ($horoscope_arr);
	        $spam_text = "Powered by Astrocenter.com - Yahoo!";
	        $spam_text_len = strlen($spam_text);

	        for ($i=0; $i<count($horoscope_arr); $i++)
	                {
	                if (strcasecmp ($horoscope_arr[$i][ZodiacSign],$zodiac) == 0)
	                        {
	                        $horoscope_text = trim(substr($horoscope_arr[$i][DailyForecast], $spam_text_len));
	                        $bot->privMsg($nick, $horoscope_text);
	                        }
	                }
		}
	}
}

function horoscope_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 10))) == "!horoscope") {
        if (strlen(trim($text)) < 11)
                {
                $bot->privMsg($nick, "Horoscope syntax : !horoscope <zodiac sign>");
                }
        else
                {
                $zodiac = trim(substr($text, 11));

                $d = new horoscope;
                $horoscope_arr = $d -> get_all();

                // print_r ($horoscope_arr);
                $spam_text = "Powered by Astrocenter.com - Yahoo!";
                $spam_text_len = strlen($spam_text);

                for ($i=0; $i<count($horoscope_arr); $i++)
                        {
                        if (strcasecmp ($horoscope_arr[$i][ZodiacSign],$zodiac) == 0)
                                {
                                $horoscope_text = trim(substr($horoscope_arr[$i][DailyForecast], $spam_text_len));
                                $bot->privMsg($nick, $horoscope_text);
                                }
                        }
                }
        }
}
?>
