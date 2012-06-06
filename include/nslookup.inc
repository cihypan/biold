<?php

/** Sample tool: "!nslookup 127.0.0.1" OR "!nslookup www.php.net" */
/** NOTE:
      Be careful with functions that may take a while to return,
      these functions will have to return before the bot will continue.
      
      If the bot were allowed to simutainiously process callbacks,
      too many procedures would cause the bot to use large amounts
      of resources.
*/

function toolNsLookup(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
  if (strToLower(substr($text, 0, 9)) == "!nslookup") {
    $lookupHost = str_replace(" ", "", substr($text, 10));
    if (@preg_match( // is IP :)
      "/^(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.".
      "(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.".
      "(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.".
      "(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])$/", $lookupHost)) {
      if (($returnHost = getHostByAddr($lookupHost)) !== $lookupHost) {
        $bot->privMsg($nick, "Resolved $lookupHost to $returnHost");
      } else {
        $bot->privMsg($nick, "Unable to resolve IP $lookupHost");
      }
    } else {
      if (($returnHost = getHostByName($lookupHost)) !== $lookupHost) {
        $bot->privMsg($nick, "Resolved $lookupHost to $returnHost");
      } else {
        $bot->privMsg($nick, "Unable to resolve $lookupHost");
      }
    }
  }
}

?>
