<?php
function eliza_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 6))) == "!eliza") {
	if (strlen(trim($text)) < 7) 
		{
		$bot->privMsg($dest, "Eliza syntax : !eliza <text for eliza to chew on>");
		}
	else
		{
		$eliza_text = trim(substr($text, 8));

		$d = new elizachat;
		$eliza_reply = $d -> say($eliza_text);

		$bot->privMsg($dest, $eliza_reply);
		}
	}
}

function eliza_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 6))) == "!eliza") {
        if (strlen(trim($text)) < 7)
                {
                $bot->privMsg($nick, "Eliza syntax : !eliza <text for eliza to chew on>");
                }
        else
                {
                $eliza_text = trim(substr($text, 8));

                $d = new elizachat;
                $eliza_reply = $d -> say($eliza_text);

                $bot->privMsg($nick, $eliza_reply);
                }
        }
}
?>
