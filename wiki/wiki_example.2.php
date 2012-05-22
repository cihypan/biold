<?
include ("xml_parser.php");


                //Creating Instance of the Class
                $xmlObj    = new SofeeXmlParser();
                //Creating Array
		$term = "Nepotism";
                $arrayData = $xmlObj->parsewikiXML($term);

		$arrayData_xml = $xmlObj->getTree();
                // print_r($arrayData_xml);

		$wiki_desc = $arrayData_xml[mediawiki][page][revision][text][value];

		$wiki_desc_short = strip_tags(substr($wiki_desc, 0, 400));
		$wiki_desc_short = preg_replace('/\[(.*?)\]/ie', "", $wiki_desc_short);
		
		$wiki_out = $wiki_term . " - " . $wiki_desc_short . "...";

		echo $wiki_out;
?>
