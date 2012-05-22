<?
function alice_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 6))) == "!alice") {
        if (strlen(trim($text)) < 7) {
		$bot->privMsg($dest, "Alice syntax : !alice <text for alice to chew on>");
		}
	else
		{
		$tell_alice_text = trim(substr($text, 7));

		$botresponse=replybotname($tell_alice_text,$nick,"TestBot");

		$botresponse->response=preg_replace('/([\s]){2,}/', ' ', $botresponse->response);

		$bot->privMsg($dest, "[alice] ".$botresponse->response);
		include ("/home/bioh/bot/subtility/db.php");
		}
	}
}
?>
