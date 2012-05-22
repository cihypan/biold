<?
function dict_lookup_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 5))) == "!dict") {
		if (strlen(trim($text)) < 6) {
			$bot->privMsg($nick, "Dictionary lookup syntax : !dict <term>");
			}
		else    
			{
			$term = trim(substr($text, 6));

			$dict_class = new Dict;
			$result_array = $dict_class->lookup($term);
			// print ("[DEBUG dict_lookup_fn ] : contents of result array \n");
			// print ("---------------------------------------------------\n");
			// print_r ($result_array);
			// print ("---------------------------------------------------\n");
			
			// print_r ($result);

			// print ("DEBUG dict_lookup_fn : elements in result_array : ".count($result_array)."\n");
			// print ("DEBUG dict_lookup_fn : elements in result_array[DefineResult] : ".count($result_array[DefineResult])."\n");
			// print ("DEBUG dict_lookup_fn : elements in result_array[DefineResult][Definitions] : ".count($result_array[DefineResult][Definitions])."\n");
			// print ("DEBUG dict_lookup_fn : elements in result_array[DefineResult][Definitions][Definition] : ".count($result_array[DefineResult][Definitions][Definition])."\n");

			$number_of_results = count ($result_array[DefineResult][Definitions][Definition]);
			$number_of_definitions = count ($result_array[DefineResult][Definitions], COUNT_RECURSIVE);
		
			// for ($x=0; $x < $number_of_results; $x++) {
			//	$dictionary = $result_array[DefineResult][Definitions][Definition][$x][Dictionary][Name];
			//	$word = $result_array[DefineResult][Definitions][Definition][$x][Word];
			//	$definition = $result_array[DefineResult][Definitions][Definition][$x][WordDefinition];
			// 
			//	print "[$dictionary : $word : $definition ]\n";
			//	}
		
			if ($number_of_results == 0 || $number_of_definitions == 1) {
				$bot->privMsg($dest, "Sorry, no definition for $term found...");
				return;
				}
		
			for ($x=0; $x <= $number_of_results; $x++)
				{
				$dictionary = $result_array[DefineResult][Definitions][Definition][$x][Dictionary][Name];
				$dictionary_list = $dictionary . ", " . $dictionary_list;
				}
			// $bot->privMsg($dest, "[Dictionaries found] ".$dictionary_list);

			$x = 1;
			$dictionary = $result_array[DefineResult][Definitions][Definition][$x][Dictionary][Name];
			$word = $result_array[DefineResult][Definitions][Definition][$x][Word];
			$definition = $result_array[DefineResult][Definitions][Definition][$x][WordDefinition];
		
			// print "number of results ".$number_of_results;
			// print " | defaulting to ".$dictionary."\n";
			// print $word.": [".$definition."]";
			// $bot->privMsg($dest, "Number of definitions found : ".$number_of_results." | Defaulting to ".$dictionary);
		
			$definition = preg_replace('/\n/', ' ', $definition);
			$definition = preg_replace('/\s\s+/', ' ', $definition);
			$definition_wordwrap = wordwrap($definition, 500);
		
			$definition_answer_array = explode ("\n", $definition_wordwrap);
			$definition_answer_array[0] = "[ $word : $dictionary ] - ".$definition_answer_array[0]."...";
			
 			$bot->privMsg($dest, $definition_answer_array[0]);

			// for ($i=0;$i<=count($definition_answer_array);$i++) {
			// $bot->privMsg($dest, $definition_answer_array[$i]);
			// 	print $definition_answer_array[$i];
			// 	}
			}
		}
	}

function thesaurus_lookup_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 5))) == "!thes") {
		if (strlen(trim($text)) < 6) {
			$bot->privMsg($nick, "Thesaurus lookup syntax : !thes <term>");
			}
		else    
			{
			$term = trim(substr($text, 6));

			$dict_class = new Dict;
			$result_array = $dict_class->lookup($term);

			$number_of_results = sizeof ($result_array[DefineResult][Definitions][Definition]);


			for ($x=0; $x < $number_of_results; $x++) {
				$thesaurus_check = stristr ($result_array[DefineResult][Definitions][Definition][$x][Dictionary][Name], 'Thesaurus');
				if ($thesaurus_check) {
					$thesaurus = $result_array[DefineResult][Definitions][Definition][$x][WordDefinition];
					$dictionary = $result_array[DefineResult][Definitions][Definition][$x][Dictionary][Name];
					$word = $result_array[DefineResult][Definitions][Definition][$x][Word];
					// print ("FOUND Thesaurus definitions : $thesaurus\n");
					}
				}
			$thesaurus = preg_replace ('/\n/', ' ', $thesaurus);
			$thesaurus = preg_replace('/\s\s+/', ' ', $thesaurus);
			$thesaurus_wordwrap = wordwrap ($thesaurus, 500);

			$thesaurus_array = explode ("\n", $thesaurus_wordwrap);
			$thesaurus_array[0] = "[ $word : $dictionary ] - ".$thesaurus_array[0]."...\n";
			$bot->privMsg($dest, $thesaurus_array[0]);
			}
		}
	}
?>
