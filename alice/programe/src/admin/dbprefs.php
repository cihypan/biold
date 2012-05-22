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

// Turn this off in case people have it on.
set_magic_quotes_runtime(0);

// Can't turn off magic quotes gpc so just redo what it did if it is on.
if (get_magic_quotes_gpc()) {
	foreach($HTTP_GET_VARS as $k=>$v)
		$HTTP_GET_VARS[$k] = stripslashes($v);
	foreach($HTTP_POST_VARS as $k=>$v)
		$HTTP_POST_VARS[$k] = stripslashes($v);
	foreach($HTTP_COOKIE_VARS as $k=>$v)
		$HTTP_COOKIE_VARS[$k] = stripslashes($v);
}


define("LOOPINGERRORMSG", "Oops. I wasn't paying attention. Tell me again what is going on.");
define("LOOPINGLIMIT",150); // -1 for no limit
define("RANDOMCHANCECLEAN",100); // -1 to never check
define("MINUTESTOKEEPDATA",120); // -1 to keep forever
define("MINUTESTOKEEPCHATLOG",-1); // -1 to keep forever
define("DEFAULTPREDICATEVALUE", "Undefined");
define("PROGRAMEVERSION","v0.08");

// This is where all the AIML and startup.xml resides
$rootdir="../../aiml/";

$DB_HOST="localhost";
$DB_UNAME="alice";
$DB_PWORD="programme";
$DB_DB="alice";

$errors="";

mysql_connect($DB_HOST,$DB_UNAME,$DB_PWORD) or $errors = $errors . "Could not connect to database.\n"; 
@mysql_select_db($DB_DB) or $errors = $errors . "Unable to select database\n";

?>
