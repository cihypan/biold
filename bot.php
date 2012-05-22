#!/usr/local/bin/php4 -q
<?php
include("lib/ircBot.class.php");
include("lib/class.RSS.php");
include("lib/class.table-extractor.php");

include("db.php");
include("google.php");
include("translate/google.translate.class.php");

// php4 lacking new shit
include ("Services/JSON.php");

if ( !function_exists('json_decode') ){
    function json_decode($content, $assoc=false){
                require_once 'Services/JSON.php';
                if ( $assoc ){
                    $json = new Services_JSON(SERVICES_JSON_LOOSE_TYPE);
        } else {
                    $json = new Services_JSON;
                }
        return $json->decode($content);
    }
}

if ( !function_exists('json_encode') ){
    function json_encode($content){
                require_once 'Services/JSON.php';
                $json = new Services_JSON;
               
        return $json->encode($content);
    }
}

include("currency/currency.xe.class.php");
include("eliza/eliza.php");
include("temperature/temp.php");
include("horoscope/horoscope.php");
include("weather/xoapWeather.php");
include("sanitize.inc.php");
include("numbers2words.php");
include("include/time.inc");
include("dictionary/dict.inc");
// old imdb shit
// include("include/imdb.functions.inc");
include ("imdb/class.imdb_parser.php");
include ("imdb/class.imdb_search.php");

// this below class is 'ok' but breaks on a lot of xml formats
include("wiki/xml_to_array.class.php");
// this class works fairly well
include("wiki/xml_parser.php");

// alice is not in kansas anymore
// include ("alice/programe/src/irc_alice.php");

// include("geoip/geoip.class.php");

// used to fake IE or mozilla.. for some sites.
// in particular : get url title function
include("browser_emulator/browseremulator.class.php");

include ("include/core_functions.inc");
include ("include/nslookup.inc");

include ("include/trivia.inc");
include ("include/oldurl.inc");
include ("include/weather.inc");
include ("include/stocks.inc");
include ("include/stock_market.inc");
include ("include/ecn.inc");
include ("include/currency.inc");
include ("include/translate.inc");
include ("include/horoscope.inc");
include ("include/google.inc");
include ("include/eliza.inc");
// include ("include/alice.inc");
include ("include/grok.inc");
include ("include/temperature.inc");
include ("include/riddle.inc");
include ("include/stfu.inc");
include ("include/gnews.inc");
include ("include/dict.inc");
include ("include/quotes.inc");
include ("include/imdb.inc");
include ("include/wiki.inc");
include ("include/karma.inc");
// include ("include/geoip.inc");
include ("include/8ball.inc");

/**  Configuration  */

ircConf::set("botNick", "bot_nick");       /** Bot nick */
ircConf::set("botName", "bot_name");  /** Bot's IRC name */
ircConf::set("botIdent", "bot_ident");      /** Bot's ident */

ircConf::set("channel", "#channel_to_join"); /** Channel to join */
ircConf::set("channelMode", "+nt"); /** Channel modes */

/** NickServ password, comment out if you don't have one.
  NOTE: this is also used when the bot NickName is taken to try ghost it
  with NickServ, only when set */
//ircConf::set("botNickServPass", "somePass");

/** Servers to connect to - MUST BE ARRAY()!!! Used for rotation */
//ircConf::set("servers", array("server1.network.com", "server2.network.com"));
ircConf::set("servers", array("irc.server.com","irc.server2.com","irc.server3.net"));
ircConf::set("serverPort", 6667); /** Server port */
ircConf::set("stayAlive", true); /** Stay-Alive - Keep trying connections */

/** Some networks and people don't like bots, especially not yewnix ones ;) */
//ircConf::set("versionReply", "mIRC v6.03 Khaled Mardam-Bey");
//ircConf::set("versionReply", ircBotVersion);
ircConf::set("versionReply", "This is the version, and stuff");

/** Bot user configuration */
ircConf::setUser("admin1", "admin1", "some.domain.com",   "admin");
ircConf::setUser("admin2", "admin2", "some.domain2.com",   "admin");
// ircConf::setUser("someNick", "ident", "*.somehost.com", "pleb");

/** Log/Debug, when TRUE messages will written to the logfile */
define("ircDoLog", true);
define("ircDebug", true);

// array used for keeping score 
$score_arr = array();
$answered_trivia_arr = array();
$answered_flag = 0;
$start_time = getmicrotime();
$similar_answer_global = 0;

$irc = new ircBot;

/** ADD CALLBACKS
 * This is where the bot gets its functionality.
 * See comments & code below for writing callback functions */
// $irc->addCallback(onNotice,    "identifyNickServ");
// $irc->addCallback(onJoin,      "greetOnJoin");
// $irc->addCallback(onTopic,     "updateTopic");

$irc->addCallback(onChanMsg,   "toolNsLookup");
// $irc->addCallback(onChanMsg,   "ip_fn");
$irc->addCallback(onChanMsg,   "google_fn");
$irc->addCallback(onChanMsg,   "google_calc_fn");
$irc->addCallback(onChanMsg,   "ticker_fn");
$irc->addCallback(onChanMsg,   "oil_fn");
$irc->addCallback(onChanMsg,   "ecn_fn");
$irc->addCallback(onChanMsg,   "do_not_old_fn");
$irc->addCallback(onChanMsg,   "old_fn");
$irc->addCallback(onChanMsg,   "old_status_fn");
$irc->addCallback(onChanMsg,   "old_release_fn");
$irc->addCallback(onChanMsg,   "translate_fn");
$irc->addCallback(onChanMsg,   "currency_fn");
$irc->addCallback(onChanMsg,   "weather_fn");
$irc->addCallback(onChanMsg,   "fweather_fn");
$irc->addCallback(onChanMsg,   "forecast_fn");
$irc->addCallback(onChanMsg,   "fforecast_fn");
$irc->addCallback(onChanMsg,   "help_fn");
$irc->addCallback(onChanMsg,   "grok_horoscope_fn");
$irc->addCallback(onChanMsg,   "eliza_fn");
// $irc->addCallback(onChanMsg,   "alice_fn");
$irc->addCallback(onChanMsg,   "ctof_fn");
$irc->addCallback(onChanMsg,   "ftoc_fn");
$irc->addCallback(onChanMsg,   "last_fn");
$irc->addCallback(onChanMsg,   "nlast_fn");
$irc->addCallback(onChanMsg,   "search_fn"); 
$irc->addCallback(onChanMsg,   "grok_urban_fn"); 
$irc->addCallback(onChanMsg,   "trivia_fn"); 
$irc->addCallback(onChanMsg,   "get_riddle"); 
$irc->addCallback(onChanMsg,   "get_stfu"); 
$irc->addCallback(onChanMsg,   "gnews_fn");
$irc->addCallback(onChanMsg,   "gnews_url_fn");
$irc->addCallback(onChanMsg,   "dict_lookup_fn");
$irc->addCallback(onChanMsg,   "thesaurus_lookup_fn");
$irc->addCallback(onChanMsg,   "addquote_fn");
$irc->addCallback(onChanMsg,   "randomquote_fn");
$irc->addCallback(onChanMsg,   "topquotes_fn");
$irc->addCallback(onChanMsg,   "imdb_lookup_fn");
$irc->addCallback(onChanMsg,   "imdb_search_fn");
$irc->addCallback(onChanMsg,   "wiki_fn");
$irc->addCallback(onChanMsg,   "karma_fn");
$irc->addCallback(onChanMsg,   "karma_stats_fn");
$irc->addCallback(onChanMsg,   "seen_fn");
$irc->addCallback(onChanMsg,   "lastSeen_msg");
$irc->addCallback(onChanMsg,   "buy_stock_fn");
$irc->addCallback(onChanMsg,   "sell_stock_fn");
$irc->addCallback(onChanMsg,   "short_stock_fn");
$irc->addCallback(onChanMsg,   "list_stocks_fn");
$irc->addCallback(onChanMsg,   "newuser_stock_fn");
$irc->addCallback(onChanMsg,   "stats_stocks_fn");
$irc->addCallback(onChanMsg,   "ball_fn");

$irc->addCallback(onPrivMsg,   "adminControl");
$irc->addCallback(onPrivMsg,   "google_fn_msg");
$irc->addCallback(onPrivMsg,   "ticker_fn_msg");
$irc->addCallback(onPrivMsg,   "ecn_fn_msg");
$irc->addCallback(onPrivMsg,   "translate_fn_msg");
$irc->addCallback(onPrivMsg,   "weather_fn_msg");
$irc->addCallback(onPrivMsg,   "fweather_fn_msg");
$irc->addCallback(onPrivMsg,   "eliza_fn_msg");
$irc->addCallback(onPrivMsg,   "help_fn_msg");
$irc->addCallback(onPrivMsg,   "forecast_fn_msg");
$irc->addCallback(onPrivMsg,   "horoscope_fn_msg");
$irc->addCallback(onPrivMsg,   "last_fn");
$irc->addCallback(onPrivMsg,   "nlast_fn");
$irc->addCallback(onPrivMsg,   "search_fn");
$irc->addCallback(onPrivMsg,   "trivia_check_fn");
$irc->addCallback(onPrivMsg,   "ticker_lookup_fn");
$irc->addCallback(onPrivMsg,   "gnews_fn");
$irc->addCallback(onPrivMsg,   "gnews_url_fn");
$irc->addCallback(onPrivMsg,   "buy_stock_fn");
$irc->addCallback(onPrivMsg,   "sell_stock_fn");
$irc->addCallback(onPrivMsg,   "short_stock_fn");
$irc->addCallback(onPrivMsg,   "stats_stocks_fn");
$irc->addCallback(onPrivMsg,   "list_stocks_fn");
$irc->addCallback(onPrivMsg,   "list_stocks_detail_fn");

// $irc->addCallback(onOp,        "secureOps");
// $irc->addCallback(onMode,      "checkMode");

// multiple functions/events can be declared
$irc->addCallback(onJoin,      "lastSeen");
$irc->addCallback(onKick,      "lastSeen");
$irc->addCallback(onPart,      "lastSeen");
$irc->addCallback(onQuit,      "lastSeen");
$irc->addCallback(onKill,      "lastSeen");

/** Connect to IRC server */
$irc->init();
/****************************************************************************
 *                     CALLBACK FUNCTIONS
 ****************************************************************************
 * Different arguments are passed to the callback function on
 * each event, argument 0 is ALWAYS a reference to the ircBot object which
 * allows us to access the bot methods.
 * Arguments 1/2/3 are typically the users nick/ident/host.
 *
 * To add a callback, simply call addCallback(event, "yourCallbackFunction")
 *
 * ---------------------------- Message events ---------------------------
 *
 * _onPrivMsg(&$bot, $nick, $ident, $host, $dest, $text)
 * onChanMsg(&$bot, $nick, $ident, $host, $channel, $text)
 *  onNotice(&$bot, $nick, $ident, $host, $dest, $text)
 *
 * ---------------------------- Channel events ---------------------------
 *
 *  onJoin(&$bot, $nick, $ident, $host, $channel)
 *  onPart(&$bot, $nick, $ident, $host, $channel)
 *  onKick(&$bot, $nick, $ident, $host, $channel, $victim, $reason)
 *  onTopic(&$bot, $nick, $ident, $host, $channel, $topic)
 *  onQuit(&$bot, $nick, $ident, $host, $quitMsg)
 *  onKill(&$bot, $nick, $ident, $host, $victim, $text)
 *
 * ---------------------------- Mode events ------------------------------
 *
 *  onOp/onDeOp(&$bot, $nick, $ident, $host, $channel, $user)
 *  onVoice/onDeVoice(&$bot, $nick, $ident, $channel, $user)
 *  onBan/onUnBan(&$bot, $nick, $ident, $host, $channel, $user)
 *
 *  Other modes are passed to onMode:
 *  onMode(&$bot, $nick, $ident, $host, $channel, $data)
 *
 ****************************************************************************/

?>
