<?
function wiki_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 5))) == "!wiki")
        {
        if (strlen(trim($text)) < 6)
                {
                $bot->privMsg($dest, "WIKI syntax : !wiki <something>");
                }
        else
                {
		$wiki_term = urlencode(trim(substr($text, 6)));

		// blinkbits.com mirror of wikipedia.org
                $handle = @fopen("http://www.blinkbits.com/en_wikifeeds_rss/".$wiki_term, "r");

                $result = "";
                do {
                        $data = fread($handle, 8192);
                        if (strlen($data) == 0) {
                                break;
                        }
                        $result .= $data;
                } while (true);
                fclose($handle);

		//Creating Instance of the Class
		$xmlObj    = new XmlToArray($result);
		//Creating Array
		$arrayData = $xmlObj->createArray();

		print_r($arrayData);
		// this is for blinkbits.com way
		$wiki_desc = $arrayData[rss][channel][0][item][0][description];

		$wiki_desc_copywrong = "The Wikipedia article is licensed";
		$wiki_desc_check = strpos($wiki_desc, $wiki_desc_copywrong);

		if ($wiki_desc_check <> FALSE) {
			// $wiki_desc = substr($wiki_desc, 0, $wiki_desc_check);
			$bot->privMsg($dest, $wiki_term ." not found on wikipedia.");
			return;
			}

		$wiki_desc_short = strip_tags(substr($wiki_desc, 0, 400));
                $wiki_desc_short = preg_replace('/\[(.*?)\]/ie', "", $wiki_desc_short);

                $wiki_out = $wiki_term . " - " . $wiki_desc_short . "...";

		$bot->privMsg($dest, $wiki_out);
		}
	}
}
?>
