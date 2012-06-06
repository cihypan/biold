<?php
function ecn($ticker) {

// since yahoo shut off their ECN service, its time to move on
// freerealtime.com seems to provide same, shrug
//
// url requires basic http auth, so, need wget or curl
// http://quotes.freerealtime.com/rt/frt/M?IM=quotes&symbol=MSFT&type=Quote
// 

  $ticker = strtoupper($ticker);

  $url = 'http://quotes.freerealtime.com/rt/frt/M?IM=quotes&symbol='.$ticker.'&type=Quote';

  // $fp = popen ("cat /tmp/ecn.txt","r");
  $fp = popen("wget -q --user=bioh --password=gupihuj -O /tmp/ecn.txt '$url' && cat /tmp/ecn.txt","r");
  $result = '';

  while(!feof($fp)) {
	$result .= fread($fp, 1024);
  }

	$stock_name_pattern ='/<.*><b>(.*)<\/b> <\/font><.*>(.*)<b>/';
	if (@preg_match ($stock_name_pattern, $result, $stock_array))
	{
		$stock_name = $stock_array[1];
		$stock_market = $stock_array[2];
	}

// <TD align=center nowrap><FONT face="arial,helvetica" size=-1><b>30.71</b></FONT></TD>
	$stock_last_pattern ='/<.*><b>(.*)<\/b><\/FONT><\/TD>/';
	if (@preg_match ($stock_last_pattern, $result, $stock_array))
	{
		$stock_last = $stock_array[1];
	}

// <TD align=center nowrap><FONT face="arial,helvetica" size=-1><IMG src="/gra/downarrow2.gif" width=11 height=10><FONT color=red> 0.0699 (0.23)</FONT></FONT></TD>
	$stock_change_pattern ='/<.*><.*><.*>(.*)<\/FONT><\/FONT><\/TD>/';
	if (@preg_match ($stock_change_pattern, $result, $stock_array))
	{
		if (stristr($stock_array[0],'down')) 
			{
			$stock_change_direction = "Down";
			}
		else
			{
			$stock_change_direction = "Up";
			}
		$stock_change = $stock_array[1];
	}

// <TD align=center nowrap><FONT face="arial,helvetica" size=-1>12:48</FONT></TD>

	$stock_time_pattern ='/<.*>([0-9]{1,2}:[0-9]{1,2})<\/FONT><\/TD>/';
	if (@preg_match ($stock_time_pattern, $result, $stock_array))
	{
		$stock_time = $stock_array[1];
	}

// <TD align=center nowrap><FONT face="arial,helvetica" size=-1>30.69 (10)</FONT></TD>
// <TD align=center nowrap><FONT face="arial,helvetica" size=-1>30.74 (10)</FONT></TD>

	$stock_bid_pattern ='/<.*>(.*)\([0-9]+\)<\/FONT><\/TD>/';
	if (@preg_match_all ($stock_bid_pattern, $result, $stock_array))
	{
		$stock_bid = $stock_array[1][0];
		$stock_ask = $stock_array[1][1];
	}

	$ecn = $stock_name.$stock_market." [last] ".$stock_last." @".$stock_time." [change %] ".$stock_change." (".$stock_change_direction.") [bid] ".$stock_bid." [ask] ".$stock_ask;

	$ecn = trim($ecn);
	return ($ecn); 
}

function ecn_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   // stock quotes... yay...
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 4))) == "!ecn") 
	{
	if (strlen(trim($text)) < 5) 
		{
		$bot->privMsg($dest, "ECN syntax : !ecn <stock_symbol>");
		}
	else	
		{
		$check_quote = trim(substr($text, 5));
		
		$ecn_quote = ecn($check_quote);

		if (strlen($ecn_quote) > 2)
			{
			$bot->privMsg($dest, $ecn_quote);
			}
		}
	}
}

function ecn_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
   // stock quotes... yay...
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }
   if (trim(strToLower(substr($text, 0, 4))) == "!ecn")
        {
        if (strlen(trim($text)) < 5)
                {
                $bot->privMsg($nick, "ECN syntax : !ecn <stock_symbol>");
                }
        else
                {
                $check_quote = trim(substr($text, 5));

                $ecn_quote = ecn($check_quote);
		$ecn_quote = trim(ecn_quote);

                if (strlen($ecn_quote) > 2)
                        {
                        $bot->privMsg($nick, $ecn_quote);
                        }
                }
        }
}

?>
