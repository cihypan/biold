<?php
/* $Id$
   ------------------------------------------------------------------
 | ircBot.class.php                                                   |
 | ------------------------------------------------------------------ |
 | Copyright (C) 2003 Donovan Schönknecht <ds@undesigned.org.za>      |
 | ------------------------------------------------------------------ |
 | Subtility - Core framework for IRC connections and event handling  |
 | ------------------------------------------------------------------ |
 | This program is free software; you can redistribute it and/or      |
 | modify it under the terms of the GNU General Public License as     |
 | published by the Free Software Foundation; either version 2 of     |
 | the License, or (at your option) any later version.                |
 |                                                                    |
 | This program is distributed in the hope that it will be useful,    |
 | but WITHOUT ANY WARRANTY; without even the implied warranty of     |
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the      |
 | GNU General Public License for more details.                       |
 |                                                                    |
 | You should have received a copy of the GNU General Public License  |
 | along with this program; if not, write to the Free Software        |
 | Foundation, Inc., 59 Temple Place, Suite 330, Boston,              |
 | MA 02111-1307  USA                                                 |
   ------------------------------------------------------------------ */

/* ------------------------------------------------------------------
 | TODO                                                               |
 | ------------------------------------------------------------------ |
 | - Add onInvite event                                               |
   ------------------------------------------------------------------ */

/* ------------------------------------------------------------------
 | CHANGELOG                                                          |
 | ------------------------------------------------------------------ |
 | 10th/Sep/2003                                                      |
 |  - Now setting stream blocking and write buffer                    |
 |  - Now uses serverPort configuration variable (sorry)              |
 |  - ircConf::isUser() now validates wildcard domains                |
 | 1st/Sep/2003                                                       |
 |  - Fixed stayAlive and ircConnect::_rotateServers();               |
 |  - Added mode handling for:                                        |
 |    onOp/onDeop, onVoice/onDevoice, onBan/onUnBan                   |
 | 31st/Aug/2003                                                      |
 |  - Initial public development release                              |
   ------------------------------------------------------------------ */

define("ircBotVersion", "biosubtility");

/** Callback events */
define("onPing",      1000);

define("onChanMsg",   1100);
define("onPrivMsg",   1101);
define("onNotice",    1102);

define("onJoin",      1200);
define("onPart",      1201);
define("onKick",      1202);
define("onKill",      1203);

define("onTopic",     1300);
define("onQuit",      1301);
define("onVersion",   1302);

define("onMode",      1400);
define("onOp",        1401);
define("onDeOp",      1402);
define("onVoice",     1403);
define("onDeVoice",   1404);
define("onBan",       1405);
define("onUnBan",     1406);

include("ircConf.class.php");

/** Parent bot methods, handles socket and log streams */
class ircConnect {
  var $log;
  var $conn;
  var $servers;
  var $lastServer;
  var $currentServer;
  var $retryConnections;
  var $retryCount = 0;
  

  function _connect() {
    $this->_rotateServers();
    $port = ircConf::get("serverPort");
    $this->log("Connecting to $this->currentServer:$port (#".($this->retryCount + 1).")");
    if (!isset($this->conn) || !@is_resource($this->conn)) {
      if ($this->conn = @fSockOpen($this->currentServer,
      $port, $error, $errMsg, 120)) {
        /* For fGets() - wait for data to become available to stream */
        stream_set_blocking($this->conn, true);
        stream_set_timeout($this->conn, 120);
        stream_set_write_buffer($this->conn, 0);
        $this->log("Connected to $this->currentServer:$port");
        return true;
      }
    }
    $this->retryCount++;
    $this->log("Can't connect to $this->currentServer:$port ($error): $errMsg");
    sleep(2); /* Retry every 2 seconds (so we don't throttle) */
    return false;
  }
  
  function _rotateServers() {
    if (!isset($this->servers)) {
      $this->servers = ircConf::get("servers");
      $this->currentServer = $this->servers[0];
      $this->lastServer = 0;
      return;
    }
    if (isset($this->servers[($this->lastServer + 1)])) {
      $this->currentServer = $this->servers[($this->lastServer + 1)];
      $this->lastServer++;
    } else {
      $this->currentServer = $this->servers[0];
      $this->lastServer = 0;
    }
  }

  function write($string) {
    fPuts($this->conn, $string . "\n");
  }

  function &read($length = 1024) {
    if (($data = @fGets($this->conn, $length)) !== false) {
      if (strlen($data) <= 0) return false; /** ignore empty data */
      $data = trim($data);
      return $data;
    }
    return false;
  }

  function close($quitMsg = "*wave*") {
    $this->log("Shutting down...");
    $this->write("QUIT :$quitMsg");
    fClose($this->conn);
    fClose($this->log);
  }

  function log($str) {
    if (ircDebug || ircDoLog) {
      if (!isset($this->log)) { /* open log file if not open */
        $logDir = str_replace("\\", "/", substr(dirName(__FILE__), 0, -3)."/log");
        if (!file_exists($logDir)) @mkDir($logDir);
        $this->log = fOpen($logDir."/subtility.log", "wb");
      }
      fWrite($this->log, sprintf("[%s] %s\n", date("D jS M H:i:s"), $str));
    }
  }

}

/** Extends ircConnect, main bot functionality */
class ircBot extends ircConnect {

  var $callbacks = array();

  function addCallback($event, $method) {
    if (!isset($this->callbacks[$event])) $this->callbacks[$event] = array();
    $this->callbacks[$event][] = $method;
  }

  function callback($code, &$args) {
    if (isset($this->callbacks[$code]) && is_array($this->callbacks[$code])) {
      $args[0] = $this; /* Return the bot object to callback */
      foreach ($this->callbacks[$code] as $callback) {
        call_user_func_array($callback, $args);
      }
    }
  }

  function init() {
    set_time_limit(0);
    ob_implicit_flush();
    $this->retryConnections = false;
    do {
      if ($this->_connect()) {
        $this->write("NICK ".ircConf::get("botNick"));
        $this->write("USER ".ircConf::get("botIdent")." 2 3 :".ircConf::get("botName"));
	$this->write("JOIN ".ircConf::get("channel2"));
	$this->write("JOIN ".ircConf::get("channel3"));
	$this->write("JOIN ".ircConf::get("channel4"));
        $this->write("JOIN ".ircConf::get("channel"));
        if (($modeLock = ircConf::get("channelMode")) !== false) {
          $this->write("MODE ".ircConf::get("channel")." ".$modeLock);
        } unset($modeLock);
        do {
          while (($buffer =& $this->read()) !== false) {
              if (ircDebug) $this->log($buffer);

              /** Handle events in order of their estimated usage desc */

              /** Privmsgs */
              if (ereg("^:(.+)!(.+)@(.+) PRIVMSG (.+) :(.*)", $buffer, $data)) {

                /** onChanMsg */
                if ($data[4][0] == "#") {
                  $this->callback(onChanMsg, $data); break;
                } elseif ($data[4] == ircConf::get("botNick")) {

                  /** CTCP versions */
                  if ($data[5] == chr(1)."VERSION".chr(1)) {
                    $this->ctcpNotice($data[1], "VERSION", ircConf::get("versionReply"));
                    break;

                  /** CTCP Times */
                  } elseif ($data[5] == chr(1)."TIME".chr(1)) {
                    $this->ctcpNotice($data[1], "TIME", date("D M j H:i:s Y"));
                    break;
                    
                  /** CTCP Fingers, reply random data..
                      (could also add a timer to ircConnect::write() */
                  } elseif ($data[5] == chr(1)."FINGER".chr(1)) {
                    $this->ctcpNotice($data[1], "FINGER",
                    ircConf::get("botName")." (".ircConf::get("botIdent").") ".
                    "Idle " . mt_rand(1, 5) . " seconds");
                    break;
                    
                  /** CTCP Pings */
                  } elseif (substr($data[5], 0, 5) == chr(1)."PING") {
                    $this->ctcpNotice($data[1], "PING", substr($data[5], 6, -1));
                    break;

                  /** onPrivMsg - handle all messages directed at us */
                  } else {
                    $this->callback(onPrivMsg, $data);
                    break;
                  }

                }
                
              /** PING? PONG! :) */
              } elseif (ereg("^PING :(.+)", $buffer, $data)) {
                $this->write("PONG " . $data[1]);
                break;

              /** Modes */
              } elseif (ereg("^:(.+)!(.+)@(.+) MODE ([a-zA-Z0-9#_\\\^\{\}\|_-]+) (.*)", $buffer, $data)) {
                $modes = array();
                $modeData = explode(" ", $data[5]);
                $modePos = 1;
                $size = strlen($modeData[0]);
                /** Shift through the raw mode data, collect our neccessary
                    data required for simple and complex modes */
                for ($i = 0; $i < $size; $i++) {
                  if ($modeData[0][$i] == "+" || $modeData[0][$i] == "-") {
                    $lastSwitch = $modeData[0][$i];
                  } else {
                    if (in_array($modeData[0][$i], array("n", "t", "s", "R", "p", "m", "i"))) {
                      $modes[] = array("type" => "simple", "switch" => $lastSwitch,
                      "mode" => $modeData[0][$i], "data" => $lastSwitch . $modeData[0][$i]);
                      /** Obviously some more mode handling needs to be done for
                          the l/k modes */
                    } elseif (in_array($modeData[0][$i], array("o", "v", "b", "l", "k"))) {
                      $modes[] = array("type" => "complex", "switch" => $lastSwitch,
                      "mode" => $modeData[0][$i], "data" => $modeData[$modePos]);
                      $modePos++; /** Mode data position needs to be shifted */
                    }
                  }
                }
                $this->_handleModes($modes, $data);
                unset($modes, $modeData, $size, $lastSwitch, $modePos);
                break;

              /** Topics */
              } elseif (ereg("^:(.+)!(.+)@(.+) TOPIC (.+) :(.*)", $buffer, $data)) {
                $this->callback(onTopic, $data); break;

              /** Notices */
              } elseif (ereg("^:(.+)!(.+)@(.+) NOTICE (.+) :(.*)", $buffer, $data)) {
                $this->callback(onNotice, $data);
                break;

              /** Channel parts */
              } elseif (ereg("^:(.+)!(.+)@(.+) PART (.+)", $buffer, $data)) {
                $this->callback(onPart, $data);
                break;

              /** Channel joins */
              } elseif (ereg("^:(.+)!(.+)@(.+) JOIN :(.+)", $buffer, $data)) {
                $this->callback(onJoin, $data);
                break;

              /** Channel kicks */
              } elseif (ereg("^:(.+)!(.+)@(.+) KICK (.+) (.+) :(.*)", $buffer, $data)) {
                if ($data[5] == ircConf::get("botNick")) { /* we've been kicked, rejoin */
                  $this->write("JOIN ".ircConf::get("channel"));
                  if (($modeLock = ircConf::get("channelMode")) !== false) {
                    $this->write("MODE ".ircConf::get("channel")." ".$modeLock);
                  }
                }
                $this->callback(onKick, $data);
                break;

              /** Quit messages */
              } elseif (ereg("^:(.+)!(.+)@(.+) QUIT :(.*)", $buffer, $data)) {
                $this->callback(onQuit, $data);
                break;
                
              /** Kill messages */
              } elseif (ereg("^:(.+)!(.+)@(.+) KILL (.+) :(.*)", $buffer, $data)) {
                $this->callback(onKill, $data);
                
              /** Nickname taken */
              } elseif (ereg("^:(.+) 433", $buffer, $data)) {
                $botNick = ircConf::get("botNick");
                $this->write("NICK ".$botNick."_");
                if (($nickPass = ircConf::get("botNickServPass")) !== false) {
                  $this->privMsg("NickServ", "GHOST ".$botNick." ". $nickPass);
                  sleep(1); /** Wait then retry our nick and join channel */
                  $this->write("NICK ".$botNick);
                  $this->write("JOIN ".ircConf::get("channel"));
                  if (($modeLock = ircConf::get("channelMode")) !== false) {
                    $this->write("MODE ".ircConf::get("channel")." ".$modeLock);
                    unset($modeLock);
                  }
                  unset($nickPass);
                } else {
                  $this->log("Nickname is taken, can't GHOST - botNickServPass not set");
                }
                unset($botNick);
                break;
              /** Error messages */
              } elseif (ereg("^ERROR :(.+)", $buffer, $data)) {
                $this->log("ERROR: ".implode("] [", $data));
                $this->log("Recieved ERROR, reconnecting");
                break 2;
              }

              /** END Hanlders */
            unset($buffer, $data);
          }
        /** Check socket */
        } while (!feOf($this->conn) || !is_resource($this->conn));
        $this->log("Connection failed, cleaning up socket");
        fClose($this->conn);
        $this->retryConnections = true;
      } else {
        $this->retryConnections = true;
      }
      unset($this->conn);
      if ($this->retryCount >= 5) {
        if (!ircConf::get("stayAlive")) {
          $this->retryConnections = false;
        } else {
          $this->retryCount = 0;
          $this->log("Stay-Alive: trying again in 30 seconds\n");
          sleep(30);
        }
        
      }
    } while ($this->retryConnections);
    $this->log("Finished, shutting down");
    fClose($this->log);
    return false;
  }
  
  function _handleModes($modes, $data) {
    foreach ($modes as $mode) {
      $data[5] = $mode["data"];
      switch ($mode["mode"]) {
        case 'o': ($mode["switch"] == "+") ?
          $this->callback(onOp, $data) : $this->callback(onDeOp, $data);
        break;
        case 'v': ($mode["switch"] == "+") ?
          $this->callback(onVoice, $data) : $this->callback(onDeVoice, $data);
        break;
        case 'b': ($mode["switch"] == "+") ?
          $this->callback(onBan, $data) : $this->callback(onUnBan, $data);
        break;
        default:
          $this->callback(onMode, $data);
        break;
      }
    }
  }
  
  function notice($to, $message) {
    $this->write("NOTICE ".$to." :".$message);
  }

  function privMsg($to, $message) {
    $this->write("PRIVMSG ".$to." :".$message);
  }

  function ctcpNotice($to, $type, $string) {
    $this->write("NOTICE $to :".chr(1).strToUpper($type)." ".$string.chr(1));
  }

}

?>
