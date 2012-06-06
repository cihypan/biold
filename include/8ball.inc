<?php
function ball_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 2))) == "!8")
        {
        if (strlen(trim($text)) < 6)
                {
                $bot->privMsg($dest, "8ball syntax : !8 <your question>");
                }
        else
                {
		$ball_question = trim(substr($text, 3));

		//Only reply if there is a question 
		If ($ball_question) 
			{ 
 
			//Chooses a random number 
			$result = Rand (1,20); 
		
			//Based on the random number, gives an answer 
			switch ($result) {
				case 1:
					$ball_answer_txt="The 8-ball says: As I see it, Yes"; 
					break;
				case 2:
					$ball_answer_txt="The 8-ball says: It is certain"; 
					break;
				case 3:
					$ball_answer_txt="The 8-ball says: It is decidedly so"; 
					break;
				case 4:
					$ball_answer_txt="The 8-ball says: Outlook good"; 
					break;
				case 5:
					$ball_answer_txt="The 8-ball says: Most likely"; 
					break;
				case 6:
					$ball_answer_txt="The 8-ball says: Signs point to yes"; 
					break;
				case 7:
					$ball_answer_txt="The 8-ball says: Without a doubt"; 
					break;
				case 8:
					$ball_answer_txt="The 8-ball says: Yes"; 
					break;
				case 9:
					$ball_answer_txt="The 8-ball says: Yes - definitely"; 
					break;
				case 10:
					$ball_answer_txt="The 8-ball says: You may rely on it"; 
					break;
				case 11:
					$ball_answer_txt="The 8-ball says: Reply hazy, try again"; 
					break;
				case 12:
					$ball_answer_txt="The 8-ball says: Ask again later"; 
					break;
				case 13:
					$ball_answer_txt="The 8-ball says: Better not tell you now"; 
					break;
				case 14:
					$ball_answer_txt="The 8-ball says: Cannot predict now"; 
					break;
				case 15:
					$ball_answer_txt="The 8-ball says: Concentrate and ask again"; 
					break;
				case 16:
					$ball_answer_txt="The 8-ball says: Don't count on it"; 
					break;
				case 17:
					$ball_answer_txt="The 8-ball says: My reply is no"; 
					break;
				case 18:
					$ball_answer_txt="The 8-ball says: My sources say no"; 
					break;
				case 19:
					$ball_answer_txt="The 8-ball says: Outlook not so good"; 
					break;
				case 20:
					$ball_answer_txt="The 8-ball says: Very doubtful"; 
					break;
				}

			$bot->privMsg($dest, $ball_answer_txt);
			} 
		}
	}
}
?>
