<?
function getmicrotime()
{
   list($usec, $sec) = explode(" ", microtime());
   return ((float)$usec + (float)$sec);
}

// a function to convert number of seconds (eg; 96172) to a readable format
// such as: 1 day, 2 hours, 42 mins, and 52 secs
// takes unix timestamp as input
// Based on the word_time function from PG+ (http://pgplus.ewtoo.org)
// 
// select unix_timestamp(now())-unix_timestamp(timestamp) from url_list;
function time_to_string($t=0) {
	if (!$t) return "no time at all";
	if ($t < 0) {
		$neg = 1;
		$t = 0 - $t;
	}

	$days = $t / 86400;
	$days = floor($days);
	$hrs = ($t / 3600) % 24;
	$mins = ($t / 60) % 60;
	$secs = $t % 60;

	$timestring = "";
	if ($neg) $timestring .= "negative ";
	if ($days) {
		$timestring .= "$days day" . ($days==1?"":"s");
		if ($hrs || $mins || $secs) $timestring .= ", ";
	}
	if ($hrs) {
		$timestring .= "$hrs hour" . ($hrs==1?"":"s");
		if ($mins && $secs) $timestring .= ", ";
		if (($mins && !$secs) || (!$mins && $secs)) $timestring .= " and ";
	}
	if ($mins) {
		$timestring .= "$mins min" . ($mins==1?"":"s");
		if ($mins && $secs) $timestring .= ", ";
		if ($secs) $timestring .= " and ";
	}
	if ($secs) $timestring .= "$secs sec" . ($secs==1?"":"s");	
	return $timestring;
}

function time_to_string_short($t=0) {
	if (!$t) return "no time at all";
	if ($t < 0) {
		$neg = 1;
		$t = 0 - $t;
	}

	$days = $t / 86400;
	$days = floor($days);
	$hrs = ($t / 3600) % 24;
	$mins = ($t / 60) % 60;
	$secs = $t % 60;

	$timestring = "";
	if ($neg) $timestring .= "negative ";
	if ($days) {
		$timestring .= "$days day" . ($days==1?"":"s");
		if ($hrs || $mins || $secs) $timestring .= ", ";
	}
	if ($hrs) {
		$timestring .= "$hrs hour" . ($hrs==1?"":"s");
		if ($mins && $secs) $timestring .= ", ";
		if (($mins && !$secs) || (!$mins && $secs)) $timestring .= " and ";
	}
	if ($mins) {
		$timestring .= "$mins min" . ($mins==1?"":"s");
	}
	return $timestring;
}
?>
