<?

/*
    Program E
    Copyright 2002, Paul Rydell
    Portions by Jay Myers
    
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

require_once "dbprefs.php";

global $selectbot;			


// Deletes everything about a bot.
// Takes an int as the parameter. The botid.
function deletebot($bot)		
{
	
	$q="delete from bot where bot=$bot";	
    $e = mysql_query($q);
    if ($e){
    }
    $q="delete from patterns where bot=$bot";	
    $e = mysql_query($q);
    if ($e){
    }
    $q="delete from templates where bot=$bot";
    $e = mysql_query($q);
    if ($e){
    }
    $q="delete from bots where id=$bot"; 
    $e = mysql_query($q);
    if ($e){
    }
    $q="delete from gmcache";			
    $e = mysql_query($q);
    if ($e){
    }


}

// Deletes information about a bot just in the cache and bot parameters tables.
// Used by the incremental bot loader program so it doesn't wipe out the whole bot on each aiml file load.
function deletejustbot($bot){			

	$q="delete from bots where id=$bot"; 
    $e = mysql_query($q);
    if ($e){
    }
	$q="delete from bot where bot=$bot";	
	$e = mysql_query($q);
	if ($e){
	}
    $q="delete from gmcache";			
    $e = mysql_query($q);
    if ($e){
    }

}

// Deletes the gmcache table. This needs to be called whenever the patterns or templates table is updated.
function flushcache()
{
    $q="delete from gmcache";			
    $e = mysql_query($q);
    if ($e){
    }
}

// Makes the keys of an array uppercase.
function upperkeysarray($testa)
{
    $newtesta=array();
    $newkeys=array_keys($testa);
    for ($x=0;$x<sizeof($newkeys);$x++){
        $newtesta[strtoupper($newkeys[$x])]=$testa[$newkeys[$x]];
    }
    return $newtesta;
}

// Add to the substitution include file
function addtosubs($string)
{

    global $fp;

    fwrite($fp,$string);

}

// Create the substitution include file
function createsubfile()
{

    global $fp;

    $fp = fopen ("subs.inc", "w+");

}

// Find a word in the patterns table given the word and the parent.
function findwordid($word,$parent)
{

    $word=addslashes($word);
    $query="select id,isend from patterns where word='$word' and parent=$parent";	

    $selectcode = mysql_query($query);
    if ($selectcode){
        if(!mysql_numrows($selectcode)){
            return 0;
        }
        else{
            while ($q = mysql_fetch_array($selectcode)){
                
                if ($q[1]==1){
                    setnotend($q[0]);
                }

                return $q[0];
            }
        }
    }
}

// Find a wildcard in the patterns table given the word and the parent.
function findwordidstar($word,$parent)
{

    if ($word=="*"){
        $val=3;
    }
    elseif ($word=="_"){
        $val=1;
    }
    $query="select id,isend from patterns where parent=$parent and word is null and ordera=$val";	
    
    $selectcode = mysql_query($query);
    if ($selectcode){
        if(!mysql_numrows($selectcode)){
            return 0;
        }
        else{
            while ($q = mysql_fetch_array($selectcode)){
                
                if ($q[1]==1){
                    setnotend($q[0]);
                }
                
                return $q[0];
            }
        }
    }
}

// Set an entry in the patterns table to not be flagged as the last word in its context.
function setnotend($wordid)
{

    $query="update patterns set isend=0 where id=$wordid";
    $q=mysql_query($query);
    if ($q){

    }

}

// Inserts the combined sentence or pattern into the patterns table.
function insertmysentence($mybigsentence)
{
    global $selectbot;		

    $sentencepart="";

    $newstarted=0;
    $parent=-$selectbot;	

    //Parse into invidividual words
    //Use split
    $allwords=split(" ",$mybigsentence);
    $qadd="";
    for ($x=0;$x<sizeof($allwords)+1;$x++){

        // Last word in context
        $lwic=0;

        if ($x==sizeof($allwords)){
            $word="";
        }
        else {
            $word=$allwords[$x];
        }
        
        if (strtoupper($word)=="<INPUT>"){
            $sentencepart="INPUT";
        } elseif (strtoupper($word)=="<THAT>"){
            $sentencepart="THAT";
        } elseif (strtoupper($word)=="<TOPIC>"){
            $sentencepart="TOPIC";
        }
        
        // Find out if it is the last word in its context
        if ($x==(sizeof($allwords)-1)){
            $lwic=1;
        }
		// Prevent some warnings by checking this first.
		elseif (($x+1) >= (sizeof($allwords))){
		
		}
        elseif ((strtoupper($allwords[$x+1])=="<THAT>") || (strtoupper($allwords[$x+1])=="<TOPIC>")){
            $lwic=1;
        }
        
        if (($word!="*")&&($word!="_")){

            if ($newstarted!=1){
                $wordid=findwordid($word,$parent);
            }
            
            if (($wordid!=0) && ($newstarted!=1)){
                $parent=$wordid;
            }
            else {
                
                $newstarted=1;

                $sword=addslashes($word);
                $qadd="($selectbot, null,'$sword',2,$parent,$lwic)";	

				$parent = insertwordpattern($qadd);
		


            }
        }
        elseif (($word=="*")||($word=="_")){

            if ($newstarted!=1){            
                $wordid=findwordidstar($word,$parent);
            }
            
            if (($wordid!=0) && ($newstarted!=1)){
                $parent=$wordid;
            }
            else {
                
                $newstarted=1;

                if ($word=="*"){
                    $val=3;
                }
                elseif ($word=="_"){
                    $val=1;
                }

                $qadd="($selectbot, null,null,$val,$parent,$lwic)";	

				$parent = insertwordpattern($qadd);


				
            }
        }
    }

    return $parent;

}

// Inserts an entry into the patterns table. Returns the ID of the new row inserted.
function insertwordpattern($qadd)
{

    $qcode=mysql_query("insert into patterns(bot,id,word,ordera,parent,isend) values $qadd");

	if ($qcode){

		return mysql_insert_id();
	}
	
}

// Inserts a template.
function insertmytemplate($idused,$template)
{
	
	global $selectbot,$templatesinserted;
    
    if (!templateexists($idused)){
        $templatesinserted++;

        $template=addslashes($template);
        $query="insert into templates (bot,id,template) values ($selectbot, $idused,'$template')";

        $qcode=mysql_query($query);
        if ($qcode){
        }
    }

}

// Checks if a template exists for a given pattern
function templateexists($idused)
{
    $query="select id from templates where id=$idused";

    $qcode=mysql_query($query);

    if ($qcode){
        if(!mysql_numrows($qcode)){
            return false;
        }
    }

    return true;

}

// Called by the XML parser that is parsing the startup.xml file.
function startS($parser,$name,$attrs)
{

    global $selectbot, $whaton, $startupwhich, $splitterarray, $inputarray, $genderarray, $personarray, $person2array, $allbots, $areinc;
    
    $attrs=upperkeysarray($attrs);

    if (strtoupper($name)=='LEARN') {
        $whaton = 'LEARN';
    }
    if (strtoupper($name)=="GENDER"){
        $startupwhich="GENDER";
    }
    elseif (strtoupper($name)=="INPUT"){
        $startupwhich="INPUT";
    }
    elseif (strtoupper($name)=="PERSON"){
        $startupwhich="PERSON";
    }
    elseif (strtoupper($name)=="PERSON2"){
        $startupwhich="PERSON2";
    }

    if (strtoupper($name)=="PROPERTY"){
        $q="insert into bot (bot,name,value) values ($selectbot,'" . addslashes($attrs["NAME"]) . "','" . addslashes($attrs["VALUE"]) . "')";	

        $qcode=mysql_query($q);
        if ($qcode){
        }

    }
    elseif (strtoupper($name)=="BOT") {					
		$bot = $attrs["ID"];					
		if (botexists($bot)){
			$existbotid = getbotid($bot);
			if ($areinc==1){
				deletebot($existbotid);	
			}
		}

		$asbot=addslashes($bot);
		$q="insert into bots (id,botname) values (null,'$asbot')";	
		$qcode=mysql_query($q);
		
		if ($areinc==1){
			if ($qcode){
			}
			$newbotid=mysql_insert_id();
		}
		else {
			$newbotid=$existbotid;
		}

		$selectbot=$newbotid;
		$allbots[]=$selectbot;

		#print "<font size='3'><b>Loading bot: $bot ($selectbot)<BR></b></font>\n";	
		flush();							
    }
    elseif (strtoupper($name)=="SPLITTER"){
        $splitterarray[]=$attrs["VALUE"];
    }
    elseif (strtoupper($name)=="SUBSTITUTE"){
        if (trim($attrs["FIND"])!=""){
            if ($startupwhich=="INPUT"){
                $inputarray[]=array($attrs["FIND"],$attrs["REPLACE"]);
            }
            elseif ($startupwhich=="GENDER"){
                $genderarray[]=array($attrs["FIND"],$attrs["REPLACE"]);
            }
            elseif ($startupwhich=="PERSON"){
                $personarray[]=array($attrs["FIND"],$attrs["REPLACE"]);
            }
            elseif ($startupwhich=="PERSON2"){
                $person2array[]=array($attrs["FIND"],$attrs["REPLACE"]);
            }
        }
    }
}

// Called by the XML parser that is parsing the startup.xml file.
function endS($parser,$name)
{
    global $whaton;
    if (strtoupper($name)=='LEARN') {
        $whaton = '';
    }
}

// Called by the XML parser that is parsing the startup.xml file.
function handlemeS($parser, $data)
{
    global $whaton, $learnfiles, $selectbot;
    if (strtoupper($whaton)=="LEARN"){
        if (trim($data)=="*"){
            learnallfiles($selectbot);        
        }
        else {
			$learnfiles[$selectbot][]=trim($data);
        }
    
    }
}

// Checks if a bot already exists.
function botexists($name){

    // search to get existing id
	$name=addslashes($name);
    $q="select id from bots where botname='$name'";
    $selectcode = mysql_query($q);
    if ($selectcode) {
        while ($q = mysql_fetch_array($selectcode)){
			return true;
        }
    }

    return false;

}

// Gets a bot's property value.
function getbotvalue($name)
{
    global $selectbot;	

    $q="select value from bot where name=" . addslashes($name) . " and bot=$selectbot";	

    $selectcode = mysql_query($q);
    if ($selectcode){
        if(!mysql_numrows($selectcode)){
                return "undefined";
        }
        else{
            while ($q = mysql_fetch_array($selectcode)){
                return $q["value"];
            }
        }
    }
}

// Gets the ID of a bot given its name.
function getbotid ($name)
{
    // search to get existing id
	$name=addslashes($name);
    $q="select id from bots where botname='$name'";
    $selectcode = mysql_query($q);
    if ($selectcode) {
        while ($q = mysql_fetch_array($selectcode)){
                return $q["id"];
        }
    }

}

function startElement($parser, $name, $attrs) 
{
    global $whaton,$template,$pattern,$recursive,$topic;

    if (strtoupper($name)=="CATEGORY"){
        $whaton="CATEGORY";
    }
    elseif (strtoupper($name)=="TEMPLATE"){
        $whaton="TEMPLATE";
        $template="";
        $recursive=0;
    }
    elseif (strtoupper($name)=="PATTERN"){
        $whaton="PATTERN";
    }
    elseif ((strtoupper($name)=="THAT")&&(strtoupper($whaton)!="TEMPLATE")){
        $whaton="THAT";
    }
    elseif (strtoupper($name)=="TOPIC"){
        $whaton="TOPIC";
    }

    if ((strtoupper($whaton)=="PATTERN")&&(strtoupper($name)!="PATTERN")){


        if (strtoupper($name)=="BOT"){

            $attrs = upperkeysarray($attrs);
            $pattern .= getbotvalue($attrs["NAME"]);

        }
        else{
            $pattern .= "<$name";

            while (list ($key, $val) = each ($attrs)) {
                $pattern .= " $key=\"$val\" ";
            }

            $pattern .= ">";
        }

    }
    elseif ((strtoupper($whaton)=="TEMPLATE")&&(strtoupper($name)!="TEMPLATE")){
        
        $template .="<$name";

        while (list ($key, $val) = each ($attrs)) {
            $template .= " $key=\"$val\" ";
        }        

        $template .=">";
    }
    elseif (strtoupper($whaton)=="TOPIC"){

        $attrs = upperkeysarray($attrs);
        $topic=$attrs["NAME"];

    }
}

function endElement($parser, $name) 
{

	global $whaton,$pattern,$template,$recursive,$topic,$that;
    
    if (strtoupper($name)=="TOPIC"){
        $topic="";
    }

    if (strtoupper($name)=="CATEGORY"){
        $template=trim($template);
        $topic=trim($topic);
        $that=trim($that);
        $pattern=trim($pattern);

        if ($that==""){
            $that="*";
        }
        if ($topic==""){
            $topic="*";
        }
        if ($pattern==""){
            $pattern="*";
        }

    
        $mybigsentence="<input> $pattern <that> $that <topic> $topic";

        $idused=insertmysentence($mybigsentence);
            
        insertmytemplate($idused,$template);

        // IIS doesn't flush properly unless it has a bunch of characters in it. This fills it with spaces.
		print "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             ";
        flush();

        $pattern="";
        $template="";
        $that="";

    }
    else {
        if ((strtoupper($whaton)=="PATTERN")&&(strtoupper($name)!="PATTERN")){
            if (strtoupper($name)!="BOT"){
                $pattern .="</$name>";
            }
        }
        elseif ((strtoupper($whaton)=="TEMPLATE")&&(strtoupper($name)!="TEMPLATE")){
            $template .="</$name>";
        }    
    }

}

function handleme($parser, $data)
{
    global $whaton,$pattern,$template,$topic,$that;
    
    if (strtoupper($whaton)=="PATTERN"){
        $pattern .= $data;
    }
    elseif (strtoupper($whaton)=="TOPIC"){
        $topic .= $data;
    }
    elseif (strtoupper($whaton)=="THAT"){
        $that .= $data;
    }
    elseif (strtoupper($whaton)=="TEMPLATE"){
        $template .= $data;
    }
	elseif (strtoupper($whaton)=="TEMPLATE"){
		$template .= "<![CDATA[$data]]>";
	}
}


// Parses startup.xml if the bot is loaded incrementally - one file at a time
// This is very hacky and may not work properly.
function loadstartupinc($fileid){

	global $rootdir,$learnfiles,$areinc,$allbots,$selectbot;
	
	$areinc=0;

	if ($fileid==1){
		$areinc=1;
	}

	print "<font size='3'>Loading startup.xml<BR></font>\n";
	$learnfiles = array(); # This array will hold the files to LEARN

	$file = $rootdir . "startup.xml";
	$xml_parser = xml_parser_create();
	xml_parser_set_option($xml_parser,XML_OPTION_CASE_FOLDING,0);
	xml_set_element_handler($xml_parser, "startS", "endS");
	xml_set_character_data_handler ($xml_parser, "handlemeS");
	if (!($fp = fopen($file, "r"))) {
		die("could not open XML input");
	}
	while ($data = fread($fp, 4096)) {
		if (!xml_parse($xml_parser, $data, feof($fp))) {
			die(sprintf("XML error: %s at line %d",
						xml_error_string(xml_get_error_code($xml_parser)),
						xml_get_current_line_number($xml_parser)));
		}
	}
	xml_parser_free($xml_parser);

	# For each of the bots learn all of the files

	$totalcounter=1;

	foreach ($allbots as $bot){

		# print "<font size='3'><b>Loading bot: $bot<BR></b></font>\n";	

	    $single_learnfiles = $learnfiles[$bot];
		$single_learnfiles = array_unique($single_learnfiles);

		foreach ($single_learnfiles as $file) {
			
			$selectbot=$bot;
			
			if ($totalcounter==$fileid){
				learn($file);
				return 0;
			}

			$totalcounter++;

		}
	}

	return 1;


}

// Parses startup.xml
function loadstartup()
{

    global $rootdir,$learnfiles,$allbots,$selectbot,$areinc;
    
	$areinc=1;

    print "<font size='3'>Loading startup.xml<BR></font>\n";
    $learnfiles = array(); // This array will hold the files to LEARN


    $file = $rootdir . "startup.xml";
    $xml_parser = xml_parser_create();
    xml_parser_set_option($xml_parser,XML_OPTION_CASE_FOLDING,0);
    xml_set_element_handler($xml_parser, "startS", "endS");
    xml_set_character_data_handler ($xml_parser, "handlemeS");
    if (!($fp = fopen($file, "r"))) {
        die("could not open XML input");
    }
    while ($data = fread($fp, 4096)) {
        if (!xml_parse($xml_parser, $data, feof($fp))) {
            die(sprintf("XML error: %s at line %d",
                        xml_error_string(xml_get_error_code($xml_parser)),
                        xml_get_current_line_number($xml_parser)));
        }
    }
    xml_parser_free($xml_parser);

	# For each of the bots learn all of the files
	foreach ($allbots as $bot){
		print "<font size='3'><b>Loading bot: $bot<BR></b></font>\n";	
	    $single_learnfiles = $learnfiles[$bot];
		$single_learnfiles = array_unique($single_learnfiles);
		foreach ($single_learnfiles as $file) {
			$selectbot=$bot;
			learn($file);
		}
	}

}

// Learn all the files in a directory ending with ".aiml"
function learnallfiles($curbot)
{
    global $rootdir,$learnfiles;

    $dir=opendir ($rootdir); 
    while ($file = readdir($dir)) { 
        
        if (substr($file,strpos($file,"."))==".aiml"){

            $learnfiles[$curbot][]=$file;
        }
    }

    closedir($dir);
}


// Learn the AIML string.
function learnstring($xmlstring)
{

    set_time_limit(600);
    $xml_parser = xml_parser_create();
    xml_parser_set_option($xml_parser,XML_OPTION_CASE_FOLDING,0);
    xml_set_element_handler($xml_parser, "startElement", "endElement");
    xml_set_character_data_handler ($xml_parser, "handleme");

    if (!xml_parse($xml_parser, $xmlstring)) {
        die(sprintf("XML error: %s at line %d",
                    xml_error_string(xml_get_error_code($xml_parser)),
                    xml_get_current_line_number($xml_parser)));
    }

    xml_parser_free($xml_parser);
}


// Learn an AIML file.
function learn($file)
{

    global $rootdir;
    set_time_limit(600);
    $xml_parser = xml_parser_create();
    xml_parser_set_option($xml_parser,XML_OPTION_CASE_FOLDING,0);
    xml_set_element_handler($xml_parser, "startElement", "endElement");
    xml_set_character_data_handler ($xml_parser, "handleme");
    print "<font size='3'>Loading data aiml file: $file<BR></font>\n";
    flush();

    if (strtoupper(substr($file,0,7))=="HTTP://"){
        $file=$file;
    }
    else {
        $file=$rootdir . $file;
    }
    
    if (!($fp = fopen($file, "r"))) {
        die("could not open XML input");
    }

    while ($data = fread($fp, 4096)) {
        if (!xml_parse($xml_parser, $data, feof($fp))) {
            die(sprintf("XML error: %s at line %d",
                        xml_error_string(xml_get_error_code($xml_parser)),
                        xml_get_current_line_number($xml_parser)));
        }
    } 
    fclose($fp);
    xml_parser_free($xml_parser);
}

// Start a timer
function ss_timing_start ($name = 'default') 
{
    global $ss_timing_start_times;
    $ss_timing_start_times[$name] = explode(' ', microtime());
}

// Stop a timer
function ss_timing_stop ($name = 'default') 
{
    global $ss_timing_stop_times;
    $ss_timing_stop_times[$name] = explode(' ', microtime());
}

// Get the timer value
function ss_timing_current ($name = 'default') 
{
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



function makesrphp($inarray,$sname)
{

    $myphp="\$" . $sname . "search=array(\n";

    for ($x=0;$x<sizeof($inarray);$x++){

        $searchvar=cleanforsearch($inarray[$x][0]);

        $beginsearch="";
        $endsearch="";

        if (substr($searchvar,0,1)==" "){
            $beginsearch="\\b";
        }
        if ((substr($searchvar,strlen($searchvar)-1,1))==" "){
            $endsearch="\\b";
        }
        $myphp.="\"/$beginsearch" . trim($searchvar) . "$endsearch/ie\",\n";

    }

    $myphp.=");\n";

    $myphp.="\$" . $sname . "replace=array(\n";

    for ($x=0;$x<sizeof($inarray);$x++){
        $myphp.="\"myfunc('" . cleanforreplace($inarray[$x][1]) . "')\",\n";
    }

    $myphp.=");\n";
    return $myphp;

}


// Load an AIML string. String must be valid XML.
// This is going to need to take bot as a parameter
function loadaimlstring($aimlstring,$botid)
{

	global $selectbot;

	$selectbot=$botid;

	flushcache();
	learnstring($aimlstring);

}


// Load an AIML string that is just a category.
// This is going to need to take bot as a parameter
function loadaimlcategory($aimlstring,$botid)
{

	global $selectbot;

	$selectbot=$botid;
	
	$aimlstring="<?xml version=\"1.0\" encoding=\"ISO-8859-1\"" . "?" . "><aiml version=\"1.0\">" . $aimlstring . "</aiml>";


	flushcache();

	learnstring($aimlstring);

}

function makesplitterphp($splitterarray)
{
    $splitterphp="\$likeperiodsearch=array(\n";
    for ($x=0;$x<sizeof($splitterarray);$x++){
        
        $splitterphp.="\"" . $splitterarray[$x] . "\",\n";

    }
    $splitterphp.=");\n";

    $splitterphp.="\$likeperiodreplace=array(\n";
    for ($x=0;$x<sizeof($splitterarray);$x++){
        
        $splitterphp.="\"" . "." . "\",\n";

    }
    $splitterphp.=");\n";
    
    return $splitterphp;
}

function cleanforreplace($input)
{
    $input = str_replace("\\", "\\\\", $input);
    $input = str_replace("\"", "\\\"", $input);
    $input = str_replace("'", "\'", $input);
    return trim($input);
}

function cleanforsearch($input)
{
    $input = str_replace("\\", "\\\\\\\\", $input);
    $input = str_replace("\"", "\\\"", $input);
    $input = str_replace("'", "\'", $input);
    $input = str_replace("/", "\/", $input);
    $input = str_replace("(", "\(", $input);
    $input = str_replace(")", "\)", $input);
    $input = str_replace(".", "\.", $input);
    return $input;
}

function makesubscode()
{

    global $genderarray,$personarray,$person2array,$inputarray,$splitterarray;

    $genderphp = makesrphp($genderarray, "gender");
    $personphp = makesrphp($personarray, "firstthird");
    $person2php = makesrphp($person2array, "firstsecond");
    $inputphp = makesrphp($inputarray, "contract");
    $splitterphp = makesplitterphp($splitterarray);

    createsubfile();
    addtosubs("<?\n");
    addtosubs($genderphp);
    addtosubs($personphp);
    addtosubs($person2php);
    addtosubs($inputphp);
    addtosubs($splitterphp);
    addtosubs("\n?>");

}

?>