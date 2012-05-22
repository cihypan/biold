#!/usr/local/bin/php -q
<?
$grokterm="urban";
$grokarg="bling bling";


# can't use [:punct:] since I want some punct to go through.
# remove various naughty punctuations
include ("../sanitize.inc.php");
$grokarg=sanitize_system_string($grokarg);
$pattern="/<br.*>/i"; 

$grok_pipe=popen("/disk3/bioh/dbn/grok/grok $grokterm.grok $grokarg", "r");
while ($s = fgets($grok_pipe,1024)) {
   	$grok_out.=$s;
	}
$grok_out=preg_replace($pattern," ",$grok_out);
echo html_entity_decode($grok_out);
?>
