<?php

/*
    Program E
	Copyright 2002, Paul Rydell
	
	This file is part of Program E.
	
	Program E is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Program E is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Program E; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

// Class to hold the reponse
class Response
{
    var $response;
    var $patternsmatched;
	var $inputs;
    var $errors;
	var $timer;
}

// subs.inc has all of the substitution values and sentence splitter values
require_once "admin/subs.inc";

// Initialize information for search replace routines
$replacecounter=1;
$aftersearch=array();
$afterreplace=array();

function make_seed() {
	list($usec, $sec) = explode(' ', microtime());
	return (float) $sec + ((float) $usec * 100000);
}

// This function will clean up old data in the database that is not needed according to user defined settings.
function cleanup(){

	if ((RANDOMCHANCECLEAN == -1) || (MINUTESTOKEEPDATA == -1)){
		return;
	}

	mt_srand(make_seed());
	$randval = mt_rand(1,RANDOMCHANCECLEAN);

	if ($randval==RANDOMCHANCECLEAN){

		if (MINUTESTOKEEPDATA != -1){
		
			$clean_dstore="delete from dstore where enteredtime < date_add(now(), interval - " . MINUTESTOKEEPDATA . " minute)";
			$clean_thatstack="delete from thatstack where enteredtime < date_add(now(), interval - " . MINUTESTOKEEPDATA . " minute)";
			$clean_thatindex="delete from thatindex where enteredtime < date_add(now(), interval - " . MINUTESTOKEEPDATA . " minute)";

			$selectcode = mysql_query($clean_dstore);
			if ($selectcode){
			}
			$selectcode = mysql_query($clean_thatstack);
			if ($selectcode){
			}
			$selectcode = mysql_query($clean_thatindex);
			if ($selectcode){
			}

		}
		
		if (MINUTESTOKEEPCHATLOG != -1){
			
			$clean_convlog="delete from conversationlog where enteredtime < date_add(now(), interval - " . MINUTESTOKEEPCHATLOG . " minute) and " . whichbots();

			$selectcode = mysql_query($clean_convlog);
			if ($selectcode){
			}

		}

	}

}

// Check if a tag is an old style AIML tag. If it is then return its new name and the fact that it is deprecated.
function isdeprecated($tag,&$ttag){

	if ($tag=="FOR_FUN"){
		$tag="FORFUN";
	}
	if ($tag=="BOTMASTER"){
		$tag="MASTER";
	}
	if ($tag=="KIND_MUSIC"){
		$tag="KINDMUSIC";
	}
	if ($tag=="LOOK_LIKE"){
		$tag="LOOKLIKE";
	}

	if ($tag=="TALK_ABOUT"){
		$tag="TALKABOUT";
	}	
	$deptags=array("NAME","BIRTHDAY","BIRTHPLACE","BOYFRIEND","FAVORITEBAND","FAVORITEBOOK","FAVORITECOLOR","FAVORITEFOOD","FAVORITESONG","FAVORITEMOVIE","FORFUN","FRIENDS","GIRLFRIEND","KINDMUSIC","LOCATION","LOOKLIKE","MASTER","QUESTION","SIGN","TALKABOUT","WEAR");

	if (in_array($tag,$deptags)){
		$ttag=$tag;
		return true;
	}
	else {
		return false;
	}

}

// When doing substitution myfunc replaces the words to be substituted with ~~x~~ where x is an incremented integer instead of what should eventually be substituted. Then when all substitution is done another function will go through and replace the ~~x~~ with the real value.
function myfunc($input){
	
	global $replacecounter,$aftersearch,$afterreplace;

	$aftersearch[]="~~" . $replacecounter . "~~";
	$afterreplace[]=$input;

	return "~~" . $replacecounter++ . "~~";

}

// Get the ID, or IP of the user
function getid(){

	return getenv("REMOTE_ADDR");

}

// Shoud look like: Wed Nov 14 18:09:55 CST 2002
function getfdate(){

	return date("D M j G:i:s T Y");

}

// Gets the # of AIML categories stored in the database
function getsize(){

	$query="select count(*) from templates where " . whichbots();	

	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return 0;
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				return $q[0];
			}
		}
	}
	return "";

}

// Get information about the bot that was entered in the startup.xml
function botget($name){

	global $uid, $selectbot;		

	$name=addslashes($name);

	$query="select value from bot where name='$name' and bot = $selectbot";	

	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return "";
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				return $q[0];
			}
		}
	}
	return "";

}

// Get a value for some variable set by the AIML
function bget($name){

	global $uid;

	$name=addslashes($name);

	$query="select value from dstore where name='$name' and uid='$uid' order by id desc limit 1";

	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return DEFAULTPREDICATEVALUE;
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				return $q[0];
			}
		}
	}
	return DEFAULTPREDICATEVALUE;

}

// Set the value for some variable
function bset($name,$value){

	global $uid;

	$value=trim($value);
	$name=addslashes($name);
	$value=addslashes($value);

	$query="insert into dstore (uid,name,value) values ('$uid','$name','$value')";
	$selectcode = mysql_query($query);
	if ($selectcode){
	}

}

// Store the clients inputs into the database
// This is an array because it separates on .'s and other sentence splitters.
function addinputs($inputsarray){

	global $uid;

	$query="insert into dstore (uid,name,value) values ";

	for ($x=0;$x<sizeof($inputsarray);$x++){

		$value=addslashes(trim($inputsarray[$x]));

		$query.="('$uid','input','$value'),";

	}

	$query=substr($query,0,(strlen($query)-1));
	$selectcode = mysql_query($query);
	if ($selectcode){
	}

}

// Store the bots responses into the database
// This is an array because it separates on .'s and other sentence splitters.
function addthats($inputsarray){

	global $uid;


	$query="insert into thatindex (uid) values ('$uid')";

	$selectcode = mysql_query($query);
	if ($selectcode){
	}
	$thatidx=mysql_insert_id();

	$query="insert into thatstack (thatid,value) values ";

	for ($x=0;$x<sizeof($inputsarray);$x++){

		$value=trim($inputsarray[$x]);
		
		$value=strip_tags($value);
		$value=addslashes($value);

		$query.="($thatidx,'$value'),";
		
	}

	$query=substr($query,0,(strlen($query)-1));

	$selectcode = mysql_query($query);
	if ($selectcode){
	}

}

// Logs the whole input and response
function logconversation($input,$response){

	global $uid, $selectbot;

	$input=addslashes($input);
	$response=addslashes($response);

	$query="insert into conversationlog (uid,input,response,bot) values ('$uid','$input','$response',$selectbot)";
	$selectcode = mysql_query($query);
	if ($selectcode){
	}

}

// Get a previous thing the bot said
function getthat($index,$offset){

	global $uid;

	$index=$index-1;
	$offset=$offset-1;

	$query="select id from thatindex where uid='$uid' order by id desc limit $index,1";
	
	
	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return "";
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				$thatid=$q[0];
			}
		}
	}
	

	$query="select value from thatstack where thatid=$thatid order by id desc limit $offset,1";


	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return "";
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				return $q[0];
			}
		}
	}
	return "";

}

// Get a previous thing the client said
function getinput($index){

	global $uid;

	$offset=1;

	$query="select value from dstore where uid='$uid' and name='input' order by id desc limit $index,$offset";

	$selectcode = mysql_query($query);
	if ($selectcode){
		if(!mysql_numrows($selectcode)){
			return "";
		}
		else{
			while ($q = mysql_fetch_array($selectcode)){
				return $q[0];
			}
		}
	}
	return "";

}

// Take the user input and do all substitutions and split it into sentences
function normalsentences($input){

	global $contractsearch,$contractreplace,$abbrevsearch,$abbrevreplace,$removepunct,$likeperiodsearch,$likeperiodreplace,$aftersearch,$afterreplace,$replacecounter;

	$cfull=$input;

	$cfull=preg_replace($contractsearch,$contractreplace,$cfull);
	
	$cfull=str_replace($aftersearch,$afterreplace,$cfull);	

	$replacecounter=1;
	$aftersearch=array();
	$afterreplace=array();

	//$cfull=str_replace($removepunct,"",$cfull);
	$cfull=str_replace($likeperiodsearch,$likeperiodreplace,$cfull);

	$newsentences=array();

	// Now split based on .'s
	$cfulls=split("\.",$cfull);

	for ($x=0;$x<sizeof($cfulls);$x++){
		if (trim($cfulls[$x])==""){

		}
		else {
			$newsentences[]=$cfulls[$x];
		}
	}

	return $newsentences;
}

// Reverse the gender of a phrase
function gender($input){

	global $gendersearch,$genderreplace,$aftersearch,$afterreplace,$replacecounter;

	$newinput=preg_replace($gendersearch,$genderreplace,$input);

	$newinput=str_replace($aftersearch,$afterreplace,$newinput);
	
	$replacecounter=1;
	$aftersearch=array();
	$afterreplace=array();

	return $newinput;

}

// Do a first to third person replacement
function firstthird($input){

	global $firstthirdsearch,$firstthirdreplace,$aftersearch,$afterreplace,$contractsearch,$contractreplace,$replacecounter;

	$newinput=preg_replace($firstthirdsearch,$firstthirdreplace,$input);

	$newinput=str_replace($aftersearch,$afterreplace,$newinput);
	
	$replacecounter=1;
	$aftersearch=array();
	$afterreplace=array();

	return $newinput;

}

// Do a first to second person replacement
function firstsecond($input){

	global $firstsecondsearch,$firstsecondreplace,$aftersearch,$afterreplace,$replacecounter;

	$newinput=preg_replace($firstsecondsearch,$firstsecondreplace,$input);

	$newinput=str_replace($aftersearch,$afterreplace,$newinput);
	
	$replacecounter=1;
	$aftersearch=array();
	$afterreplace=array();

	return $newinput;

}

// Insert gossip into the database
function insertgossip($gossip){

	global $selectbot;

	$gossip=addslashes($gossip);

	$query="insert into gossip (gossip,bot) values ('$gossip'," . $selectbot . ")";
	$selectcode = mysql_query($query);
	if ($selectcode){
	}

}

// Used by GetXMLTree
function GetChildren($vals, &$i) { 

	$children = array(); 
	
	if (isset($vals[$i]['value'])) 
		array_push($children, $vals[$i]['value']); 

	while (++$i < count($vals)) { 
		
	if (!isset($vals[$i]['attributes'])){
		$vals[$i]['attributes']="";
	}
	if (!isset($vals[$i]['value'])){
		$vals[$i]['value']="";
	}
	
		switch ($vals[$i]['type']) { 
			case 'cdata': 
			array_push($children, $vals[$i]['value']); 
			break; 
		
			case 'complete': 
			array_push($children, array('tag' => $vals[$i]['tag'], 'attributes' => $vals[$i]['attributes'], 'value' => $vals[$i]['value'])); 
			break; 
			
			case 'open': 
			array_push($children, array('tag' => $vals[$i]['tag'],'attributes' => $vals[$i]['attributes'], 'children' => GetChildren($vals,$i))); 
			break; 

			case 'close': 
			return $children; 
		} 
	} 
} 

// Get an XML tree
function GetXMLTree($data) { 

	$p = xml_parser_create(); 
	xml_parser_set_option($p,XML_OPTION_CASE_FOLDING,0);
	
	xml_parse_into_struct($p, $data, $vals, $index); 
	xml_parser_free($p); 

	$tree = array(); 
	$i = 0; 

	if (!isset($vals[$i]['attributes'])){
		$vals[$i]['attributes']="";
	}

	array_push($tree, array('tag' => $vals[$i]['tag'], 'attributes' => $vals[$i]['attributes'], 'children' => GetChildren($vals, $i))); 
	return $tree; 

} 

// Start a timer
function ss_timing_start ($name = 'default') {
    global $ss_timing_start_times;
    $ss_timing_start_times[$name] = explode(' ', microtime());
}

// Stop a timer
function ss_timing_stop ($name = 'default') {
    global $ss_timing_stop_times;
    $ss_timing_stop_times[$name] = explode(' ', microtime());
}

// Get the timer value
function ss_timing_current ($name = 'default') {
    global $ss_timing_start_times, $ss_timing_stop_times;
    if (!isset($ss_timing_start_times[$name])) {
        return 0;
    }
    if (!isset($ss_timing_stop_times[$name])) {
        $stop_time = explode(' ', microtime());
    }
    else {
        $stop_time = $ss_timing_stop_times[$name];
    }
    // do the big numbers first so the small ones aren't lost
    $current = $stop_time[1] - $ss_timing_start_times[$name][1];
    $current += $stop_time[0] - $ss_timing_start_times[$name][0];
    return $current;
}

// Change the case of the keys of an array to all uppercase
function upperkeysarray($testa){
	
	$newtesta=$testa;
	if (is_array($testa)){
		$newtesta=array();
		$newkeys=array_keys($testa);
		for ($x=0;$x<sizeof($newkeys);$x++){
			$newtesta[strtoupper($newkeys[$x])]=$testa[$newkeys[$x]];
		}
	}
	return $newtesta;

}


function iscustomtag($tagname,&$functocall){

	global $cttags;

	if (in_array(strtoupper($tagname),$cttags)){
		$functocall="ct_" . $tagname;
		return true;
	}
	else {
		return false;
	}

}

function loadcustomtags(){

	global $cttags;
	$cttags=array();

	$definedfuncs = get_defined_functions();
	$definedfuncs=$definedfuncs["user"];

	// find all funcs in ["user"] funcs that match ct_??? and register each function and tag name
	foreach($definedfuncs as $x){
		if (substr($x,0,3)=="ct_"){
			$cttags[]=strtoupper(substr($x,3,strlen($x)));
		}
	}

}

function lookupbotid($botname){

	$name=addslashes($botname);
    $q="select id from bots where botname='$name'";
    $selectcode = mysql_query($q);
    if ($selectcode) {
        while ($q = mysql_fetch_array($selectcode)){
                return $q["id"];
        }
    }
	return -1;

}

function whichbots()
{
	global $selectbot;

	return "bot=$selectbot";
}

?>