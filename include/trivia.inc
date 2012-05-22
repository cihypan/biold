<?
function trivia_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $score_arr, $answered_flag, $answered_trivia_arr;

   if ((strcasecmp($nick, "teebag") == 0)) {
        return;
        }

   if (trim(strToLower(substr($text, 0, 12))) == "!trivia help") {
        $bot->privMsg($dest, "TRIVIA syntax : !trivia <category>, also available are : !trivia score, !trivia categories");
	return;
	}

   if (trim(strToLower(substr($text, 0, 13))) == "!trivia score") {
	$scores = trivia_show_scores();

	$bot->privMsg($dest, "TRIVIA SCORES : ".$scores);
	return;
	}

   if (trim(strToLower(substr($text, 0, 18))) == "!trivia categories") {
	$db_query = "SELECT DISTINCT(trivia_category) AS Category,COUNT(trivia_category) AS Category_count FROM questions GROUP BY trivia_category ORDER BY Category_count DESC LIMIT 0,10";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { $bot->privMsg($dest, "trivia_fn(): ERROR: ".mysql_errno().":".mysql_error()); }
	if ($db_send_query > 0) {
		$top_trivia = "[TOP 10 TRIVIA CATEGORIES] :";
                while ($trivia_row = mysql_fetch_object($db_send_query)) {
                        $trivia_category = $trivia_row->Category;
                        $trivia_category_count = $trivia_row->Category_count;

			if ($trivia_category == "") {
				$trivia_category = "MISC/UNKNOWN";
				}
			$top_trivia = $top_trivia." ".$trivia_category." [".$trivia_category_count."] |";
                        }
                mysql_free_result($db_send_query);
		$bot->privMsg($dest, $top_trivia);
		}     
	return;   	
	}

   if (trim(strToLower(substr($text, 0, 7))) == "!trivia") {
	$chosen_category = mysql_escape_string(trim(substr($text, 8)));
	if (strlen($chosen_category) > 3 && is_string($chosen_category)) 
		{
		$SQL_CATEGORY = " WHERE trivia_category LIKE '%".$chosen_category."%' ";
		}
	else 
		{
		$SQL_CATEGORY = "";
		}
		        	
        // has the last trivia been answered?
        // if not, spew the answer for last one....
        $last_unanswered_id = get_last_id();
        if (trivia_check_if_answered($last_unanswered_id) == 0) {
        	$db_query = "SELECT trivia_answer FROM questions WHERE trivia_id=".$last_unanswered_id;
		$db_send_query = @mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "trivia_fn(): ERROR: ".mysql_errno().":".mysql_error()); }
		if ($db_send_query > 0) {
			$row_obj = mysql_fetch_object($db_send_query);
			$trivia_unanswered = trim($row_obj->trivia_answer);
		}        	
		$bot->privMsg($dest, "Answer to last trivia : ".$trivia_unanswered);
	}
        // db getting too large to order by rand() :(
	// optimized by selecting only trivia_id by rand and then getting that question
	$db_query = "SELECT trivia_id FROM questions ".$SQL_CATEGORY." ORDER BY RAND() LIMIT 1";
	$db_send_query = @mysql_query ($db_query);
	if (mysql_errno()) { 
		// $bot->privMsg($dest, "trivia_fn(): ERROR: ".mysql_errno().":".mysql_error()); 
		$bot->privMsg($dest, "No matching trivia found... ");
		return;
		}
	if ($db_send_query > 0) {
		$row_obj = mysql_fetch_object($db_send_query);
		$trivia_id = $row_obj->trivia_id;
		}
	
	if (is_numeric($trivia_id) && $trivia_id > 0) {
		$db_query = "SELECT * FROM questions WHERE trivia_id=".$trivia_id;
		$db_send_query = @mysql_query ($db_query);
		if (mysql_errno()) { $bot->privMsg($dest, "trivia_fn(): ERROR: ".mysql_errno().":".mysql_error()); }
		if ($db_send_query > 0) {
			$question_obj = mysql_fetch_object($db_send_query);
			$question_category = strtoupper($question_obj->trivia_category);
			$question_question = $question_obj->trivia_question;
			$question_answer = $question_obj->trivia_answer;
			$question_id = $question_obj->trivia_id;

			last_id($question_id);
			$answered_flag = 0;

			if ($question_category == '') $question_category='MISC';

			$bot->privMsg($dest, "[$question_category] - $question_question");
			}
		}
	}
}


function check_trivia_answer ($id, $answer) {
	global $similar_answer_global;

	$sql = "SELECT trivia_answer FROM questions WHERE trivia_id=".$id; 
	$select = @mysql_query ($sql);
       	if (mysql_errno()) { echo "check_trivia_answer(): ERROR: ".mysql_errno().":".mysql_error(); }
       	if ($select > 0) {
		$row = mysql_fetch_object($select);

		$real_answer = strtolower(trim($row->trivia_answer));
		$real_answer = str_replace('#', '', $real_answer);

		$answer = strtolower(trim($answer));

		if (is_numeric($real_answer) && !is_numeric($answer)) {
			$NW = new numberstowords();
			$real_answer = trim($NW->n2w($real_answer));
		}
		
		// what about 
		// <steelrat> how about if is_numeric($answer) && !is_numeric($real_answer)?
		if (is_numeric($answer) && !is_numeric($real_answer)) {
			$NW = new numberstowords();
			$answer = trim($NW->n2w($answer));
		}

		$real_answer_nothe = str_replace('the ', '', $real_answer);

		$similar_answer = similar_text ($real_answer, $answer, $similar_pct);
		$similar_answer_global = number_format($similar_pct, 0);
	
		if (strcasecmp($answer, $real_answer) == 0 || strcasecmp($answer, $real_answer_nothe) == 0 || (number_format($similar_pct, 0) > 90)) 
			return 1;
		else
			return 0;
	}
	return 0;
}

function trivia_check_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $score_arr, $answered_flag, $similar_answer_global, $answered_trivia_arr;

   if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }

   if (trim(strToLower(substr($text, 0, 7))) == "!answer")
        {
        if (strlen(trim($text)) < 8)
                {
                $bot->privMsg($nick, "TRIVIA ANSWER syntax : !answer <answer to last question>");
                }
        else
                {
		$guessedAnswer = trim(substr($text, 8));
		$answer_id = get_last_id();

	        if ((check_trivia_answer($answer_id, $guessedAnswer )) == 1)
	                {
	                // right answer
			
			if (trivia_check_if_answered ($answer_id) == 0) {
				// set status 'R'ight
				$status = 'R';
				trivia_update_question_db ($answer_id, $nick, $status);
				trivia_score_right ($nick);
				$bot->privMsg("#sinned", $nick." got it right! [Answer is : $guessedAnswer]");
				}
	                }
	        else
	                {
	                // wrong answer

			if ((trivia_check_if_attempted ($answer_id, $nick) == 0) && (trivia_check_if_answered ($answer_id) == 0)) {
				// set status 'W'rong
				$status = 'W';
				trivia_update_question_db ($answer_id, $nick, $status);
				trivia_score_wrong ($nick);
				}

			$bot->privMsg($nick, "Similar text %".$similar_answer_global);
	                }
		}
	}
}

function trivia_show_scores () {
	$sql = "SELECT * FROM trivia_scores WHERE score_val > 0 ORDER BY score_val DESC";
       	$select = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_show_scores(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
       	if ($select > 0) {
       		while ($row = mysql_fetch_object($select)) {
			$nick = $row->score_nick;
			$score = $row->score_val;
			
			$scores_text = $scores_text." ".$nick." - ".$score." | ";	
			}
		}			
	return $scores_text;
}

function trivia_check_if_exists ($nick) {
	$nick = mysql_escape_string($nick);
	$sql = "SELECT * FROM trivia_scores WHERE score_nick='".$nick."'";
       	$check_exists = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_check_if_exists(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
       	if (@mysql_num_rows ($check_exists) > 0) 
       		return 1;  
       	else
		return 0;			
}

function trivia_score_right ($nick) {
	$nick = mysql_escape_string($nick);
	if (trivia_check_if_exists ($nick) == 1)
		$sql = "UPDATE trivia_scores SET score_val=score_val+100 WHERE score_nick='".$nick."'";
	else
		$sql = "INSERT INTO trivia_scores (score_nick, score_val) VALUES ('".$nick."',100)";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_score_right(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;	
}

function trivia_score_wrong ($nick) {
	$nick = mysql_escape_string($nick);
	if (trivia_check_if_exists ($nick) == 1)
		// got to check for negative scores... still...
		$sql = "UPDATE trivia_scores SET score_val=score_val-20 WHERE score_nick='".$nick."'";
	else
		$sql = "INSERT INTO trivia_scores (score_nick, score_val) VALUES ('".$nick."',0)";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_score_wrong(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;	
}

function trivia_score_reset ($nick) {
	if (!empty($nick)) {
		$nick = mysql_escape_string($nick);
		$sql = "DELETE FROM trivia_scores WHERE score_nick='".$nick."'";
	} else {
		trivia_questions_reset();
		$sql = "TRUNCATE TABLE trivia_scores";
		}
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_score_reset(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;
}

function trivia_questions_reset () {
	$sql = "TRUNCATE TABLE trivia_questions";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_questions_reset(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;
}

function trivia_update_question_db ($id, $nick, $status) {
	$sql = "INSERT INTO trivia_questions (question_trivia_id, question_nick, question_status) VALUES (".$id.", '".$nick."','".$status."')";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_update_question_db(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;
}

function trivia_check_if_answered ($id) {
	$sql = "SELECT * FROM trivia_questions WHERE question_trivia_id=".$id." AND question_status='R'";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_check_if_answered(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
       	if (@mysql_num_rows ($update) > 0) 
       		return 1;  
       	else
		return 0;
}

function trivia_check_if_attempted ($id, $nick) {
	$nick = mysql_escape_string($nick);
	$sql = "SELECT * FROM trivia_questions WHERE question_trivia_id=".$id." AND question_nick='".$nick."'";
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "trivia_check_if_attempted(): ERROR: ".mysql_errno().":".mysql_error()."\n"; }
       	if (@mysql_num_rows ($update) > 0) 
       		return 1;  
       	else
		return 0;
}

function last_id ($id) {
	$sql = "UPDATE trivia SET last_id=".$id;
       	$update = @mysql_query ($sql);
       	if (mysql_errno()) { echo "last_id()[$id]: ERROR: ".mysql_errno().":".mysql_error()."\n"; }
	return;
}

function get_last_id () {
	$sql = "SELECT * FROM trivia";
       	$select = @mysql_query ($sql);
       	if (mysql_errno()) { echo "get_last_id(): ERROR: ".mysql_errno().":".mysql_error(); }
       	if ($select > 0) {
		$row = mysql_fetch_object($select);
		return $row->last_id;
		}
}
?>
