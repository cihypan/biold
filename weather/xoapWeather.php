<?
class xoapWeather
{
#######################################################################################
#xoapWeather - Process XML feeds from weather.com for display on a website            #
#			   keeping with in weather.com's standards for cacheing requests and links#
#Copyright (C) 2003  Brian Paulson <spectre013@spectre013.com>						  #
#																					  #
#This program is free software; you can redistribute it and/or 						  #
#modify it under the terms of the GNU General Public License                          #
#as published by the Free Software Foundation; either version 2                       #
#of the License, or (at your option) any later version.                               #
#																					  #
#This program is distributed in the hope that it will be useful,                      #
#but WITHOUT ANY WARRANTY; without even the implied warranty of                       #
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                        #
#GNU General Public License for more details.                                         # 
# 																					  #
#You should have received a copy of the GNU General Public License                    #
#along with this program; if not, write to the Free Software                          #
#Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.          #
#######################################################################################

########################################################
# VERSION 1.1										   #
########################################################
#        Sign-In Page: http://registration.weather.com/registration/xmloap/step1
#        E-mail address: bioh@biodome.org
#        Partner ID: 1004079260
#        License Key: 221f71a306dd1593

########################################################
# Weather Channel Partner Variables					   #
########################################################
var $xoapKey = 'xxx';		//Weather.com xoap Key
var $xoapPar = 'yyy'; 			//Partner ID
var $product = 'xoap';
//Note: you can get this information for free by signing up at http://www.weather.com
########################################################
# Weather Channel Cache Requirement Variables		   #
########################################################
var $currentCondCache = 1;  			//Minutes
var $multiDayforecastCache = 2;  		//hours
//Note* these are the minimum settings according to The Weather Channel xoap Product Guide

########################################################
# xoap Application Variables						   #
########################################################
var $defaultZip = CAXX0343;
var $sitePath;	      				 	//Path to the Document Root we will set this when the class starts
var $ccFile = 'cc.xml';  			 	//filename For Current Conditons 
var $forecastFile = 'forecast.xml';  	 	//filename For forecastdata
var $cacheDir = "wxCache"; 				// Folder where the XML files will be stored
var $forecastDays = 4; 					//How many Days the Extended forecast extends.
var $error;								//Error var for catching errors from weather.com
var $units = "m";			// s = standard m = metric;
var $Administrator = 'webmaster@somewhere.org';
var $error_text;
var $first_zip;

/**
*	Perform Setup Actions for class
*	@access	Private
*	@param	None
*	@return	None
*/

function xoapWeather()
	{
	$this->setZip();
	$this->setFiles();
	$this->statusCheck();
	$this->cacheControl();
	}

/**
*   Sets the Zip code that the program should use	
*	@access Private
*	@param	None
*	@return None
*/

function setZip()
	{
	global $zipcode,$sorm;
	
	$zipcode=$GLOBALS['zipcode'];
	$sorm=$GLOBALS['sorm'];

	if (isset($zipcode)) {
		$this->zip = $zipcode;
		}
	else
		{
		$this->zip = $this->defaultZip;
		}

	if (isset($sorm)) {
		$this->units = $sorm;
		}
	}

/**
*   Checks the status of certain Variables that are needed to execute	
*	@access Private
*	@param	None
*	@return None
*/

function statusCheck()
	{
	if($this->defaultZip == "")
		{
		$error .= "Please Specify a Zip code.<br>";
		}
	if($this->xoapKey == "")
		{
		$error .= "Your Xoap Key is Empty Please Visit <a href=\"http://www.weather.com\">http://www.weather.com</a> and sign-up for thier xoapXML Services.<br>";
		}
	if($this->xoapPar == "")
		{
		$error .= "Your Xoap Partner ID is Empty Please Visit <a href=\"http://www.weather.com\">http://www.weather.com</a> and sign-up for thier xoapXML Services.<br>";
		}
	if($this->cacheDir == "")
		{
		$error .= "Please Specify a cache Directory, be sure that data can be written to you cache dir by your web server.<br>";
		}
	if($error != "")
		{
		return;
		}
	
	}

/**
*	Sets the paths and filenames of for Proper cacheing.	
*	@access Private
*	@param	None
*	@return	None
*/


function setFiles()
	{
	// $this->sitePath = $_SERVER['DOCUMENT_ROOT']."/weather";
	$this->sitePath = "/home/biold/biold/weather";
	$this->cc = $this->sitePath."/".$this->cacheDir."/".$this->zip.$this->ccFile;
	$this->forecast = $this->sitePath."/".$this->cacheDir."/".$this->zip.$this->forecastFile;
	}

/**
*	Performs all cacheing for the application and file creation	
*	@access	Private
*	@param	None
*	@return None
*/


function cacheControl()
	{
	if(!is_file($this->cc))
		{
			if($this->error != 1)
			{
			$this->getXMLdata($this->zip,'cc'); 
			}
			if($this->error != 1)
			{
			$this->getXMLdata($this->zip,'forecast');
			}
		}
		else
		{
		$ccFiletime = filemtime($this->cc);
		$forecastFiletime = filemtime($this->forecast);
		$cccache = time() - ($this->currentCondCache* 60);
		$forecastCache = time() -  (60*60*$this->multiDayforecastCache);
	
		if($ccFiletime <= $cccache or $error == True)
			{
			$this->getXMLdata($this->zip,'cc'); 
			}
		
		if($forecastFiletime <= $forecastCache or $error == True)
			{
			$this->getXMLdata($this->zip,'forecast');
			}
		}
	$this->cleanCache();
	}

/**
*   Checks for cache Files that are older then 24 Hours old and removes them	
*	@access	Private
*	@param None
*	@return None
*/


function cleanCache()
	{
	$dir = $this->sitePath."/".$this->cacheDir;
	$open = opendir($dir);
	while($file = readdir($open))
		{
		if($file == "." or $file == "..")
			{
			
			}
			else
			{
			$Filetime = filemtime($dir."/".$file);
			$Purge = mktime(date("H")-24,date("i"),0,date("m"),date("d"),date("Y"));
			if($Purge > $Filetime)
				{
				unlink($dir."/".$file);
				} 
			}
	 	}
	
	}

/**
*	Retrives XML data from weather.com's website 	
*	@access Public
*	@param	$zip string Current Zipcode
*	@param	$type string Either 'cc' or 'forecast' 
*	@return None
*/

function getXMLdata($zip,$type)
        {
        if($type == "cc")
                {
                 $setup = "cc=*";
                 $file = $this->cc;
                }
                else
                {
                $setup = "dayf=".$this->forecastDays;
                $file = $this->forecast;
                }
		$zip= urlencode($zip);
        $stream = "http://xoap.weather.com/weather/local/".$zip."?".$setup."&link=xoap&unit=".$this->units."&prod=".$this->product."&par=".$this->xoapPar."&key=".$this->xoapKey."";	
        print "http://xoap.weather.com/weather/local/".$zip."?".$setup."&link=xoap&unit=".$this->units."&prod=".$this->product."&par=".$this->xoapPar."&key=".$this->xoapKey."";	
		$data = file($stream);
		$error = $this->errorCheck($data);
		if($error['number'] == 2)
			{
			$this->error = 1;
			$location = $this->locData($zip);
			$this->locSearch($location);
			}
			else
			{
			$xmi=join('',$data);
			if($xmi != "")
				{
				$open = fopen($file,"w");
				fputs($open,$xmi,strlen($xmi));
				fclose($open);
				}	
			}
        }

/**
*	Checks for errors in the XML file and handles them accordingly	
*	@access	Public
*	@param	$file string filename of the xml file that we are currenly loading 
*	@return	$error string True or NULL for detecting errors in the XML Feed
*/

function errorCheck($file)
	{
	/*
	if there is as problem with the XML file you request The weather channel returns an errror XML File
	here we take the errors and Display.
	*/
	$tree = $this->GetXMLTree($file);
	$error['number'] = $tree[ERROR][0][ERR][0][ATTRIBUTES][TYPE];
	$error['type'] = $tree[ERROR][0][ERR][0][VALUE];
	
	if($error['type'] != "" or $error['number'] != "")
		{			
		$error['exists'] = True;
		}
	
	return $error;
	}

/**
*	Displays an the error message in the XML Feed	
*	@access	Public
*	@param	$errror String True or NULL
*	@return None
*/


function errorMsg($error)
	{
	print $error;
	}
	
/**
*	Get the Children data from and XML file
*	@access	Public
*	@param	$vals Array current Nodes in the XML File 
*	@param	$i Integer current Increment in the process
*	@return	Return Array Children for Node
*/

function GetChildren($vals, &$i)
	{
 	 $children = array();     // Contains node data

  	/* Node has CDATA before it's children */
  	if (isset($vals[$i]['value']))
  	  $children['VALUE'] = $vals[$i]['value'];

  	/* Loop through children */
  	while (++$i < count($vals))
  	{
  	  switch ($vals[$i]['type'])
   	 {
    	  /* Node has CDATA after one of it's children
     	   (Add to cdata found before if this is the case) */
     	 case 'cdata':
      	  if (isset($children['VALUE']))
         	 $children['VALUE'] .= $vals[$i]['value'];
      	  else
         	 $children['VALUE'] = $vals[$i]['value'];
       	 break;
      	/* At end of current branch */
     	 case 'complete':
       	 if (isset($vals[$i]['attributes'])) {
       	 	  $children[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
        	  $index = count($children[$vals[$i]['tag']])-1;

       	   if (isset($vals[$i]['value']))
            $children[$vals[$i]['tag']][$index]['VALUE'] = $vals[$i]['value'];
          else
            $children[$vals[$i]['tag']][$index]['VALUE'] = '';
        } else {
          if (isset($vals[$i]['value']))
            $children[$vals[$i]['tag']][]['VALUE'] = $vals[$i]['value'];
          else
            $children[$vals[$i]['tag']][]['VALUE'] = '';
                }
        break;
      /* Node has more children */
      case 'open':
        if (isset($vals[$i]['attributes'])) {
          $children[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
          $index = count($children[$vals[$i]['tag']])-1;
          $children[$vals[$i]['tag']][$index] = array_merge($children[$vals[$i]['tag']][$index],$this->GetChildren($vals, $i));
        } else {
          $children[$vals[$i]['tag']][] = $this->GetChildren($vals, $i);
        }
        break;
      /* End of node, return collected data */
      case 'close':
        return $children;
    }
  }
}

/**
*	Function will attempt to open the xmlloc as a local file, on fail it will attempt to open it as a web link	
*	@access	Public
*	@param	string XML File to load
*	@return	$tree array of data in the XML file
*/

function GetXMLTree($xmlloc)
	{
	if(is_array($xmlloc))
		{
		$data = implode('', $xmlloc);
		}
		else
		{
			if(file_exists($xmlloc)) 
			  {
			   $data = implode('', file($xmlloc));
			  }
		}

  	$parser = xml_parser_create('ISO-8859-1');
 	 xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
  	xml_parse_into_struct($parser, $data, $vals, $index);
 	 xml_parser_free($parser);

  	$tree = array();
  	$i = 0;

  	if (isset($vals[$i]['attributes'])) 
		{
        $tree[$vals[$i]['tag']][]['ATTRIBUTES'] = $vals[$i]['attributes'];
        $index = count($tree[$vals[$i]['tag']])-1;
        $tree[$vals[$i]['tag']][$index] =  array_merge($tree[$vals[$i]['tag']][$index], $this->GetChildren($vals, $i));
  		}
 		else
		{
    	$tree[$vals[$i]['tag']][] = $this->GetChildren($vals, $i);
		}

  return $tree;
}

/**
*	Parses forecast XML file 		
*	@access Public
*	@param	None
*	@return	$forecast Array Contains and array with of the data in the forecast XML file
*/


function forecastData()
	{
	/*
	Here we are taking the Array from the XML file and putting it into an manageble array
	*/
	$xmi = $this->forecast;
	$tree = $this->GetXMLTree($xmi);
	$days = $tree[WEATHER][0][DAYF][0][DAY];
	$error = $this->errorCheck($xmi);

	$forecast[0]['loc'] = $tree[WEATHER][0][LOC][0][DNAM][0][VALUE];
	$forecast[0]['lsup'] = $tree[WEATHER][0][DAYF][0][LSUP][0][VALUE];
	$forecast[0]['unitsTemp'] = $tree[WEATHER][0][HEAD][0][UT][0][VALUE];                  
	$forecast[0]['unitsDistance'] = $tree[WEATHER][0][HEAD][0][UD][0][VALUE];
	$forecast[0]['unitsSpeed'] = $tree[WEATHER][0][HEAD][0][US][0][VALUE];
	$forecast[0]['unitsPrecip'] = $tree[WEATHER][0][HEAD][0][UP][0][VALUE];
	$forecast[0]['tempPressure'] = $tree[WEATHER][0][HEAD][0][UR][0][VALUE];
	$forecast[0]['linkOne'] = $tree[WEATHER][0][LNKS][0][LINK][0][L][0][VALUE];
	$forecast[0]['titleOne'] = $tree[WEATHER][0][LNKS][0][LINK][0][T][0][VALUE];
	$forecast[0]['linkTwo'] = $tree[WEATHER][0][LNKS][0][LINK][1][L][0][VALUE];
	$forecast[0]['titleTwo'] = $tree[WEATHER][0][LNKS][0][LINK][1][T][0][VALUE];
	$forecast[0]['linkThree'] = $tree[WEATHER][0][LNKS][0][LINK][2][L][0][VALUE];
	$forecast[0]['titleThree'] = $tree[WEATHER][0][LNKS][0][LINK][2][T][0][VALUE];
	$forecast[0]['linkFour'] = $tree[WEATHER][0][LNKS][0][LINK][3][L][0][VALUE];
	$forecast[0]['titleFour'] = $tree[WEATHER][0][LNKS][0][LINK][3][T][0][VALUE];
	$forecast[0]['error'] = $error;
	$forecast[0]['error_text'] = $this->error_text;
	/*
	With the Current Conditions we have up to 10 days of data that needs to be collected
	we do that here by looping though and grabbing the data
	*/
	for($i=0; $i<count($days); $i++)
			{
			
			$forecast[$i]['wkday'] = $days[$i][ATTRIBUTES][T];
			$forecast[$i]['date'] = $days[$i][ATTRIBUTES][DT];
			$forecast[$i]['hi'] = $days[$i][HI][0][VALUE];
			$forecast[$i]['lo'] = $days[$i][LOW][0][VALUE];
			$forecast[$i]['sunr'] = $days[$i][SUNR][0][VALUE];
			$forecast[$i]['suns'] = $days[$i][SUNS][0][VALUE];
			$forecast[$i]['part']['d']['icon'] = $days[$i][PART][0][ICON][0][VALUE];
			$forecast[$i]['part']['d']['cond'] = $days[$i][PART][0][T][0][VALUE];
			$forecast[$i]['part']['d']['windspeed'] = $days[$i][PART][0][WIND][0][S][0][VALUE];
			$forecast[$i]['part']['d']['windgust'] = $days[$i][PART][0][WIND][0][GUST][0][VALUE];
			$forecast[$i]['part']['d']['winddir'] = $days[$i][PART][0][WIND][0][T][0][VALUE];
			$forecast[$i]['part']['d']['ppcp'] = $days[$i][PART][0][PPCP][0][VALUE];
			$forecast[$i]['part']['d']['humid'] = $days[$i][PART][0][HMID][0][VALUE];
			$forecast[$i]['part']['n']['icon'] = $days[$i][PART][1][ICON][0][VALUE];
			$forecast[$i]['part']['n']['cond'] = $days[$i][PART][1][T][0][VALUE];
			$forecast[$i]['part']['n']['windspeed'] = $days[$i][PART][1][WIND][0][S][0][VALUE];
			$forecast[$i]['part']['n']['windgust'] = $days[$i][PART][1][WIND][0][GUST][0][VALUE];
			$forecast[$i]['part']['n']['winddir'] = $days[$i][PART][1][WIND][0][T][0][VALUE];
			$forecast[$i]['part']['n']['ppcp'] = $days[$i][PART][1][PPCP][0][VALUE];
			$forecast[$i]['part']['n']['humid'] = $days[$i][PART][1][HMID][0][VALUE];
			$forecast[$i]['error'] = $error;
			}
	return $forecast;	
}

/**
*	Displays Extended forecast for the current Zip Code	
*	@access	Public
*	@param	$forecast Array Contains and array with of the data in the forecast XML file
*	@return	None
*/


function extforecast($forecast)
        {
	if($this->error == 1)
		{
	 		 $this->errorMsg($cc['error']);
		}
		else
		{
		if($forecast[0]['error']['exists'] != True)
			{
		 	$forecast_text = $this->forecastDays." day forecast for ".$forecast[0]['loc']." :"; 
		// Extended forecast | Hi/Lo $forecast[0]['unitsTemp'] | Precip. %
		// loop through da days 
        	for($i=0; $i<count($forecast); $i++)
                	{
	                $tod = "d";
	                $hi = "High: ".$forecast[$i]['hi'].$forecast[0]['unitsTemp']." ";
			$lo ="Low: ".$forecast[$i]['lo'].$forecast[0]['unitsTemp']." ";

	                if(date("H") > 14 and $i == 0)
				{
				$tod = "n";
				$hi = "";
				$lo = "Tonights Low: ".$forecast[$i]['lo'].$forecast[0]['unitsTemp']." ";
				}
			$forecast_text=$forecast_text." [".$forecast[$i]['wkday']." ".$forecast[$i]['date']."] "; 
			$forecast_text=$forecast_text.$forecast[$i]['part'][$tod]['cond'].", ";
			$forecast_text=$forecast_text.$hi; 
			$forecast_text=$forecast_text.$lo.", "; 
			$forecast_text=$forecast_text."Precip %: ".$forecast[$i]['part'][$tod]['ppcp']." || "; 

	                }
			return ($forecast_text);
		  }
	 		 else
	 		 {
	 		 $this->errorMsg($cc['error']);
	         }
			}
        }
		
/**
*	Displays the Default forecast for the selected date and ZipCode	
*	@access	Public
*	@param	$forecast Array Data from XML File
*	@param	$did Integer  Day ID needed to build the forecast Details
*	@return	None	
*/

function detailforecast($forecast,$did)
        {
		/*
		Here we needed to know if it was after 1400 hours as that is when the Weather channel no
		longer send the data for that morning in the forecast file so we only show the 
		evening forecast after 1400 hours
		*/
		$tod = "day";
	if($this->error == 1)
		{
		
		}
		else
		{
		if($forecast[0]['error']['exists'] != True)
			{
			$hi = $forecast[$did]['hi'];
			$lo = $forecast[$did]['lo'];
			if(date("H") > 14 and $did == 0)
				{
				$tod = "night";
				}
		?>
<!--
xoapWeather is a weather presentation application for php. It takes the XML feed from 
the weather channel and parses it into the current conditions and the weekly forcast. 
Designed to meet the standards in weather.com's xoap XML Feed SDK.
©2003 Spectre013.com
--><table class="ccDetails" cellspacing="0" cellpadding="0" border="0" width="400">
	<tr align="center">
		<td class="wObserv">Forecast for <? echo $forecast[$did]['wkday']." ".$forecast[$did]['date']; ?></td>
 	</tr>
 	<tr>
 		<td colspan="2">
		<table cellspacing="1" cellpadding="1" border="0" width="100%">
		<?
		if($tod != "night")
			{
		?>
			<tr>
 				<td class="wTod" align="center" colspan="2">Day</td>
			</tr>
			<tr>
				<td class="wConditions" align="center" colspan="2">
					<img src="/weather/wxicons/128/<? echo $forecast[$did]['part']['d']['icon']; ?>.png" width="128" height="128" align="middle"><bR>
					<? echo $forecast[$did]['part']['d']['cond']; ?> <? echo $hi; ?>&deg; <? echo $forecast[0]['unitsTemp']; ?><br>
				</td>
			</tr>
			<tr>
 				<td class="wHumidity">Humidity</td>
				<td class="wHumidityValue"  align="right"><? echo $forecast[$did]['part']['d']['humid']; ?> %</td>
 			</tr>
 			<tr>
 				<td class="wWinds">Wind Speed</td>
 				<td class="wWindsValue" align="right"><? echo $forecast[$i]['part']['d']['winddir']; ?> <? echo $forecast[$did]['part']['d']['windspeed']; ?> <? echo $forecast[0]['unitsSpeed']; ?></td>
 			</tr>
			<tr>
 				<td class="wWindg">Wind Gusts</td>
 				<td class="wWindgValue" align="right"><? echo $forecast[$i]['part']['d']['winddir']; ?> <? echo $forecast[$did]['part']['d']['windgust'] == "N/A" ? "N/A" : $forecast[$did]['part']['d']['windgust']." ".$forecast[0]['unitsSpeed']; ?></td>
 			
			<tr>
 				<td class="wPrecip">Precipitation</td>
 				<td class="wPrecipValue" align="right"><? echo $forecast[$did]['part']['d']['ppcp']; ?>%</td>
 			</tr>
			<?
			}
			?>
			<!-- Night forecast-->

			<tr>
 				<td class="wTod" align="center" colspan="2">Night</td>
			</tr>
			<tr>
				<td class="wConditions" align="center" colspan="2">
					<img src="/weather/wxicons/128/<? echo $forecast[$did]['part']['n']['icon']; ?>.png" width="128" height="128" align="middle"><bR>
					<? echo $forecast[$did]['part']['n']['cond']; ?> <? echo $lo; ?>&deg; <? echo $forecast[0]['unitsTemp']; ?><br>
				</td>
			</tr>
			<tr>
 				<td class="wHumidity">Humidity</td>
				<td class="wHumidityValue"  align="right"><? echo $forecast[$did]['part']['n']['humid']; ?> %</td>
 			</tr>
 			<tr>
 				<td class="wWinds">Wind Speed</td>
 				<td class="wWindsValue" align="right"><? echo $forecast[$i]['part']['n']['winddir']; ?> <? echo $forecast[$did]['part']['n']['windspeed']; ?> <? echo $forecast[0]['unitsSpeed']; ?></td>
 			</tr>
			<tr>
 				<td class="wWindg">Wind Gusts</td>
 				<td class="wWindgValue" align="right"><? echo $forecast[$did]['part']['n']['windgust'] == "N/A" ? "N/A" : $forecast[$i]['part']['n']['winddir']." ".$forecast[$did]['part']['n']['windgust']." ".$forecast[0]['unitsSpeed']; ?></td>
 			</tr>
			<tr>
 				<td class="wPrecip">Precipitation</td>
 				<td class="wPrecipValue" align="right"><? echo $forecast[$did]['part']['n']['ppcp']; ?>%</td>
 			</tr>
			<tr>
				<td align="left"><a class="wforecastLink" href="forecast.php<? echo $_GET['zip'] == "" ? "" : "?zip=".$_GET['zip']; ?>"><- Back to forecast</a></td>
				<td class="wLsup" align="right" bgcolor="#ffffff" nowrap> Last Updated <? echo $forecast[0]['lsup']; ?></td>
			</tr>
		</table>
		</td>
	</tr>
</table>
<br>
<!-- xoapWeather - http://www.spectre013.com-->
<?
			  }
	 		 else
	 		 {
	 		 $this->errorMsg($cc['error']);
	         }
			}
        }
	
/**
*	Gets the Current Conditions data from the XML file and puts it into and aarry	
*	@access	Public
*	@param	None
*	@return	$cc Array Current Conditions Data
*/
		
function ccData()
	{
		/*
		Grabbing the Current XML data for the Current Condition and the Current Conditions details
		*/ 
		$xmi = $this->cc;
		$tree = $this->GetXMLTree($xmi);
		$error = $this->errorCheck($xmi);
		$cc['linkOne'] = $tree[WEATHER][0][LNKS][0][LINK][0][L][0][VALUE];
		$cc['titleOne'] = $tree[WEATHER][0][LNKS][0][LINK][0][T][0][VALUE];
		$cc['linkTwo'] = $tree[WEATHER][0][LNKS][0][LINK][1][L][0][VALUE];
		$cc['titleTwo'] = $tree[WEATHER][0][LNKS][0][LINK][1][T][0][VALUE];
		$cc['linkThree'] = $tree[WEATHER][0][LNKS][0][LINK][2][L][0][VALUE];
		$cc['titleThree'] = $tree[WEATHER][0][LNKS][0][LINK][2][T][0][VALUE];
		$cc['linkFour'] = $tree[WEATHER][0][LNKS][0][LINK][3][L][0][VALUE];
		$cc['titleFour'] = $tree[WEATHER][0][LNKS][0][LINK][3][T][0][VALUE];
		$cc['unitsTemp'] = $tree[WEATHER][0][HEAD][0][UT][0][VALUE];                  
		$cc['unitsDistance'] = $tree[WEATHER][0][HEAD][0][UD][0][VALUE];
		$cc['unitsSpeed'] = $tree[WEATHER][0][HEAD][0][US][0][VALUE];
		$cc['unitsPrecip'] = $tree[WEATHER][0][HEAD][0][UP][0][VALUE];
		$cc['tempPressure'] = $tree[WEATHER][0][HEAD][0][UR][0][VALUE];
		$cc['location'] = $tree[WEATHER][0][LOC][0][ATTRIBUTES][ID];
		$cc['sunrise'] = $tree[WEATHER][0][LOC][0][SUNR][0][VALUE];
		$cc['sunset'] = $tree[WEATHER][0][LOC][0][SUNS][0][VALUE];
		$cc['lastUpdate'] = $tree[WEATHER][0][CC][0][LSUP][0][VALUE];
		$cc['observStation'] = $tree[WEATHER][0][CC][0][OBST][0][VALUE];
		$cc['temp'] = $tree[WEATHER][0][CC][0][TMP][0][VALUE];
		$cc['feelsLike'] = $tree[WEATHER][0][CC][0][FLIK][0][VALUE];
		$cc['conditons'] = $tree[WEATHER][0][CC][0][T][0][VALUE];
		$cc['icon'] = $tree[WEATHER][0][CC][0][ICON][0][VALUE];
		$cc['baromoter'] = $tree[WEATHER][0][CC][0][BAR][0][R][0][VALUE];
		$cc['baromoterDesc'] = $tree[WEATHER][0][CC][0][BAR][0][D][0][VALUE];
		$cc['wind'] = $tree[WEATHER][0][CC][0][WIND][0][S][0][VALUE];
		$cc['windGust'] = $tree[WEATHER][0][CC][0][WIND][0][GUST][0][VALUE];
		$cc['windDirdeg'] = $tree[WEATHER][0][CC][0][WIND][0][D][0][VALUE];
		$cc['windDirname'] = $tree[WEATHER][0][CC][0][WIND][0][T][0][VALUE];
		$cc['humidity'] = $tree[WEATHER][0][CC][0][HMID][0][VALUE];
		$cc['visibility'] = $tree[WEATHER][0][CC][0][VIS][0][VALUE];
		$cc['uv'] = $tree[WEATHER][0][CC][0][UV][0][I][0][VALUE];
		$cc['uvDesc'] = $tree[WEATHER][0][CC][0][UV][0][T][0][VALUE];
		$cc['dewPoint'] = $tree[WEATHER][0][CC][0][DEWP][0][VALUE];
		$cc['error'] = $error;
		$cc['error_text'] = $this->error_text;
		
		
	return $cc;
	}	

/**
*	Displays Current Condtion	
*	@access	Public
*	@param	$cc Array array containing the Current Conditions Data
*	@return	None
*/

	
function currentConditions($cc)
	{
	
	if($cc['error']['exists'] != True)
		{
		?>
<!--
xoapWeather is a weather presentation application for php. It takes the XML feed from 
the weather channel and parses it into the current conditions and the weekly forcast. 
Designed to meet the standards in weather.com's xoap XML Feed SDK.
©2003 Spectre013.com
-->
<table class="ccDetails" width="80" cellpadding="0" cellspacing="0" border="0">
	<tr>
		<td class="wConditions" width="80" align="center"><img border="0" src="/weather/wxicons/32/<? echo $cc['icon']; ?>.png" width="32" height="32">
						   <bR><? echo $cc['temp']; ?>&deg;<br><? echo $cc['conditons']; ?></td>
	</tr>
	<tr>
		<td class="wDetails">
		   <a class="wDetails" href="/weather/details.php<? echo $_SERVER['QUERY_STRING'] == "" ? "" : "?".$_SERVER['QUERY_STRING']; ?>">Details</a>
		</td>
						
		</td>
	</tr>
</table>
<!-- xoapWeather - http://www.spectre013.com-->
		<?
	  }
	  else
	  {
	  $this->errorMsg($cc['error']);
	  }

	}

/**
*	Displays Current Condition Details	
*	@access	Public
*	@param	$cc Array array containing the Current Conditions Data
*	@return	None
*/


function ccDetails($cc)
	{
	if($this->error == 1)
		{
		}
		else
		{
		if($cc['error']['exists'] != True)
			{
			$txt_cond = "Conditions for ".$cc['observStation']." | ".$cc['conditons']." | ";
			$txt_temp = $cc['temp'].$cc['unitsTemp']." | ";
			$txt_feels = "Feels Like ".$cc['feelsLike'].$cc['unitsTemp']." | ";
			$txt_humidity = "Humidity ".$cc['humidity']."% | ";
			$txt_wind = "Wind Speed ";
			if ($cc['wind'] == "calm") 
				{
				$txt_wind = $txt_wind . "Calm";
				}
			else
				{
				$txt_wind = $txt_wind . $cc['windDirname']." ".$cc['wind']." ".$cc['unitsSpeed'];
				}

			if ($cc['windGust'] == "N/A")
				{
				$txt_wind = $txt_wind . " | ";
				}
			else
				{
				$txt_wind = $txt_wind . "(gusting to ".$cc['windGust']." ".$cc['unitsSpeed'].") | ";
				}

			$txt_baro = "Barometer ".$cc['baromoter']." ".$cc['baromoterDesc']." | ";
			$txt_dew = "Dewpoint ".$cc['dewPoint'].$cc['unitsTemp']." | ";
			$txt_visi = "Visibility ";
			if ($cc['visibility'] == "Unlimited") 
				{
				$txt_visi = $txt_visi . $cc['visibility']." | ";
				}
			else
				{
				$txt_visi = $txt_visi . $cc['visibility']." ".$cc['unitsDistance']." | ";
				}
			$txt_uv = "UV Index ".$cc['uv']." ".$cc['uvDesc']." | ";
			$txt_sunrise = "Sunrise ".$cc['sunrise']." | ";
			$txt_sunset = "Sunset ".$cc['sunset'];

			$txt_weather = $txt_cond.$txt_temp.$txt_feels.$txt_humidity.$txt_wind.$txt_baro.$txt_dew.$txt_visi.$txt_uv.$txt_sunrise.$txt_sunset;
			return ($txt_weather);

		}
	  	else
	  	{
			$this->errorMsg($cc['error']);
		 }
	}
	}

function locData($loc)
	{
	$stream = "http://xoap.weather.com/search/search?where=".$loc;
	$data = file($stream);
	if($data != "")
		{
		$tree = $this->GetXMLTree($data);
		}
	for($i=0; $i<count($tree[SEARCH][0][LOC]); $i++)
		{
			$info[$i]['zip'] = $tree[SEARCH][0][LOC][$i][ATTRIBUTES][ID];
			$info[$i]['name'] = $tree[SEARCH][0][LOC][$i][VALUE];
		}
	return $info;
	}

function locSearch($loc)
	{
	 $total = count($loc);
	 if($total == 0)
	 	{
		$err_msg = "Unable to locate any Locations that matched the information you entered. If you entred an actual zip code please check to see that you entered it correctly. If you entred a City name please check that the name you entered is spelled correctly and try again.";
		$this->error_text=$err_msg;
		}
	else
		{
		if ($total > 1) 
			{
			$err_msg = "Sorry, but I was unable to locate the City you entred. I did locate several Zip Codes that matched. Please select the [Zipcode] from the list to view the Weather. : ";

			for($i=0; $i<$total; $i++) 
				{
				$err_msg = $err_msg . "[".$loc[$i]['zip']."] ".$loc[$i]['name']." | ";
				}
			$this->error_text=$err_msg;
			$this->first_zip=$loc[0]['zip'];
			}
		else
			{
			$this->first_zip=$loc[0]['zip'];
			}
		}
	}
#############################################################################################################
}//End Class
?>
