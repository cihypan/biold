<?php
function ctof_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 5))) == "!ctof") {
	if (strlen(trim($text)) < 6) 
		{
		$bot->privMsg($nick, "CtoF syntax : !ctof <temperature in C to convert to F>");
		}
	else
		{
		$temp_input = trim(substr($text, 6));
		if (!is_numeric($temp_input))
			{
			$bot->privMsg($nick, "CtoF syntax : !ctof <temperature in C to convert to F>");
			return;
			}

	        $d = new temperature;
	        $ctof_result = $d -> ctof($temp_input);

		$bot->privMsg($dest, $ctof_result);
		}
	}
}

function ftoc_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 5))) == "!ftoc") {
	if (strlen(trim($text)) < 6) 
		{
		$bot->privMsg($nick, "FtoC syntax : !ftoc <temperature in F to convert to C>");
		}
	else
		{
                $temp_input = trim(substr($text, 6));
                if (!is_numeric($temp_input))
                        {
			$bot->privMsg($nick, "FtoC syntax : !ftoc <temperature in F to convert to C>");
                        return;
                        }

                $d = new temperature;
                $ftoc_result = $d -> ftoc($temp_input);

                $bot->privMsg($dest, $ftoc_result);
		}
	}
}
?>
