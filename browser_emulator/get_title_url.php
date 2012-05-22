<?php
include "browseremulator.class.php";

// example code

//$be = new BrowserEmulator();
//$be->addHeaderLine("Referer", "http://previous.server.com/");
//$be->addHeaderLine("Accept-Encoding", "x-compress; x-zip");
//$be->addPostData("Submit", "OK");
//$be->addPostData("item", "42");
//$be->setAuth("admin", "secretpass");
// also possible:
// $be->setPort(10080);

//$file = $be->fopen("http://restricted.server.com:10080/somepage.html");
//$response = $be->getLastResponseHeaders();

//while ($line = fgets($file, 1024)) {
//    // do something with the file
//}
//fclose($file);

	// $url="http://www.youtube.com/watch?v=NXZpTC4MXhY";
	// try a url that requires bullshit user-agents
	$url="http://www.bing.com/maps/#JndoZXJlMT0yNjcrQXZvbmRhbGUrQXYlMmMrT3R0YXdhJTJjK0sxWis3RzcmYmI9NDUuMzg5ODkxMjgxNjg5OSU3ZS03NS42MzMzMTYwNDAwMzkxJTdlNDUuMzM2ODIyNTc3MzU3OCU3ZS03NS43NzU2MjMzMjE1MzMy";

	$be = new BrowserEmulator();

	$fp = $be->fopen($url);
	$result = '';
//	$result = fread($fp, 1024);


//	if ($fp) { print ("$fp"); }
//	exit();
//	while(!feof($fp)) {
//       		$result .= fread($fp, 1024);
//	}

//	print $result;
	
	$title_pattern='/<title>(.*)<\/title>/';

	// $fp = fopen($url, "rb");
	// plain fopen replaced by 'be' - browser emulator class
	$be = new BrowserEmulator();
	$fp = $be->fopen($url);

                if ($fp) {
                        print ("[DEBUG] trying to get title from $url\n");
                        for ($x = 1; $x < 40; $x++) {
                                $result = '';
                                $result = fread ($fp, 8192);

                                if (@preg_match ($title_pattern, $result, $title_array_match)) {
                                        $title_txt = trim($title_array_match[1]);
                                        if (strlen($title_txt) > 300) {
                                                if (stristr ($title_txt, "</title>")) {
                                                        // for some reason the above matches with </title> at end of string
                                                        // and floods the bot off the irc :)
                                                        // so this should cut it off
                                                        $title_txt = substr ($title_txt, 0, stristr ($title_txt, "</title>"));
                                                        }
                                                else    
                                                        {
                                                        $title_txt = substr ($title_txt, 0, 300);
                                                        }
                                                }

                                        print ( $domain_txt . $title_txt);
                                        // get out of the loop no need to read further
                                        $bytes_read = 8192 * $x;
                                        print ("[DEBUG] read " . $bytes_read . " bytes before title tag\n");
                                        break;
                                        }
                                }
                        fclose ($fp);
                        }
                else
                        {
                        // do nothing...
                        print ("[DEBUG] Failed to fetch open url :/");
                        }

?> 
