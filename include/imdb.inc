<?
//function html_entity_decode_utf8($string) {
//	static $trans_tbl;
 //      
//	// replace numeric entities
//	$string = preg_replace('~&#x([0-9a-f]+);~ei', 'code2utf(hexdec("\\1"))', $string);
//	$string = preg_replace('~&#([0-9]+);~e', 'code2utf(\\1)', $string);
//	
//	// replace literal entities
//	if (!isset($trans_tbl)) {
//		$trans_tbl = array();
//	
//		foreach (get_html_translation_table(HTML_ENTITIES) as $val=>$key)
//			$trans_tbl[$key] = utf8_encode($val);
//		}
//	
//	return strtr($string, $trans_tbl);
//	}
//	
// Returns the utf string corresponding to the unicode value (from php.net, courtesy - romans@void.lv)
function code2utf($num) {
	if ($num < 128) return chr($num);
	if ($num < 2048) return chr(($num >> 6) + 192) . chr(($num & 63) + 128);
	if ($num < 65536) return chr(($num >> 12) + 224) . chr((($num >> 6) & 63) + 128) . chr(($num & 63) + 128);
	if ($num < 2097152) return chr(($num >> 18) + 240) . chr((($num >> 12) & 63) + 128) . chr((($num >> 6) & 63) + 128) . chr(($num & 63) + 128);
	return '';
	}

function imdb_lookup($imdb_id) {

	$imdb=new imdb($imdb_id);	

	$movie_title = "Title: ".$imdb->get('Title');
	$movie_plot = "Plot: ".$imdb->get('Plot');
	$movie_year = "Year: ".$imdb->get('Year');
	$movie_runtime = "Runtime: ".$imdb->get('Runtime');
	$movie_stars = "Rating: ".$imdb->get('imdbRating');
	$movie_votes = "Votes: ".$imdb->get('imdbVotes');

	$imdb_lookup_txt = $movie_title . " | " . $movie_plot . " | " . $movie_year  . " | " . $movie_runtime  . " | " . $movie_stars  . " | " . $movie_votes;

	return $imdb_lookup_txt;
	}

function imdb_lookup_id($imdb_id) {

	$imdb=new imdb();	
	$imdb->set('id',$imdb_id);

	$movie_title = "Title: ".$imdb->get('Title');
	$movie_plot = "Plot: ".$imdb->get('Plot');
	$movie_year = "Year: ".$imdb->get('Year');
	$movie_runtime = "Runtime: ".$imdb->get('Runtime');
	$movie_stars = "Rating: ".$imdb->get('imdbRating');
	$movie_votes = "Votes: ".$imdb->get('imdbVotes');

	$imdb_lookup_txt = $movie_title . " | " . $movie_plot . " | " . $movie_year  . " | " . $movie_runtime  . " | " . $movie_stars  . " | " . $movie_votes;

	return $imdb_lookup_txt;
	}



function imdb_auto_lookup_fn(&$bot, $dest, $imdb_url) {

	// preg_match_all("/tt\\d{7}/", $string, $ids);

	preg_match_all("/tt\\d{7}/", $imdb_url, $imdb_id_arr);
	print ("[DEBUG] - imdb_auto_lookup_fn - ".$imdb_url." - ".$imdb_id_arr[0][0]."\n");

	$imdb_lookup_txt=imdb_lookup_id($imdb_id_arr[0][0]);

	$bot->privMsg($dest, $imdb_lookup_txt);
	}

function imdb_lookup_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 5))) == "!imdb") {
		if (strlen(trim($text)) < 6) {
			$bot->privMsg($nick, "Internet Movie Database Lookup syntax : !imdb <movie>");
			}
		else    
			{
			$imdb_movie = trim(substr($text, 6));

			$imdb_text = imdb_lookup($imdb_movie);

			$bot->privMsg($dest, $imdb_text);
			}
		}
	}
