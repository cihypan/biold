<?
// originally written by julian_bond@voidstar.com
// adapted to subtility by bioh

function gnews_url_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if ((strcasecmp($nick, "teebag") == 0)) {
		return;
		}
	if (trim(strToLower(substr($text, 0, 7))) == "!ugnews") {
		if (strlen(trim($text)) < 8) {
			$bot->privMsg($dest, "UGNEWS syntax : !ugnews <topic>");
			}
		else
			{
			$q=trim(substr($text, 8));
			$num=4;

			if (isset ($rss_data)) 
				{unset($rss_data);}
			if (isset ($rss)) 
				{unset($rss);}

			$rss_data = parse_html($q,$num);
			$rss = new RSS ($rss_data);
			$allItems = $rss->getAllItems();
			$itemCount = count($allItems);

			for($y=0;$y<$itemCount;$y++)
				{
				// print "\nItem [$y] has data\n";
				$bot->privMsg($nick, "[$y]: " . html_entity_decode($allItems[$y]['TITLE']));
				$bot->privMsg($nick, "[$y]: " . $allItems[$y]['LINK']);
				// print "\n[$y]: Link : " . $allItems[$y]['LINK'];
				// print "\n[$y]: Desc : " . $allItems[$y]['DESCRIPTION'];
				}
			return;
			}
		}
	}



function gnews_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if ((strcasecmp($nick, "teebag") == 0)) {
		return;
		}
	if (trim(strToLower(substr($text, 0, 6))) == "!gnews") {
		if (strlen(trim($text)) < 7) {
			$bot->privMsg($dest, "GNEWS syntax : !gnews <topic>");
			}
		else
			{
			$q=trim(substr($text, 7));
			$num=4;

			if (isset ($rss_data)) 
				{unset($rss_data);}
			if (isset ($rss)) 
				{unset($rss);}

			$rss_data = parse_html($q,$num);
			$rss = new RSS ($rss_data);
			$allItems = $rss->getAllItems();
			$itemCount = count($allItems);

			for($y=0;$y<$itemCount;$y++)
				{
				// print "\nItem [$y] has data\n";
				$bot->privMsg($nick, "[$y]: " . html_entity_decode($allItems[$y]['TITLE']));
				// print "\n[$y]: Link : " . $allItems[$y]['LINK'];
				// print "\n[$y]: Desc : " . $allItems[$y]['DESCRIPTION'];
				}
			return;
			}
		}
	}


//****************
function parse_html($q,$num){

  header("Cache-Control: public");  

  $itemregexp = "%<td valign=top><a href=\"(.+?)\".+?>(.+?)</a><br><font size=-1><font color=#6f6f6f>(.+?)</font><br></table>%is";
  $allowable_tags = "<A><B><BR><BLOCKQUOTE><CENTER><DD><DL><DT><HR><I><IMG><LI><OL><P><PRE><U><UL>";

  $num = ($num) ? $num+1 : 16 ;

  $url = "http://news.google.com/news?hl=en&num=$num&scoring=d&q=".urlencode($q);

  if ($fp = @fopen($url, "r")) {
    while (!feof($fp)) $data .= fgets($fp, 128);
    fclose($fp);
  }

// *******************
// Some people seem to have problems with google not returning anything
// uncomment the following lines and comment out the content-type header
// to see what google is returning.
  
//  print "<html>";
//  print "<pre>";
//  print htmlentities($data);  
  header("Content-Type: text/xml");

  $data = strstr($data,"Sorted by date</b>");

  eregi("<title>(.*)</title>", $data, $title);
  $channel_title = $title[1];

  $match_count = preg_match_all($itemregexp, $data, $items);
  $match_count = ($match_count > 25) ? 25 : $match_count;
  
  $output .= "<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>\n";
  $output .= "<!-- generator=\"gnews2rss/1.0\" -->\n";
  $output .= "<!DOCTYPE rss >\n";

  $output .= "<rss version=\"2.0\">\n";
  $output .= "  <channel>\n";
  $output .= "    <title>Google News Search: $q</title>\n";
  $output .= "    <link>". htmlentities($url) ."</link>\n";
  $output .= "    <description>Google News Search: $q</description>\n";
  $output .= "    <webMaster>julian_bond@voidstar.com</webMaster>\n";
  $output .= "    <language>en-us</language>\n";
  $output .= "    <generator>&lt;a href=\"http://www.voidstar.com/gnews2rss.php\">GNews2Rss&lt;/a></generator>\n";

  $day = date("d"); 
  if (false) {
//  if ($day == 1 || $day == 11 || $day == 21) {
    $output .= "    <item>\n";
    $output .= "      <title>". date("d-M-y"). " Do you find Gnews2RSS useful?</title>\n";
    $output .= "      <link>http://www.voidstar.com/gnews2rss.php</link>\n";
    $output .= "      <description>If you're using Gnews2rss you presumably find it useful. Please &lt;a href=\"mailto:news-feedback@google.com\">email Google&lt;/a> asking them to produce RSS directly out of Google News Search. And why not &lt;a href=\"http://www.voidstar.com/gnews2rss.php.txt\">host it yourself&lt;/a> to save my bandwidth costs.</description>\n";
    $output .= "    </item>\n";
  }

  for ($i=0; $i< $match_count; $i++) {

    $item_url = $items[1][$i];
    $title = $items[2][$i];
    $title = strip_tags($title);
    $desc = $items[3][$i];

    $desc = eregi_replace("&nbsp;-&nbsp;.* ago</font><br>", "<br>", $desc);
    $desc = strip_tags($desc, $allowable_tags);
    $desc = htmlspecialchars($desc);

    $output .= "    <item>\n";
    $output .= "      <title>". htmlspecialchars($title) ."</title>\n";
    $output .= "      <link>". htmlspecialchars($item_url) ."</link>\n";
    $output .= "      <description>". $desc ."</description>\n";
    $output .= "    </item>\n";
  }

  $output .= "  </channel>\n";
  $output .= "</rss>\n";

  return $output;
}
?>
