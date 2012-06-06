<?php
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
		$wiki_term = trim(substr($text, 6));
		$wiki_term = str_replace(" ", "_", $wiki_term);
		// print ($wiki_term);

                //Creating Instance of the Class
                $xmlObj    = new SofeeXmlParser();
                //Creating Array
                $arrayData = $xmlObj->parsewikiXML($wiki_term);

		$arrayData_xml = $xmlObj->getTree();
                // print_r($arrayData_xml);

		$wiki_desc = $arrayData_xml[mediawiki][page][revision][text][value];

		$wiki_tmp = substr($wiki_desc, 0, 600);
		// print ("[DEBUG] [$wiki_tmp] \n");
		// [DEBUG] ['''Nepotism''' means favouring relatives or personal friends because of their relationship rather than because of their abilities. For example, if a manager employed or promoted a relative rather than a more qualified non-relative, that manager would be guilty of nepotism. Some biologists have suggested that a tendency towards nepotism is instinctive, a form of [[kin selection]].The word ''nepotism'' comes from the Latin word 'nepos', meaning "nephew".  In the [[Middle Ages]], some [[Catholicism|Catholic]] [[pope]]s and [[bishop]]s, who had taken vows of [[chastity]], and therefore usually had]

		if (stristr($wiki_tmp, "#redirect")) {
			// [DEBUG] [#redirect [[Willy Wonka]]]
			// gotta get the redirect
			$wiki_tmp = preg_replace('/\[\[/', "", $wiki_tmp);
			$wiki_tmp = preg_replace('/\]\]/', "", $wiki_tmp);
			$wiki_tmp = str_replace("#redirect ", "", $wiki_tmp);
			$wiki_tmp = str_replace("#REDIRECT ", "", $wiki_tmp);
			$wiki_tmp = str_replace(" ", "_", $wiki_tmp);
			$bot->privMsg($dest, "Got redirect page to $wiki_tmp");

			$arrayData = $xmlObj->parsewikiXML($wiki_tmp);
			$arrayData_xml = $xmlObj->getTree();

			$wiki_desc = $arrayData_xml[mediawiki][page][revision][text][value];
			// 
			}

		$wiki_desc_short = strip_tags(substr($wiki_desc, 0, 600));
		$wiki_desc_short = preg_replace('/[[:space:]]+/', " ", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\[\[/', "", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\]\]/', "", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\{\{/', "_", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\}\}/', "_", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\'\'\'/', "_", $wiki_desc_short);
		$wiki_desc_short = preg_replace('/\((\w+)\)/', "(\$1)", $wiki_desc_short);

		$wiki_out = $wiki_term . " - " . $wiki_desc_short . "...";

		$bot->privMsg($dest, $wiki_out);
		}
	}
}
?>
