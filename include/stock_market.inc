<?
// stock market fantasy game addon
// 
// functions in this include module : 
// 
// internal utility/private functions called by the main functions 
//
// get_ticker_price($ticker) 
// check_user ($nick) 
// check_ifowned($nick_id, $ticker) 
// check_stock ($nick_id, $ticker) 
// check_time () 
// update_user ($nick_id, $nick_amount, $action, $numstocks) 
// check_lastreset ($nick_id) 
// count_days( $a, $b ) 
// sortByInteger(&$personDetails, $field) 
//
// main functions providing core functionality to the module
// 
// newuser_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) 
// buy_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) 
// sell_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) 
// list_stocks_fn (&$bot, $nick, $ident, $host, $dest, $text) 
// stats_stocks_fn (&$bot, $nick, $ident, $host, $dest, $text) 
// list_stocks_detail_fn (&$bot, $nick, $ident, $host, $dest, $text) 


// just internal function to get price, name and ticker of a stock (used by buy/sell functions)
// return result in an array 
// [0] - ticker
// [1] - name
// [2] - price
function get_ticker_price($ticker) {

    $ticker = urlencode($ticker);
    // $fp = fopen("google.test.txt","r");
    $fp = fopen("http://finance.google.com/finance?q=$ticker", "r");
    $result = '';

    if ($fp) {
	    while(!feof($fp)) {
	        $result .= fread($fp, 1024);
	        }
	}
    else
	{
	return;
	}

    // <div>
    // <span class=nwp>
    // Real-time:
    // &nbsp;
    // <span id="ref_22144_ltt">
    // 10:16AM EDT
    // </span>
    // </span>
    // <div class=mdata-dis>

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
    $stock_pattern='/<span class=\"pr\">.<span id=\"ref_[0-9]{2,9}_l\">([0-9.]+)<\/span>.<\/span>/s';
    if (@preg_match ($stock_pattern, $result, $stock))
           {
    // print_r($stock);
    $stock_price = $stock[1];
           }

    $stock_ticker = urldecode($stock_ticker);
    $stock_name = urldecode($stock_name);

    $stock_info[0] = $stock_ticker;
    $stock_info[1] = $stock_name;
    $stock_info[2] = $stock_price;


    // first check if we even got a price
    if ($stock_price == "") {
        return -1;
        }
    // now check if quote is not realtime (anything older than 119 seconds)
    elseif (abs($realtime_check) > 119)
        {
        return -2;
        }
    else
        {
        return ($stock_info); 
        }
}

// just internal function to get price, name and ticker of a stock (used by buy/sell functions)
// return result in an array 
// duplicated function from above, this one doest return -2 on RealTime failure 
// used in !portfolio and !market display
// [0] - ticker
// [1] - name
// [2] - price
function get_ticker_price_display($ticker) {

    $ticker = urlencode($ticker);
    // $fp = fopen("google.test.txt","r");
    $fp = fopen("http://finance.google.com/finance?q=$ticker", "r");
    $result = '';

    while(!feof($fp)) {
        $result .= fread($fp, 1024);
        }

    // <div>
    // <span class=nwp>
    // Real-time:
    // &nbsp;
    // <span id="ref_22144_ltt">
    // 10:16AM EDT
    // </span>
    // </span>
    // <div class=mdata-dis>

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
    $stock_pattern='/<span class=\"pr\">.<span id=\"ref_[0-9]{2,9}_l\">([0-9.]+)<\/span>.<\/span>/s';
    if (@preg_match ($stock_pattern, $result, $stock))
           {
    // print_r($stock);
    $stock_price = $stock[1];
           }

    $stock_ticker = urldecode($stock_ticker);
    $stock_name = urldecode($stock_name);

    $stock_info[0] = $stock_ticker;
    $stock_info[1] = $stock_name;
    $stock_info[2] = $stock_price;


    // first check if we even got a price
    if ($stock_price == "") {
        return -1;
        }
    else
        {
        return ($stock_info); 
        }
}

// get info on user by nick
// returns an array
// [0] - player id
// [1] - player cash
// -1 on error
function check_user ($nick) {
    $db_query = "SELECT * FROM stock_players WHERE player_nick='" . $nick ."' ";
    $db_send_query = @mysql_query ($db_query);
    
    if (mysql_errno()) { print  "ERROR: ".mysql_errno().":".mysql_error(); }

    if ($db_send_query > 0) {
        $user_object = mysql_fetch_object ($db_send_query);
        $user_array[0] =  $user_object->player_id;
        $user_array[1] =  $user_object->player_cash;
        print ("DEBUG : $user_array[0]  $user_array[1] \n");
        
        if ($user_array[0] == "") 
			{
            return -1;
            }
        else
            {
            return ($user_array);
            }
        }
    else
        {
        return -1;
        }

    }
    

// get info on user by nick
// returns an array
// [0] - player id
// [1] - player cash+current portfolio worth
// -1 on error
function check_user_portfolio ($nick) {

	$portfolio_nick = $nick;
        $check_user_array = check_user($portfolio_nick);
        $portfolio_worth = 0;
        $portfolio_invested = 0;

        $db_query = "SELECT SUM(stocks_owned) AS stocks_owned, stock_ticker, stock_price, b.player_nick, b.player_cash FROM stock_portfolio a INNER JOIN stock_players b ON a.nick_id=b.player_id WHERE player_nick='".$portfolio_nick."' GROUP BY a.nick_id, stock_ticker ORDER BY stocks_owned DESC";

        $db_send_query = mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                while ($row = mysql_fetch_object($db_send_query)) {
                    $stocks_owned=$row->stocks_owned;
                    $stock_ticker=$row->stock_ticker;
                    $stock_purchase_price=$row->stock_price;
                    
                    $stock_info_array = get_ticker_price ($stock_ticker);
                    $portfolio_worth = $portfolio_worth + ($stocks_owned * $stock_info_array[2]);
                    $portfolio_invested = $portfolio_invested + ($stocks_owned * $stock_purchase_price);
                    }

                mysql_free_result($db_send_query);
            }

		$net_worth = $check_user_array[1] + $portfolio_worth;
		$user_array[0] = $check_user_array[0];
		$user_array[1] = $net_worth;
		
		if ( empty($user_array[0]) ) 
			{
			return -1;
			}
		else
			{
			return ($user_array);
			}
    }


// check if player owns a stock they're buying
// returns an array
// [0] - portfolio id
// [1] - number of stocks owned 
// -1 on error
function check_ifowned($nick_id, $ticker) {
    $db_query = "SELECT * FROM stock_portfolio WHERE nick_id='".$nick_id."' AND stock_ticker='".$ticker."' ";
    $db_send_query = @mysql_query ($db_query);
    
    if (mysql_errno()) { print  "ERROR: ".mysql_errno().":".mysql_error(); }

    if ($db_send_query > 0) {
        $portfolio_object = mysql_fetch_object ($db_send_query);
        $portfolio_array[0] = $portfolio_object->portfolio_id;
        $portfolio_array[1] = $portfolio_object->stocks_owned;

        print ("DEBUG : check_ifowned()    $portfolio_array[0]   $portfolio_array[1] \n");

        return ($portfolio_array);
        }
    else
        {
        return -1;
        }
    }

// check stock user portfolio for a stock
// [0] - number of stocks owned
// [1] - stock ticker
// -1 on error
function check_stock ($nick_id, $ticker) {
    $db_query = "SELECT SUM(stocks_owned) AS stocks_owned_total, stock_ticker  FROM stock_portfolio WHERE nick_id='" . $nick_id ."' AND stock_ticker='" . $ticker . "' GROUP BY stock_ticker";
    $db_send_query = @mysql_query ($db_query);
    
    if (mysql_errno()) { print  "ERROR: ".mysql_errno().":".mysql_error(); }

    if ($db_send_query > 0) {
        $portfolio_object = mysql_fetch_object ($db_send_query);
        $portfolio_array[0] = $portfolio_object->stocks_owned_total;
        $portfolio_array[1] = $portfolio_object->stock_ticker;

        print ("DEBUG : check_stock()    $portfolio_array[0]   $portfolio_array[1] \n");

        return ($portfolio_array);
        }
    else
        {
        return -1;
        }

    }

// check time for stock market hours
// only buy/sell 9:30am - 4:00 pm
//               monday-friday
// return 0 on 'OK'
// 1 on 'fail'
function check_time () {
    $current_hour = date ("G");
    $current_minute = date ("i");
    $day_of_week = date ("w");

    if ($day_of_week > 0 && $day_of_week < 6) {
        if ($current_hour >= 9 && $current_hour <= 15) {
            if ($current_hour == 9) {
                if ($current_minute >= 30) {
                    return 0;
                    }
                else
                    {
                    return -1;
                    }
                }
            return 0;
            }
        else
            {
            return -1;
            }
        }
    else
        {
        return -1;
        }
    }


// update player's cash after buy or sell 
// returns 1 on success, -1 on failure
function update_user ($nick_id, $nick_amount, $action, $numstocks) {
    // first, get commission :)
    // 1c per stock 
    $commission_amount = abs($numstocks) * 0.01;

    $db_query = "UPDATE stock_players SET player_cash = (player_cash+".$commission_amount."), player_trades=(player_trades+1), player_stocks_traded=(player_stocks_traded+".$numstocks.") WHERE player_nick='biold' ";
    $db_send_query = mysql_query ($db_query);
    
    if ($action == "buy") {
		$nick_amount = $nick_amount + $commission_amount;
        $db_query = "UPDATE stock_players SET player_cash = (player_cash-".$nick_amount."), player_last_activitydt=now(), player_trades=(player_trades+1), player_stocks_traded=(player_stocks_traded+".$numstocks.")  WHERE player_id='" .$nick_id."' ";
        }
    elseif ($action == "short") {
		$nick_amount = $nick_amount - $commission_amount;
		$db_query = "UPDATE stock_players SET player_cash = (player_cash+".$nick_amount."), player_last_activitydt=now(), player_trades=(player_trades+1), player_stocks_traded=(player_stocks_traded+".$numstocks.")  WHERE player_id='" .$nick_id."' ";
		}
	else
        {    
        // sell 
        $nick_amount = $nick_amount - $commission_amount;
        $db_query = "UPDATE stock_players SET player_cash = (player_cash+".$nick_amount."), player_last_activitydt=now(), player_trades=(player_trades+1), player_stocks_traded=(player_stocks_traded+".$numstocks.")  WHERE player_id='".$nick_id."' ";
        }

    $db_send_query = @mysql_query ($db_query);
        
    if (mysql_errno()) { print  "ERROR: ".mysql_errno().":".mysql_error(); }
    
    if ($db_send_query > 0) {
        return 1;
        }
    else
        {
        return -1;
        }

    }

//
// check time (days) since last reset
// return 0 on OK
//        last reset date on FAIL
function check_lastreset ($nick_id) {
    // set the timeframe // this is for one week
    // 7 days; 24 hours; 60 mins; 60secs
    // im sure there is an easier sql-only alternative (well, i know there is) but it didnt 'come' to me during writing of 
    // this silly function :)
    $lastWeek = time() - (7 * 24 * 60 * 60);

    $lastWeekquery = date('Y-m-d', $lastWeek);

    $db_query = "SELECT * FROM stock_players WHERE player_id='".$nick_id."' AND player_last_resetdt <= '".$lastWeekquery."' ";
    $db_send_query = mysql_query ($db_query);
    print ("[DEBUG] : check_lastreset [$db_query] \n");
    if (mysql_num_rows($db_send_query) > 0) {
        // if we got anything at all for this user (there is only one record per user) 
        return 0;
        }
    else
        {
        // nothing returned, which means the date was not within last week
        // soooo get the date to show to user
        $db_query = "SELECT player_last_resetdt FROM stock_players WHERE player_id='".$nick_id."' ";
        $db_send_query = mysql_query ($db_query);
        $row = mysql_fetch_object ($db_send_query);
        $last_reset_dt = $row->player_last_resetdt;
        // print ("[DEBUG] check_lastreset   last_reset_dt = $last_reset_dt \n");
        
        return $last_reset_dt;
        }
    }

//
// Will return the number of days between the two dates passed in
//
function count_days( $a, $b ) {
    // First we need to break these dates into their constituent parts:
    $gd_a = getdate( $a );
    $gd_b = getdate( $b );

    print ("[DEBUG] $gd_a $gd_b \n");
     
    // Now recreate these timestamps, based upon noon on each day
    // The specific time doesn't matter but it must be the same each day
    $a_new = mktime( 12, 0, 0, $gd_a['mon'], $gd_a['mday'], $gd_a['year'] );
    $b_new = mktime( 12, 0, 0, $gd_b['mon'], $gd_b['mday'], $gd_b['year'] );
 
    // Subtract these two numbers and divide by the number of seconds in a
    // day. Round the result since crossing over a daylight savings time
    // barrier will cause this time to be off by an hour or two.
    return round( abs( $a_new - $b_new ) / 86400 );
}

// add or reset a player
// add if new, reset if exists (and remove their portfolio)
// return -1 on failure
// 
function newuser_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) {
    if (trim(strToLower(substr($text, 0, 8))) == "!newuser") {
        // set up new user
        // first check if they already exist
        // [0] - player id    [1] - player cash
        $user_info_array = check_user($nick);

        if ($user_info_array == -1) {
            $db_query = "INSERT INTO stock_players (player_nick, player_cash, player_last_activitydt, player_last_resetdt, player_host) VALUES ('".$nick."', '10000', now(), now(), '".$host."' )"; 
            $db_send_query = @mysql_query ($db_query);
            if ($db_send_query > 0) {
                $bot->privMsg ($dest, "Added new user : $nick");
                return;
                }
            else
                {
                return -1;
                }
            }
        else
            {
            $last_reset_dt = check_lastreset($user_info_array[0]);
            // print ("[DEBUG] newuser_stock_fn   last_reset_dt = $last_reset_dt \n");
            if ($last_reset_dt == 0) {
                // user already exists...reset their money and erase portfolio
                $db_query = "UPDATE stock_players SET player_cash='10000', player_last_activitydt=now(), player_last_resetdt=now() WHERE player_id='".$user_info_array[0]."' ";
                $db_send_query = @mysql_query ($db_query);
            
                $db_query = "DELETE FROM stock_portfolio WHERE nick_id = '".$user_info_array[0]."'  ";
                $db_send_query = @mysql_query ($db_query);
    
                $bot->privMsg ($dest, "Reset user : $nick");
                }
            else
                {
                $days_between = count_days (date ("Y-m-d"), $last_reset_dt);
                $bot->privMsg ($dest, "Sorry, you can reset only once every 7 days (your last reset was on $last_reset_dt [$days_between days ago])");
                }
            }
        }
    }

//
// buy stock function
// 
function buy_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) {
    if (trim(strToLower(substr($text, 0, 4))) == "!buy") {
        if (strlen(trim($text)) < 4) {
            $bot->privMsg($nick, "!buy <stock_ticker> <amount_of_stocks>");
            return;
            }

        print ("DEBUG : buy_stock_fn [dest is $dest]\n");
        if ($dest == "biold") {
            $dest = $nick;
            }

        if (check_time() == -1) {
            $bot->privMsg($dest, "Stock market closed... you cheater.");
            return;
            }
    
        $stock_txt = trim(substr($text, 5));

        $stock_quote_array = split (" ", $stock_txt);
        $stock_ticker = $stock_quote_array[0];
        $stock_amount = intval($stock_quote_array[1]);

        if (empty($stock_amount) || ($stock_amount < 0)) { 
            $bot->privMsg($dest, "umm.. buy how many ?");
            return;
            }

        print ("DEBUG : buy_stock_fn [ $stock_ticker ] [ $stock_amount ] \n");

        // get price of stock
        // [0] - ticker   [1] - stock name   [2] - stock price
        $stock_info_array = get_ticker_price ($stock_ticker);
        if ($stock_info_array == -1) {
            $bot->privMsg($dest, "Error getting current stock price :(");
            return;
            }
        if ($stock_info_array == -2) {
            $bot->privMsg($dest, "Sorry, but it seems the quote is not Real Time, unable to continue");
            return;
            }
        
        // check if whoever is trying to buy can afford it
        // [0] - player id    [1] - player cash
        $user_info_array = check_user_portfolio ($nick);
        if ($user_info_array == -1) {
            $bot->privMsg($dest, "Error:  user doesnt exist (start with !newuser ?)");
            return;
            }

        $stock_total_cost = $stock_info_array[2] * $stock_amount;

        if ($stock_total_cost > $user_info_array[1]) {
            $bot->privMsg($dest, "Error:  you dont have enough cash... (need : $".$stock_total_cost." have : $". $user_info_array[1].") ");
            return;
            }

        // buy or fail
        $check_stock_array = check_ifowned ($user_info_array[0], $stock_info_array[0]);
        if ($check_stock_array == -1 || empty($check_stock_array[0]) || empty($check_stock_array[1])) {
            // user doesnt own any stock yet
            print ("DEBUG : buy_stock  - new \n");
            $db_query = "INSERT INTO stock_portfolio(nick_id, stock_ticker, stocks_owned, stock_price, timestamp, exchange_rate ) VALUES ('".$user_info_array[0]."','".$stock_info_array[0]."', '".$stock_amount."', '".$stock_info_array[2]."', now(), '1'  ) ";
            }
        else    
            {
            // $check_stock_array[0] should have portfolio_id
            print ("DEBUG : buy_stock  - add \n");
            $db_query = "UPDATE stock_portfolio SET stocks_owned=( stocks_owned+".$stock_amount." ), stock_price='".$stock_info_array[2]."', timestamp=now() WHERE portfolio_id='".$check_stock_array[0]."' ";
            }
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            $commission_amt = abs($stock_amount) * 0.01;
            $bot->privMsg($dest, "Bought $stock_amount shares of $stock_info_array[1] ($stock_info_array[0]) for \$ $stock_total_cost total (@ \$ $stock_info_array[2])  [+ \$$commission_amt commission fee]");
            update_user ($user_info_array[0], $stock_total_cost, "buy", $stock_amount);

            // check if the stock amount is now 0 (bought back the shorted stuff ?) - delete the records
            $db_query = "DELETE FROM stock_portfolio WHERE stocks_owned = 0";
            $db_send_query = @mysql_query ($db_query);
            }
        else
            {
            $bot->privMsg($dest, "buy_stock_fn fail :(");
            }

        }
    }

//
// sell stock function
//
function sell_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) {
        if (trim(strToLower(substr($text, 0, 5))) == "!sell") {
                if (strlen(trim($text)) < 6) {
                        $bot->privMsg($nick, "!sell <stock_ticker> <amount_of_stocks>");
                        return;
                        }

        print ("DEBUG : buy_stock_fn [dest is $dest]\n");
        if ($dest == "biold") {
            $dest = $nick;
            }

        if (check_time() == -1) {
            $bot->privMsg($dest, "Stock market closed... you cheater.");
            return;
            }

        $stock_txt = trim(substr($text, 6));

        $stock_quote_array = split (" ", $stock_txt);
        $stock_ticker = $stock_quote_array[0];
        $stock_amount = intval($stock_quote_array[1]);
        
        if (empty($stock_amount) || ($stock_amount < 0)) { 
            $bot->privMsg($dest, "umm.. sell how many ?");
            return;
            }

		// [0] - ticker   [1] - stock name   [2] - stock price
		$stock_info_array = get_ticker_price ($stock_ticker);
		if ($stock_info_array == -1) {
				$bot->privMsg($dest, "Error getting current stock price :(");
				return;
				}

        if ($stock_info_array == -2) {
            $bot->privMsg($dest, "Sorry, but it seems the quote is not Real Time, unable to continue");
            return;
            }

        if (!strstr($stock_ticker, ":")) {
            print ("DEBUG : setting stock_ticker to $stock_info_array[0]\n");
            $stock_ticker = $stock_info_array[0];
            }

        // check if the user exists
        // [0] - player id    [1] - player cash
        $user_info_array = check_user ($nick);
        if ($user_info_array == -1) {
            $bot->privMsg($dest, "Error:  user doesnt exist (start with !newuser ?)");
            return;
            }

        
        // check if they have the stock they're trying to sell
        // [0] - # of stocks total   [1] - ticker   
        $portfolio_array = check_stock($user_info_array[0], $stock_ticker);
        if ($portfolio_array == -1) {
            $bot->privMsg($dest, "Error: umm... are you sure you own this stock? ");
            return;
            }

        if ($portfolio_array[0] < $stock_amount) {
            $bot->privMsg($dest, "Error: hmmm, trying to sell more than you own? ");
            return;
            }

        // subtract number of shares requested for sale from portfolio
        $total_sale_price = $stock_info_array[2] * $stock_amount;

        // add sale dollars to user
        // sell or fail

        // first lets find portfolio_id (or ids?) with sufficient stock amounts
        // since by this time we already know there are enough stocks in this users portfolio
        $db_query = "SELECT portfolio_id FROM stock_portfolio WHERE stocks_owned >= '". $stock_amount ."' AND nick_id='" . $user_info_array[0] . "' AND stock_ticker='".$stock_info_array[0]."' ";
        $db_send_query = @mysql_query ($db_query);
        $portfolio_array_object = mysql_fetch_object ($db_send_query);
        $portfolio_array[2] = $portfolio_array_object->portfolio_id;
        

        $db_query = "UPDATE stock_portfolio SET stocks_owned=(stocks_owned-".$stock_amount."), timestamp=now() WHERE portfolio_id='".$portfolio_array[2]."'  ";
        print ("DEBUG : sell_stock_fn   [$db_query] \n");
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            $commission_amt = abs($stock_amount) * 0.01;
            $bot->privMsg($dest, "Sold $stock_amount shares of $stock_info_array[1] ($stock_info_array[0]) for \$ $total_sale_price (@ \$ $stock_info_array[2]) [+ \$$commission_amt commission fee]");
            update_user ($user_info_array[0], $total_sale_price, "sell", $stock_amount);
            // check if the stock amount is now 0 (all sold?) - delete the record
            $db_query = "DELETE FROM stock_portfolio WHERE stocks_owned = 0";
            $db_send_query = @mysql_query ($db_query);
            }
        else
            {
            $bot->privMsg($dest, "sell_stock_fn fail :(");
            }

        }
    }
    
//
// short stock function
//
function short_stock_fn (&$bot, $nick, $ident, $host, $dest, $text) {
        if (trim(strToLower(substr($text, 0, 6))) == "!short") {
                if (strlen(trim($text)) < 7) {
                        $bot->privMsg($nick, "!short <stock_ticker> <amount_of_stocks>");
                        return;
                        }

        print ("DEBUG : short_stock_fn [dest is $dest]\n");
        if ($dest == "biold") {
            $dest = $nick;
            }

        if (check_time() == -1) {
            $bot->privMsg($dest, "Stock market closed... you cheater.");
            return;
            }

        $stock_txt = trim(substr($text, 6));

        $stock_quote_array = split (" ", $stock_txt);
        $stock_ticker = $stock_quote_array[0];
        $stock_amount = intval($stock_quote_array[1]);
        
        if (empty($stock_amount) || ($stock_amount < 0)) { 
            $bot->privMsg($dest, "umm.. short how many ?");
            return;
            }

		// [0] - ticker   [1] - stock name   [2] - stock price
		$stock_info_array = get_ticker_price ($stock_ticker);
		if ($stock_info_array == -1) {
				$bot->privMsg($dest, "Error getting current stock price :(");
				return;
				}

        if ($stock_info_array == -2) {
            $bot->privMsg($dest, "Sorry, but it seems the quote is not Real Time, unable to continue");
            return;
            }

        if (!strstr($stock_ticker, ":")) {
            print ("DEBUG : setting stock_ticker to $stock_info_array[0]\n");
            $stock_ticker = $stock_info_array[0];
            }

        // check if the user exists
        // [0] - player id    [1] - player cash
        $user_info_array = check_user ($nick);
        if ($user_info_array == -1) {
            $bot->privMsg($dest, "Error:  user doesnt exist (start with !newuser ?)");
            return;
            }

        // check if whoever is trying to buy can afford it
        // have to total all stocks + cash  (vs just cash in a buy)
        // [0] - player id    [1] - player cash+current stock portfolio worth
        $user_info_array = check_user_portfolio ($nick);
        if ($user_info_array == -1) {
            $bot->privMsg($dest, "Error:  user doesnt exist (start with !newuser ?)");
            return;
            }

        $stock_total_cost = $stock_info_array[2] * $stock_amount;

        if ($stock_total_cost > $user_info_array[1]) {
            $bot->privMsg($dest, "Error:  you dont have enough cash... (need : $".$stock_total_cost." portfolio worth : $". $user_info_array[1].") ");
            return;
            }

        // short or fail
        $check_stock_array = check_ifowned ($user_info_array[0], $stock_info_array[0]);
        if ($check_stock_array == -1 || empty($check_stock_array[0]) || empty($check_stock_array[1])) {
            // user doesnt own any stock yet
            print ("DEBUG : short_stock  - new \n");
            $db_query = "INSERT INTO stock_portfolio(nick_id, stock_ticker, stocks_owned, stock_price, timestamp, exchange_rate ) VALUES ('".$user_info_array[0]."','".$stock_info_array[0]."', '-".$stock_amount."', '".$stock_info_array[2]."', now(), '1'  ) ";
            }
        else    
            {
            // $check_stock_array[0] should have portfolio_id
            print ("DEBUG : buy_stock  - add \n");
            $db_query = "UPDATE stock_portfolio SET stocks_owned=( stocks_owned-".$stock_amount." ), stock_price='".$stock_info_array[2]."', timestamp=now() WHERE portfolio_id='".$check_stock_array[0]."' ";
            }
        $db_send_query = @mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            $commission_amt = abs($stock_amount) * 0.01;
            $bot->privMsg($dest, "Shorted $stock_amount shares of $stock_info_array[1] ($stock_info_array[0]) for \$ $stock_total_cost total (@ \$ $stock_info_array[2])  [+ \$$commission_amt commission fee]");
            update_user ($user_info_array[0], $stock_total_cost, "short", $stock_amount);
            }
        else
            {
            $bot->privMsg($dest, "short_stock_fn fail :(");
            }

        }
    }


// 
// simple tally of the user portfolio
// list cash and stocks of a player in short format
// 
function list_stocks_fn (&$bot, $nick, $ident, $host, $dest, $text) {
    if (trim(strToLower(substr($text, 0, 10))) == "!portfolio") {

        $portfolio_nick = trim(substr($text, 11));

        if (empty($portfolio_nick)) {
            $portfolio_nick=$nick;
            }

        print ("DEBUG : buy_stock_fn [dest is $dest]\n");
        if ($dest == "biold") {
            $dest = $nick;
            }
    

        print ("DEBUG : list_stocks_fn ($portfolio_nick) \n");

        $check_user_array = check_user($portfolio_nick);
	// $check_user_portfolio_array = check_user_portfolio($portfolio_nick);
        $portfolio_worth = 0;
        $portfolio_invested = 0;

        $db_query = "SELECT SUM(stocks_owned) AS stocks_owned, stock_ticker, stock_price, b.player_nick, b.player_cash FROM stock_portfolio a INNER JOIN stock_players b ON a.nick_id=b.player_id WHERE player_nick='".$portfolio_nick."' GROUP BY a.nick_id, stock_ticker ORDER BY stocks_owned DESC";

        $db_send_query = mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                while ($row = mysql_fetch_object($db_send_query)) {
                    $stocks_owned=$row->stocks_owned;
                    $stock_ticker=$row->stock_ticker;
                    $stock_purchase_price=$row->stock_price;
                    
                    $stock_info_array = get_ticker_price_display ($stock_ticker);
                    $portfolio_worth = $portfolio_worth + ($stocks_owned * $stock_info_array[2]);
                    $portfolio_invested = $portfolio_invested + ($stocks_owned * $stock_purchase_price);
                    
                    $portfolio_txt = $portfolio_txt . $stock_ticker . " X " . $stocks_owned . " | ";
                    }

                $portfolio_txt = $portfolio_nick . "'s portfolio : $" . $check_user_array[1] . " (cash), STOCKS : " . $portfolio_txt;

                mysql_free_result($db_send_query);
                $bot->privMsg($dest, $portfolio_txt);
                if ($portfolio_invested != 0) {
                    $net_worth = $check_user_array[1] + $portfolio_worth;
                    $portfolio_worth_txt = "Total spent on stocks : \$ $portfolio_invested  | Total current stock worth : \$ $portfolio_worth | (Cash + Portfolio) \$ $net_worth";
                    $bot->privMsg($dest, $portfolio_worth_txt);
                    }
            }
        }
    }

//
// function used to sort the 'top 3' for !market stats
// 
function sortByInteger(&$personDetails, $field) {
    $sort = "return -strnatcmp(\$p1['$field'], \$p2['$field']);";
    usort($personDetails, create_function('$p1,$p2', $sort));
    return $personDetails;
    }

// 
// produce top 3 type total tally of all players portfolios and cash
// sort of awkward, but have to look up stock price of each stock owned, lots of lookups :(
// dont know of any other way.
//
function stats_stocks_fn (&$bot, $nick, $ident, $host, $dest, $text) {
    if (trim(strToLower(substr($text, 0, 7))) == "!market") {

        print ("DEBUG : stats_stocks_fn [dest is $dest]\n");
        if ($dest == "biold") {
            $dest = $nick;
            }
        // find top 3 players ? - total worth
        $bot->privMsg($dest, "Calculating Totals ....");

        // loop through all portfolios grouped by player, add em up
        $db_query = "SELECT player_id FROM stock_players WHERE player_nick <> 'biold'";
        $db_send_query = mysql_query ($db_query);

        $user_array_count = 0;

        if ($db_send_query > 0) {
            while ($row = mysql_fetch_object ($db_send_query)) {
                // for each player_id with stocks get their total portfolio
                $player_id = $row->player_id;

                // rewrote the below to count the people that down own stocks :)
                // $db_portfolio_query = "SELECT stocks_owned, stock_ticker, stock_price, b.player_nick, b.player_cash FROM stock_portfolio a INNER JOIN stock_players b ON a.nick_id=b.player_id WHERE nick_id='".$player_id."' ";

                $db_portfolio_query = "SELECT b.stocks_owned, b.stock_ticker, b.stock_price, a.player_nick, a.player_cash FROM stock_players a LEFT OUTER JOIN stock_portfolio b ON a.player_id = b.nick_id WHERE a.player_id = '".$player_id."' ";

                $db_send_portfolio_query = mysql_query ($db_portfolio_query);

                // reset user's portfolio
                $portfolio_worth = 0;

                if ($db_send_portfolio_query > 0) {
                    while ($row_portfolio = mysql_fetch_object($db_send_portfolio_query)) {
                        $stocks_owned = $row_portfolio->stocks_owned;
                        $stock_ticker = $row_portfolio->stock_ticker;
                        $stock_purchase_price = $row_portfolio->stock_price;
                        $players_cash = $row_portfolio->player_cash;
                        $players_nick = $row_portfolio->player_nick;

                        // get current price for each ticker :/
                        $stock_info_array = get_ticker_price_display ($stock_ticker);
                        $portfolio_worth = $portfolio_worth + ($stocks_owned * $stock_info_array[2]);
                        }
                    }
                $portfolio_worth = $portfolio_worth + $players_cash;

                $portfolio_array[$user_array_count]['nick'] = $players_nick;
                $portfolio_array[$user_array_count]['worth'] = $portfolio_worth;

                $user_array_count++;
                }
            }
        print_r ($portfolio_array);
        print ("Sorting \n");
        $portfolio_array = sortByInteger($portfolio_array, 'worth');
        print_r ($portfolio_array);

        $stock_market_txt = "[TOP 3 Stock Players]   #1 ".$portfolio_array[0]['nick'] ." $".$portfolio_array[0]['worth']." ||  #2 ".$portfolio_array[1]['nick'] ." $".$portfolio_array[1]['worth']."  ||  #3 ".$portfolio_array[2]['nick'] ." $".$portfolio_array[2]['worth'];

        $bot->privMsg($dest, $stock_market_txt);
        }
    }

// 
// simple tally of the user portfolio
// list cash and stocks of a player in long / detail format
// 
function list_stocks_detail_fn (&$bot, $nick, $ident, $host, $dest, $text) {
    if (trim(strToLower(substr($text, 0, 7))) == "!stocks") {

        $portfolio_nick = trim(substr($text, 8));

        if (empty($portfolio_nick)) {
            $portfolio_nick=$nick;
            }

        // always send to user (too much spam for channel)
        $dest = $nick;

        print ("DEBUG : list_stocks_detail_fn ($portfolio_nick) \n");

        $check_user_array = check_user($portfolio_nick);
        $portfolio_worth = 0;
        $portfolio_invested = 0;

        $bot->privMsg($dest, "Rate-Limiting display to 1 msg / second ...");

        // this needs to change to more suited sql
        // it is this way because portfolios used to contain each instance of purchased stock (now its just one per)
        // sooo, no need to sum/group by but... i dont feel like thinking too hard atm :)
        //
        $db_query = "SELECT SUM(stocks_owned) AS stocks_owned, stock_ticker, stock_price, b.player_nick, b.player_cash, b.player_trades, b.player_stocks_traded FROM stock_portfolio a INNER JOIN stock_players b ON a.nick_id=b.player_id WHERE player_nick='".$portfolio_nick."' GROUP BY a.nick_id, stock_ticker ORDER BY stocks_owned DESC";

        $db_send_query = mysql_query ($db_query);
        if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }

        if ($db_send_query > 0) {
            if (mysql_errno()) { $bot->privMsg($dest, "ERROR: ".mysql_errno().":".mysql_error()); }
                while ($row = mysql_fetch_object($db_send_query)) {
                    $stocks_owned=$row->stocks_owned;
                    $stock_ticker=$row->stock_ticker;
                    $stock_purchase_price=$row->stock_price;
                    $player_trades=$row->player_trades;
                    $player_stocks_traded=$row->player_stocks_traded;
                    
                    $stock_info_array = get_ticker_price ($stock_ticker);
                    $current_stock_worth = $stocks_owned * $stock_info_array[2];
                    $past_stock_worth = $stocks_owned * $stock_purchase_price;
                    $portfolio_worth = $portfolio_worth + ($stocks_owned * $stock_info_array[2]);
                    $portfolio_invested = $portfolio_invested + ($stocks_owned * $stock_purchase_price);
                    
                    // $portfolio_txt = $portfolio_txt . $stock_ticker . " X " . $stocks_owned . " | ";
                    $bot->privMsg($dest, "[ " . $stock_ticker . " X " . $stocks_owned . " ][ Bought @ \$" . $stock_purchase_price . " ( Total \$".$past_stock_worth." ) ][ Currently @ \$" . $stock_info_array[2] . " ( Total \$" . $current_stock_worth . " ) ] ");
                    sleep (1); // delay the msg spam so we dont get killed by server 
                    }

                mysql_free_result($db_send_query);

                if ($portfolio_invested > 0) {
                    $net_worth = $check_user_array[1] + $portfolio_worth;
                    $portfolio_worth_txt = "Total spent on stocks : \$ $portfolio_invested  | Total current stock worth : \$ $portfolio_worth | (Cash + Portfolio) : \$ $net_worth | Your total trades : $player_trades | Total stocks traded : $player_stocks_traded";
                    $bot->privMsg($dest, $portfolio_worth_txt);
                    }
            }
        }
    }
?>
