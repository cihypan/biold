<?php
// ***************************************************************
//       Author: Alexander Kabanov (shurikk@mail.ru)
//      Version: $Id$
//  Description: This class allows you to get stock quotes
//               from yahoo server.
//
// This program is free software. You can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 
// of the License.
//
// ***************************************************************

class stocks {
    var $_URL  = 'http://finance.yahoo.com/d/quotes.csv?f=sl1d1t1c1ohgv&e=.csv&s=';

    function stocks() {
        // do nothing
    }

    function get_quotes($stocks_list) {
        if (!$stocks_list) return array();
        
        $this->stocks_list = $stocks_list;
        
        $symbols = '';
        foreach($this->stocks_list as $symbol) {
            $symbol = rawurldecode($symbol);
            $symbols .= $symbols == '' ? $symbol : '+'.$symbol;
        }
        
        $lines  = $this->get_data($symbols);
        $this->last_quotes = $this->calculate($lines);
        
        return $this->last_quotes;
    }

    function get_data(&$symbols) {
        $url = $this->_URL.$symbols;
        $fp = fopen($url, "r");
        $result = '';
        
        while(!feof($fp)) {
            $result .= fread($fp, 1024);
        }
        $lines = split("\n", $result, count($this->stocks_list));
        
        return $lines;
    }

    function calculate(&$lines) {
        $quotes = array();
        
        foreach($lines as $line) {
            $data = $this->parse($line);
            
            if ($data[1] > 0 && $data[4] != 0) {
                $pchange = round((10000*$data[1]/($data[1]-$data[4]))/100-100, 2);
            }
            else $pchange = 0;
            
            if ($data[4] > 0) {
                $pchange = '+'.$pchange;
            }
            else if($data[4] == 0) $pchange = 0;
            
            // $name = isset($this->stocks_list[$data[0]]) ? $this->stocks_list[$data[0]] : $data[0];
            // $name = $name != '' ? $name : $data[0];
            
            $quotes[] = array(
                    'symbol'     => $data[0],
                    'price_last' => $data[1],
                    'date'       => $data[2],
                    'time'       => $data[3],
                    'dchange'    => $data[4],
                    'price_min'  => $data[5],
                    'price_max'  => $data[6],
                    'pchange'    => $pchange,
                    'volume'     => $data[8]
            );
        }
        
        return $quotes;
    }

    function parse(&$line) {
        $line = ereg_replace('"','',$line);
        
        //symbol, price_last, date, time, dchange, price_min, price_max, not used, volume
        return split(',', $line);
    }

    function get_last() {
        return $this->last_quotes;
    }
}


function nymex_fn() {

  // $ticker = urlencode($ticker);
  // $fp = fopen("nymex.html","r");
  $fp = fopen("http://www.nymex.com", "r");
  $result = '';

  if (!$fp) { return; };
  while(!feof($fp)) {
	$result .= fread($fp, 1024);
  }
  fclose ($fp);

// oil price
// <span id="LastCL" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 106; LEFT: 665px; COLOR: black; POSITION: absolute; TOP: 380px">146.30</span>
  $oil_pattern='/<span id=\"LastCL\" (.*)>(.*)<\/span><span id=\"LastBZ\"/is';
  if (@preg_match ($oil_pattern, $result, $stock))
        {
	// print_r($stock);
        $oil_price = $stock[2];
        }

// oil change
// <span id="ChangeCL" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 114; LEFT: 720px; POSITION: absolute; TOP: 380px"><font color="Green">+1.12</font></span><span id="ChangeBZ"
  $oil_pattern='/<span id=\"ChangeCL\" (.*)><font color=\"(Green|Red)\">(.*)<\/font><\/span><span id=\"ChangeBZ\"/is';
  if (@preg_match ($oil_pattern, $result, $stock))
        {
	// print_r($stock);
        $oil_price_change = $stock[3];
        }

// gold price
// <span id="LastGC" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 109; LEFT: 665px; COLOR: black; POSITION: absolute; TOP: 450px">984.80</span><span id="LastSI"
   $gold_pattern='/<span id=\"LastGC\" (.*)>(.*)<\/span><span id=\"LastSI\"/is';
  if (@preg_match ($gold_pattern, $result, $stock))
        {
	// print_r($stock);
        $gold_price = $stock[2];
	}

// gold change
// <span id="ChangeGC" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 117; LEFT: 720px; POSITION: absolute; TOP: 450px"><font color="Green">+11.10</font></span><span id="ChangeSI"
  $gold_pattern='/<span id=\"ChangeGC\" (.*)><font color=\"(Green|Red)\">(.*)<\/font><\/span><span id=\"ChangeSI\"/is';
  if (@preg_match ($gold_pattern, $result, $stock))
        {
	// print_r($stock);
        $gold_price_change = $stock[3];
        }

// natural gas
// <span id="LastNG" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 108; LEFT: 665px; COLOR: black; POSITION: absolute; TOP: 436px">12.035</span><span id="LastGC"
   $gas_pattern='/<span id=\"LastNG\" (.*)>(.*)<\/span><span id=\"LastGC\"/is';
  if (@preg_match ($gas_pattern, $result, $stock))
        {
	// print_r($stock);
        $gas_price = $stock[2];
	}

// natural gas change
// <span id="ChangeNG" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 116; LEFT: 720px; POSITION: absolute; TOP: 436px"><font color="Green">+0.076</font></span><span id="ChangeGC"
  $gas_pattern='/<span id=\"ChangeNG\" (.*)><font color=\"(Green|Red)\">(.*)<\/font><\/span><span id=\"ChangeGC\"/is';
  if (@preg_match ($gas_pattern, $result, $stock))
        {
	// print_r($stock);
        $gas_price_change = $stock[3];
	}

// heating oil
// <span id="LastHO" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 104; LEFT: 665px; COLOR: black; POSITION: absolute; TOP: 408px">4.1043</span><span id="LastRB"
   $ho_pattern='/<span id=\"LastHO\" (.*)>(.*)<\/span><span id=\"LastRB\"/is';
  if (@preg_match ($ho_pattern, $result, $stock))
        {
	// print_r($stock);
        $ho_price = $stock[2];
	}

// heating oil change
// <span id="ChangeHO" ms_positioning="FlowLayout" style="display:inline-block;DISPLAY: inline; FONT-WEIGHT: bold; FONT-SIZE: 11px; Z-INDEX: 112; LEFT: 720px; POSITION: absolute; TOP: 408px"><font color="Green">+0.0394</font></span><span id="ChangeRB"
  $ho_pattern ='/<span id=\"ChangeHO\" (.*)><font color=\"(Green|Red)\">(.*)<\/font><\/span><span id=\"ChangeRB\"/is';
  if (@preg_match ($ho_pattern, $result, $stock))
        {
	// print_r($stock);
        $ho_price_change = $stock[3];
	}

	$oil_price_pct = (float)$oil_price_change / (float)$oil_price * 100;
	$oil_price_pct = round($oil_price_pct, 2);
	$gold_price_pct = (float)$gold_price_change / (float)$gold_price * 100;
	$gold_price_pct = round($gold_price_pct, 2);
	$gas_price_pct = (float)$gas_price_change / (float)$gas_price * 100;
	$gas_price_pct = round($gas_price_pct, 2);
	$ho_price_pct = (float)$ho_price_change / (float)$ho_price * 100;
	$ho_price_pct = round($ho_price_pct, 2);
	
	$nymex_txt = "Crude Oil : \$$oil_price $oil_price_change ($oil_price_pct%) | Gold : \$$gold_price $gold_price_change ($gold_price_pct%) | Natural Gas : \$$gas_price $gas_price_change ($gas_price_pct%) | Heating Oil : \$$ho_price $ho_price_change ($ho_price_pct%)";

   	return ($nymex_txt); 
}

function google_ticker_fn($ticker) {

	// for reference
	// this could be used to change this stupid scraping
	// http://www.google.com/ig/api?stock=GOOG

	$ticker = urlencode($ticker);
	// $fp = fopen("google.test.txt","r");
	$fp = fopen("http://finance.google.com/finance?q=$ticker", "r");
	$result = '';

	if ($fp) {
		while(!feof($fp)) {
			$result .= fread($fp, 1024);
			}
		}

        $stock_pattern='/<span id=\"ref_[0-9]{2,9}_ltt\">.([0-9]{1,2}):([0-9]{1,2})(AM|PM) E[DS]T.<\/span>.<\/span>.<div class=mdata-dis>/s';
        if (@preg_match ($stock_pattern, $result, $stock))
        {
        // print_r($stock);
        // [1] - hour
        // [2] - minute
        // [3] - AM / PM
        $quote_time = $stock[1].":".$stock[2]." ".$stock[3];
        $quote_time_converted = strtotime ($quote_time);
        $current_time = time();

        $realtime_check = $quote_time_converted - $current_time;

        // print ("[DEBUG] $quote_time | $quote_time_converted | $current_time | $realtime_check\n");
        }


	// var _companyName = 'Apple Inc.';
	$stock_pattern='/var _companyName = \'(.*)\'/';
	if (@preg_match ($stock_pattern, $result, $stock))
	{
	// print_r($stock);
	$stock_name = $stock[1];
	}

	// var _ticker = 'NASDAQ:AAPL';
	$stock_pattern='/var _ticker = \'(.*)\'/';
	if (@preg_match ($stock_pattern, $result, $stock))
	{
	// print_r($stock);
	$stock_ticker = $stock[1];
	}

	// <span class="pr">
	// <span id="ref_22144_l">146.90</span>
	$stock_pattern='/<span class=\"pr\">.<span id=\"ref_[0-9]{2,20}_l\">([0-9.]+)<\/span>.<\/span>/s';
	if (@preg_match ($stock_pattern, $result, $stock))
       	{
	// print_r($stock);
	$stock_price = $stock[1];
       	}

	// <span class="ch bld"><span class="chg" id="ref_22144_c">+0.02</span>
	$stock_pattern='/<span class=\"ch bld\"><span class=\"ch[gr]\" id=\"ref_[0-9]{2,20}_c\">(.*)<\/span>/';
	if (@preg_match_all ($stock_pattern, $result, $stock))
        {
        // print_r($stock);
        $stock_price_change = $stock[1][0];
        }

	// <span class="chg" id="ref_22144_cp">(0.01%)</span>
	$stock_pattern='/<span class=\"ch[gr]\" id=\"ref_[0-9]{2,20}_cp\">(.*)<\/span>/';
	if (@preg_match_all ($stock_pattern, $result, $stock))
        {
        // print_r($stock);
        $stock_price_change_pct = $stock[1][0];
        }

	// <span id="ref_22144_ltt">
	// 11:51AM EDT
	// </span>
	$stock_pattern='/<span id=\"ref_[0-9]{2,9}_ltt\">(.*) E[DS]T<\/span><\/span>/s';
	if (@preg_match_all ($stock_pattern, $result, $stock))
	{
	print_r($stock);
       	$stock_rt = $stock[1][0];
       	}

	// <span class="goog-inline-block key">Range</span>
	// <span class="goog-inline-block val">145.57 - 147.91</span>
	$stock_pattern='/<span class=\"goog-inline-block val\">(.*)<\/span>/';
	if (@preg_match_all ($stock_pattern, $result, $stock_val))
        {
        // print_r($stock);
        }

	// <span class="goog-inline-block key">Range</span>
	$stock_pattern='/<span class=\"goog-inline-block key\">(.*)<\/span>/';
	if (@preg_match_all ($stock_pattern, $result, $stock_key))
        {
        // print_r($stock);
        }

	// latest changes in google finance :/
	// get rid of newlines... sigh
	$result = str_replace ("\n", "", $result);

	// <table class="snap-data">
        $stock_pattern='/<td class=\"val\">([\d.]+) - ([\d.]+)<\/td><\/tr>/';
        if (@preg_match_all ($stock_pattern, $result, $stock_ranges))
        {
        // print_r($stock_val);
        }

        $stock_pattern='/<td class=\"val\">([\d.]+)<\/td><\/tr>/';
        if (@preg_match_all ($stock_pattern, $result, $stock_single_prices))
        {
        // print_r($stock_val);
        }

        $stock_pattern='/<td class=\"val\">([\d.]+)([A-Z])<\/td><\/tr>/';
        if (@preg_match_all ($stock_pattern, $result, $stock_capshares))
        {
        // print_r($stock_val);
        }

        $stock_pattern='/<td class=\"val\">([\d.]+)([A-Z])\/([\d.]+)([A-Z])<\/td><\/tr>/';
        if (@preg_match_all ($stock_pattern, $result, $stock_vol))
        {
        // print_r($stock_val);
        }

	$stock_range_val = $stock_ranges[1][0] . " - " . $stock_ranges[1][1];
	$stock_52wk_val = $stock_ranges[2][0] . " - " . $stock_ranges[2][1];
	$stock_open = $stock_single_prices[1][0];
	$stock_vol = $stock_vol[1][0].$stock_vol[2][0]."/".$stock_vol[3][0].$stock_vol[4][0];
	$stock_cap = $stock_capshares[1][0] . $stock_capshares[2][0];
	$stock_shares = $stock_capshares[1][1] . $stock_capshres[2][1];;

	if (strlen($stock_name) == 0)
		{
		return;
		}

	$stock_ticker = urldecode($stock_ticker);
	$stock_name = urldecode($stock_name);
	$realtime_quote = $stock_name . " (" . $stock_ticker .") : $" . $stock_price . " (" . $stock_price_change . ") " . $stock_price_change_pct . " { " . $stock_range_val . " }   [ 52wk  " . $stock_52wk_val . " ]  [ Vol/Avg.  " . $stock_vol . " ]  [ Mkt cap  ". $stock_cap . " ]  [ Shares  " . $stock_shares . " ] [ " . $quote_time . " ]";

	return ($realtime_quote); 
}

function oil_fn (&$bot, $nick, $ident, $host, $dest, $text) {
	if (trim(strToLower(substr($text, 0, 4))) == "!oil") {
		$bot->privMsg($dest, nymex_fn());
		}
	}

function ticker_fn(&$bot, $nick, $ident, $host, $dest, $text) {
	// stock quotes... yay...
	if (trim(strToLower(substr($text, 0, 7))) == "!ticker") {
		if (strlen(trim($text)) < 8) {
			$bot->privMsg($dest, "Ticker syntax : !ticker <stock_symbol> <stock_symbol2> ...");
			}
		else	{
			$check_quotes = trim(substr($text, 8));
	
// yahoo old way ... changed to realtime google
//			$stocks = new stocks();
//			$quotes = explode (" ", $check_quotes);
	
//			$result = $stocks->get_quotes($quotes);

//			for ($i=0; $i<count($result); $i++) {
//				$stock_out = implode (" | ", $result[$i]);
//				$bot->privMsg($dest, $stock_out);
//				}

			$quotes_array = explode (" ", $check_quotes);
			foreach ($quotes_array as $quotes_array_key => $quotes_array_val) {
				// hack for ^dji as google likes to call it INDEXDJX:.DJI
				if ($quotes_array_val == "^dji") {
					$quotes_array_val = "INDEXDJX:.DJI";
					}
				$result = google_ticker_fn($quotes_array_val);
				$bot->privMsg($dest, $result);
				}

			}
		}
	}

function ticker_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
	// stock quotes... yay...
	if (trim(strToLower(substr($text, 0, 7))) == "!ticker") {
		if (strlen(trim($text)) < 8) {
			$bot->privMsg($nick, "Ticker syntax : !ticker <stock_symbol> <stock_symbol2> ...");
			}
		else	{
			$check_quotes = trim(substr($text, 8));
	
			// yahoo ... old way ... changed to google / realtime
			// $stocks = new stocks();
			// $quotes = explode (" ", $check_quotes);
			// $result = $stocks->get_quotes($quotes);
			// for ($i=0; $i<count($result); $i++) {
		 	//	$stock_out = implode (" | ", $result[$i]);
			//	$bot->privMsg($nick, $stock_out);
			//	}

			$quotes_array = explode (" ", $check_quotes);
			foreach ($quotes_array as $quotes_array_key => $quotes_array_val) {
				// hack for ^dji as google likes to call it INDEXDJX:.DJI
				if ($quotes_array_val == "^dji") {
					$quotes_array_val = "INDEXDJX:.DJI";
					}
				$result = google_ticker_fn($quotes_array_val);
				$bot->privMsg($nick, $result);
				}
			}
		}
	}
	
function ticker_lookup($t_parameter, $company, $m_parameter) {
//this is the company you wish to lookup
// $company = "microsystems";

// t parameter can be :
//	[S]tocks
//	[O]ptions
// 	[M]utual Funds
//	[I]ndices

// m parameter can be :
//	[US]US & Canada
//	[] World Markets

//this is the url from yahoo + the company
$url = "http://quote.yahoo.com/l?m=".$m_parameter."&s=" . $company . "&t=".$t_parameter;

$readfile = file_get_contents($url);
$readfile = ereg("<!--- Background table--->(.*)<!--- End of Background Table --->" , $readfile, $content);
$stock_table = $content[1];

// <TR bgcolor=#ffffff><TD><font face=arial size=-1><a href="/q?s=NTLI&d=t">NTLI</a></font></TD><TD><font face=arial size=-1>NTL Inc</font></TD><TD><font face=arial size=-1>NasdaqNM</font></TD><TD><font face=arial size=-1>Broadcasting &amp; Cable TV</font></TD><TD><font face=arial size=-1><a href="http://rd.yahoo.com/addtomy/*http://edit.finance.dcn.yahoo.com/ec?.intl=us&.src=quote&.portfover=1&.done=http://finance.yahoo.com&.cancelPage=http://finance.yahoo.com/l?m%3d%26s%3dntl%26t%3d&.sym=NTLI&.nm=NTLI"><center>Add</center></a></font></TD></TR>

// new format :
// <tr bgcolor="ffffff"><td><a href="/q?s=AAAE.OB">AAAE.OB</a></td><td>AAA ENERGY INC</td><td>OTC BB</td><td>Industrial Metals &amp; Minerals</td><td align="center"><a href="http://us.rd.yahoo.com/addtomy/*http://edit.finance.yahoo.com/ec?.intl=us&amp;.src=quote&amp;.portover=1&amp;.done=http://finance.yahoo.com&amp;.cancelPage=http://finance.yahoo.com/lookup?s=energy&amp;t=S&amp;m=US&amp;r=3&amp;.sym=AAAE.OB&amp;.nm=AAAE.OB">Add</a></td></tr>

// need to find ticker, company and exchange in above shiat
$stock_pattern = '/<TR bgcolor=#ffffff><TD><font face=arial size=-1><a href=\"\/q\?s=(.*)&d=t\">(.*)<\/a><\/font><\/TD>/';
$check_stock = preg_match_all ($stock_pattern, $stock_table, $stock_check);
// print_r ($stock_check);

// all the stocks in format like above in the below array
$stocks_array = $stock_check[0];

$stock_symbols_array = $stock_check[1];
// print_r ($stock_symbols_array);

reset($stocks_array);
while (list($key, $val) = each($stocks_array)) {
	$stock_pattern = '/<TR bgcolor=#ffffff><TD><font face=arial size=-1><a href=\"\/q\?s=(.*)&d=t\">(.*)<\/a><\/font><\/TD><TD><font face=arial size=-1>([a-zA-Z\ ]+)<\/font><\/TD><TD><font face=arial size=-1>([a-zA-Z\ ]+)<\/font><\/TD><TD><font face=arial size=-1>/';

	$stock_match = preg_match($stock_pattern, $val, $stock_out);
	// print_r ($stock_out);
	if (!empty($stock_out[2]) && !empty($stock_out[3]))
		$stock_listing[$key]=$stock_out[2]." - ".$stock_out[3]." - ".$stock_out[4];
	}

return $stock_listing;
}

function ticker_lookup_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   // stock quotes... yay...
  if ((strcasecmp($nick, "teebag") == 0)) {
        return;
        }
   if (trim(strToLower(substr($text, 0, 8))) == "!lticker") 
   	{
	// ticker_lookup($t_parameter, $company, $m_parameter)
	if (strlen(trim($text)) < 9) 
		{
		$bot->privMsg($nick, "Lookup Ticker Syntax : !lticker <company> <market type> <location>");
		$bot->privMsg($nick, "    <company> : company ticker to look for");
		$bot->privMsg($nick, "    <market type> : [S]tocks, [O]ptions, [M]utual funds, [I]ndices");
		$bot->privMsg($nick, "    <location> : [US], blank defaults to World Markets");
		} 
	else 
		{
		$params_text = trim(substr($text, 9));
		$params = explode (" ", $params_text);	

		if (!empty($params[0])) 
			{
			$company = $params[0];
			} 

		if (!empty($params[1]) && 
		    ((strcasecmp($params[1], "M") == 0) || 
			(strcasecmp($params[1], "O") == 0) || 
			(strcasecmp($params[1], "S") == 0) ||
			(strcasecmp($params[1], "I") == 0) )) 
			{
			$t_parameter = strtoupper($params[1]);
			}

		if (!empty($params[2]) && 
		    (strcasecmp($params[2], "US") == 0)) 
		    	{
			$m_parameter = strtoupper($params[2]);
			} 
		else 
			{
			$m_parameter = "";
			}

		$stocks_list = ticker_lookup($t_parameter, $company, $m_parameter);

		if (count ($stocks_list) > 0) {
			reset($stocks_list);
			if (count ($stocks_list) > 5)
				$bot->privMsg($nick, "[note] -- Output Limited To 1 Record per second... --");
			if (count ($stocks_list) > 25)
				$bot->privMsg($nick, "Search Returned ".count($stocks_list)." matches....Limiting display to first 25 ...");

			$bot->privMsg($nick, "[format] Ticker - Company Name - Market");
			while (list($key, $val) = each($stocks_list)) {
				$bot->privMsg($nick, $val);
				// dont flood... eh... :(
				if (count ($stocks_list) > 5) sleep(1);
				$max_count++;
				if ($max_count > 25) return;
				}
			}
		else
			{
			$bot->privMsg($nick, "Search returned nothing....");
			}
		}
	}
}
?>
