<?
include ("xml_to_array.class.php");


		$wiki_term = "test";

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
                $wiki_desc = $arrayData[rss][channel][0][item][0][description];

		$wiki_desc = substr($wiki_desc, 0, 500);
                $wiki_out = $wiki_term . " - " . $wiki_desc;

		print $wiki_desc;
?>
