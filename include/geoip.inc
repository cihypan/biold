<?php
function ip_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 3))) == "!ip")
        {
        if (strlen(trim($text)) < 4)
                {
                $bot->privMsg($dest, "IP LOOKUP syntax : !ip <host or ip>");
                }
        else
                {
		$ip_lookup = trim(substr($text, 4));

		$ip_lookup = gethostbyname($ip_lookup);

		// construct an instance (should fire off an http get)
		$geo = new GeoLocation($ip_lookup);
	
		// no php5 so id rather use an xml parser than write my own.. :)
		$xmlObj    = new SofeeXmlParser();
		// parse output
		$arrayData = $xmlObj->parseString($geo->xml_text);
		
		// put that junx into an array
		$arrayData_xml = $xmlObj->getTree();
		// print_r($arrayData_xml);
	
		$lookup_city = $arrayData_xml[HostipLookupResultSet]['gml:featureMember'][Hostip]['gml:name'][value];
		$lookup_country = $arrayData_xml[HostipLookupResultSet]['gml:featureMember'][Hostip][countryName][value];
	
		$bot->privMsg($dest, "$ip_lookup is in $lookup_city, $lookup_country");
		}
	}
}
?> 
