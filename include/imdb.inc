<?php
function html_entity_decode_utf8($string) {
	static $trans_tbl;
       
	// replace numeric entities
	$string = preg_replace('~&#x([0-9a-f]+);~ei', 'code2utf(hexdec("\\1"))', $string);
	$string = preg_replace('~&#([0-9]+);~e', 'code2utf(\\1)', $string);
	
	// replace literal entities
	if (!isset($trans_tbl)) {
		$trans_tbl = array();
	
		foreach (get_html_translation_table(HTML_ENTITIES) as $val=>$key)
			$trans_tbl[$key] = utf8_encode($val);
		}
	
	return strtr($string, $trans_tbl);
	}
	
// Returns the utf string corresponding to the unicode value (from php.net, courtesy - romans@void.lv)
function code2utf($num) {
	if ($num < 128) return chr($num);
	if ($num < 2048) return chr(($num >> 6) + 192) . chr(($num & 63) + 128);
	if ($num < 65536) return chr(($num >> 12) + 224) . chr((($num >> 6) & 63) + 128) . chr(($num & 63) + 128);
	if ($num < 2097152) return chr(($num >> 18) + 240) . chr((($num >> 12) & 63) + 128) . chr((($num >> 6) & 63) + 128) . chr(($num & 63) + 128);
	return '';
	}

function imdb_search($imdb_movie) {
	//init the class
	$IMDB_Search = new IMDB_Search;

	if ($IMDB_Search->searchTitles($imdb_movie)) {
		$i=0;
		foreach($IMDB_Search->searchTitles($imdb_movie) as $value) {
			// print $value[1].' > '.$value[2]."\n";
			$search_array[$i][id] = $value[1] ;
			$search_array[$i][title] = $value[2] ;
			$i++;
			// if ($i > 10) { break; }
			}
			return $search_array;
		}
	else
		{ return false; }
	}

function imdb_lookup($imdb_id) {
	$imdb_url = 'http://imdb.com/title/'.$imdb_id.'/';
	$imdb_website = file_get_contents ($imdb_url);

	//init the class
	$IMDB_Parser = new IMDB_Parser;

	$movie_title = "Title: ".$IMDB_Parser->getMovieTitle($imdb_website);
	$movie_title = html_entity_decode_utf8 ($movie_title);
	$movie_tagline = "Tagline: ".$IMDB_Parser->getMovieTagline($imdb_website);
	$movie_tagline = html_entity_decode_utf8 ($movie_tagline);
	$movie_plot = "Plot: ".$IMDB_Parser->getMoviePlot($imdb_website);
	$movie_plot = html_entity_decode_utf8 ($movie_plot);
	$movie_year = "Year: ".$IMDB_Parser->getMovieYear($imdb_website);
	$movie_runtime = "Runtime: ".$IMDB_Parser->getMovieRuntime($imdb_website);
	$movie_stars = "Stars: ".$IMDB_Parser->getMovieStars($imdb_website);
	$movie_votes = "Votes: ".$IMDB_Parser->getMovieVotes($imdb_website);

	$imdb_lookup_txt = $movie_title . " | " . $movie_tagline . " | " . $movie_plot . " | " . $movie_year  . " | " . $movie_runtime  . " | " . $movie_stars  . " | " . $movie_votes;

	$imdb_lookup_txt = str_replace (array("\r\n", "\r", "\n"), " ", $imdb_lookup_txt);

	return $imdb_lookup_txt;
	}


function imdb_auto_lookup_fn(&$bot, $dest, $imdb_url) {
	//get a video detail site from imdb.com
	// $imdb_website = file_get_contents ('http://imdb.com/title/$imdb_url/'); 
	$imdb_website = file_get_contents ($imdb_url);


	// function getMovieActors($imdb_website, $name_and_id=False)
	// function getMovieDirectedBy($imdb_website, $all_directors=False)
	// function getMovieColor($imdb_website)
	// function getMovieCountry($imdb_website)
	// function getMovieLanguage($imdb_website)
	// function getMovieRating($imdb_website)
	// function getMovieGenres($imdb_website)
	// function getMoviePlot($imdb_website)
	// function getMovieTagline($imdb_website)
	// function getMovieTitle($imdb_website)
	// function getMoviePictureHtml($imdb_website)
	// function getMoviePicture($imdb_website)
	// function getMovieYear($imdb_website)
	// function getMovieRuntime($imdb_website)
	// function getMovieStars($imdb_website)
	// function getMovieAka($imdb_website)
	// function getMovieAwards($imdb_website)
	// function getMovieVotes($imdb_website)

	//init the class
	$IMDB_Parser = new IMDB_Parser;

	// output movie-picture (html)
	//print "<p>".$IMDB_Parser->getMoviePictureHtml($imdb_website)."</p>";

	// output movie-title
	$movie_title = "Title: ".$IMDB_Parser->getMovieTitle($imdb_website);
	$movie_title = html_entity_decode_utf8 ($movie_title);

	// output movie-tagline
	$movie_tagline = "Tagline: ".$IMDB_Parser->getMovieTagline($imdb_website);
	$movie_tagline = html_entity_decode_utf8 ($movie_tagline);

	// output movie-plot
	$movie_plot = "Plot: ".$IMDB_Parser->getMoviePlot($imdb_website);
	$movie_plot = html_entity_decode_utf8 ($movie_plot);

	// output movie-actors (array)
	// print "<br />Actors: ";
	// foreach($IMDB_Parser->getMovieActors($imdb_website) as $value)
	// {
	//     print $value." | ";
	// }

	// output movie-actors (array) (name and ID)
	// print "<br />Actors (link, ID, name): ";
	// $a_actors = $IMDB_Parser->getMovieActors($imdb_website, $name_and_id=True);
	// for ($i=0; $i<count($a_actors[1]); $i++)
	// {
	//     print $a_actors[0][$i].", ";
	//     print $a_actors[1][$i].", ";
	//     print $a_actors[2][$i].", ";
	//     print ' | ';
	// }

	// output movie-user rating
	// print "<br />User Rating: ".$IMDB_Parser->getMovieRating($imdb_website);
	
	// output movie-languages (array)
	// print "<br />Languages: ";
	// foreach($IMDB_Parser->getMovieLanguage($imdb_website) as $value)
	// {
	//     print $value." | ";
	// }

	// output movie-color
	// print "<br />Color: ".$IMDB_Parser->getMovieColor($imdb_website);

	// output movie-country (array)
	// print "<br />Country: ";
	// foreach($IMDB_Parser->getMovieCountry($imdb_website) as $value)
	// {
	//     print $value." | ";
	// }
	
	// output movie-directed by
	// print "<br />Directed by: ".$IMDB_Parser->getMovieDirectedBy($imdb_website);
	
	// output movie-picture path
	// print "<br />Picture Path: ".$IMDB_Parser->getMoviePicture($imdb_website);
	
	// output movie-genres (array)
	// print "<br />Genres: | ";
	// foreach($IMDB_Parser->getMovieGenres($imdb_website) as $value)
	// {
	//     print $value." | ";
	// }
	
	// output movie-year
	$movie_year = "Year: ".$IMDB_Parser->getMovieYear($imdb_website);
	
	// output runtime
	$movie_runtime = "Runtime: ".$IMDB_Parser->getMovieRuntime($imdb_website);
	
	// output moviestars
	$movie_stars = "Stars: ".$IMDB_Parser->getMovieStars($imdb_website);
	
	// output also known as
	//print "<br />Also known as: ".$IMDB_Parser->getMovieAka($imdb_website);
	
	// output votes
	$movie_votes = "Votes: ".$IMDB_Parser->getMovieVotes($imdb_website);

	$imdb_lookup_txt = $movie_title . " | " . $movie_tagline . " | " . $movie_plot . " | " . $movie_year  . " | " . $movie_runtime  . " | " . $movie_stars  . " | " . $movie_votes;

	$imdb_lookup_txt = str_replace (array("\r\n", "\r", "\n"), " ", $imdb_lookup_txt);

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
			$imdb_movie_nott = str_replace("tt", "", $imdb_movie);

			if (is_numeric ($imdb_movie_nott)) {
				$imdb_id = $imdb_movie;
				}
			else
				{
				// first have to search .. get id, and get data
				$imdb_search_array = imdb_search($imdb_movie);

				if (isset($imdb_search_array [0][id]))
					{
					$imdb_id = $imdb_search_array [0][id];
					$imdb_title = $imdb_search_array [0][title];
					}
				else
					{ return; }
				}

			$imdb_text = imdb_lookup($imdb_id);

			$bot->privMsg($dest, $imdb_text);
			}
		}
	}


function imdb_search_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 6))) == "!simdb") {
		if (strlen(trim($text)) < 7) {
			$bot->privMsg($nick, "Internet Movie Database Search syntax : !simdb <movie>");
			}
		else    
			{
			$imdb_movie = trim(substr($text, 7));

			//init the class
			$IMDB_Search = new IMDB_Search;

			if ($IMDB_Search->searchTitles($imdb_movie)) {
				$search_text = 'Found : ';
				$i=0;
				foreach($IMDB_Search->searchTitles($imdb_movie) as $value) {
					// print $value[1].' > '.$value[2]."\n";
					$value[2] = html_entity_decode_utf8($value[2]);
					$search_text = $search_text . "[" . $value[1] . "] " . $value[2] . " | ";
					$i++;
					if ($i > 10) { break; }
					}
				$bot->privMsg($dest, $search_text);
				}
			else
				{
				// print 'nothing found';
				$bot->privMsg($dest, "Nothing found matching $movie_title, champ.");
				}

			}
		}
	}


