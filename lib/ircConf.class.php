<?php
/* $Id$
   ------------------------------------------------------------------
 | ircConf.class.php                                                  |
 | ------------------------------------------------------------------ |
 | Copyright (C) 2003 Donovan Schönknecht <ds@undesigned.org.za>      |
 | ------------------------------------------------------------------ |
 | Subtility - Bot Configuration                                      |
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

$GLOBALS["ircConf"] = array();
$GLOBALS["ircConf"]["users"] = array();

/** Configuration class, handles settings and data */
class ircConf {

  function set($var, $value) {
    $GLOBALS["ircConf"][$var] = $value;
  }

  function get($var) {
    if (isset($GLOBALS["ircConf"][$var])) {
      return $GLOBALS["ircConf"][$var];
    }
    return false;
  }

  function setUser($nick, $ident, $host, $access) {
    $GLOBALS["ircConf"]["users"]["$nick"] = array(
      "nick" => $nick, "ident" => $ident,
      "host" => $host, "access" => $access);
  }
  
  function isUser($nick, $ident, $host, $checkAccess = false) {
    if (isset($GLOBALS["ircConf"]["users"][$nick])) {
      if ($ident !== $GLOBALS["ircConf"]["users"][$nick]["ident"]) return false;

      if (!ircConf::_hostCompare(
      $GLOBALS["ircConf"]["users"][$nick]["host"], $host)) {
        return false;
      }

      if ($checkAccess !== false) {
        if ($checkAccess !== $GLOBALS["ircConf"]["users"][$nick]["access"]) {
          return false;
        }
      }
      return true;
    }
    return false;
  }
  
  /** Compare two hosts, allowing wildcards */
  function _hostCompare($expected, $real) {
    $eData = explode(".", $expected);
    $rData = explode(".", $real);
    $eSize = sizeof($eData);
    $rSize = sizeof($rData);
    if ($eSize == $rSize) {
      for ($i = 0; $i < $eSize; $i++) {
        if ($eData[$i] !== "*") {
          if ($eData[$i] !== $rData[$i]) return false;
        }
      }
      return true;
    }
    return false;
  }

}

?>
