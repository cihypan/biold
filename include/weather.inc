<?
function weather_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}

   if (trim(strToLower(substr($text, 0, 8))) == "!weather") 
	{
	if (strlen(trim($text)) < 9) 
		{
		$bot->privMsg($dest, "Weather syntax (in metric) : !weather <zipcode> or <city>");
		}
	else	
		{
		$city_txt = trim(substr($text, 9));

		// $city_txt = "ottawa, canada";

		if (stristr($city_txt, ",") === FALSE )
			{
			// no commas, check for space?
			if (stristr ($city_txt, " ") === FALSE)
				{
				// no space or comma, just the city
				$city_array_city = trim($city_txt);
				$city_array_country = "Canada"; 
				}
			else
				{
				// ottawa canada, split it up
				$city_array = explode (" ", $city_txt);
				$city_array_city = trim($city_array[0]);
				$city_array_country = trim($city_array[1]);
				}
			}
		else
			{
			// ottawa, canada  split it up
			$city_array = explode (",", $city_txt);
			$city_array_city = trim($city_array[0]);
			$city_array_country = trim($city_array[1]);
			}

	
		$city_txt = urlencode($city_txt);

		if (is_numeric($city_array_city))
			{
			// just a zip code
		    	$json_string = file_get_contents("http://api.wunderground.com/api/xxxx_insert_your_api_key_xxxxxxx/almanac/forecast/conditions/q/".$city_array_city.".json");
			}
		else
			{
		if ($city_array_country <> "") 
			{
			$json_string = file_get_contents("http://api.wunderground.com/api/xxxx_insert_your_api_key_xxxxxxx/geolookup/almanac/forecast/conditions/q/".$city_array_country."/".$city_array_city.".json");
			}
		else 
			{
			$json_string = file_get_contents("http://api.wunderground.com/api/xxxx_insert_your_api_key_xxxxxxx/geolookup/almanac/forecast/conditions/q/IA/".$city_txt.".json");
			}
			}

		// ",true" gets it into array nicely
		$parsed_json = json_decode($json_string,true);
		
		print_r($parsed_json);
		
		// $location = $parsed_json->{'location'}->{'city'};
		// $temp_f = $parsed_json->{'current_observation'}->{'temp_f'};
		// echo "Current temperature in ${location} is: ${temp_f}\n";
				
		$check_many_results = count($parsed_json["response"]["results"]);
		if ($check_many_results > 1)
		    {
		    foreach ($parsed_json["response"]["results"] as $result_key => $result_value)
		            {
		            $result_name = $result_value["name"];
		            $result_city = $result_value["city"];
		            $result_state = $result_value["state"];
		            $result_country = $result_value["country"];
		            $result_country_name = $result_value["country_name"];
		            $result_url = $result_value["l"];
		
		            $result_txt = $result_txt . $result_name . ", " . $result_state . ", " . $result_country_name . " [" . $result_key ."] | ";
		            }
		    // echo $result_txt;
		
		    // echo $result_url;

		    // debug
		    // $result_multiple_txt = "Received multiple results ( " . count($parsed_json["response"]["results"]) . " ) --- DEFAULTING to " . $result_name . ", " . $result_state . ", " . $result_country_name . "\n";
		    // $bot->privMsg($dest, $result_multiple_txt);

		    // change default to first record found
		    $result_url = $parsed_json["response"]["results"][0]["l"];
		
		    $json_string = file_get_contents("http://api.wunderground.com/api/xxxx_insert_your_api_key_xxxxxxx/almanac/forecast/conditions".$result_url.".json");
		    $parsed_json = json_decode($json_string,true);
		
		    print_r($parsed_json);
		    
		    $result_location_city = $parsed_json["current_observation"]["display_location"]["full"];
		    $result_current_obs_location_city = $parsed_json["current_observation"]["observation_location"]["city"];
		    $result_weather = $parsed_json["current_observation"]["weather"];
		    $result_tempf = $parsed_json["current_observation"]["temp_f"];
		    $result_tempc = $parsed_json["current_observation"]["temp_c"];
		    $result_temp_str = $parsed_json["current_observation"]["temperature_string"];
		    $result_humidity = $parsed_json["current_observation"]["relative_humidity"];
		    $result_wind_str = $parsed_json["current_observation"]["wind_string"];
		    $result_heat_str = $parsed_json["current_observation"]["heat_index_string"];
		    $result_windchill_str = $parsed_json["current_observation"]["windchill_string"];
		    $result_precip_today = $parsed_json["current_observation"]["precip_today_string"];
		    $result_almanac_highc = $parsed_json["almanac"]["temp_high"]["normal"]["C"];
		    $result_almanac_highf = $parsed_json["almanac"]["temp_high"]["normal"]["F"];
		    $result_almanac_lowc = $parsed_json["almanac"]["temp_low"]["normal"]["C"];
		    $result_almanac_lowf = $parsed_json["almanac"]["temp_low"]["normal"]["F"];
		    $result_almanac_record_highc = $parsed_json["almanac"]["temp_high"]["record"]["C"];
		    $result_almanac_record_highf = $parsed_json["almanac"]["temp_high"]["record"]["F"];
		    $result_almanac_record_highyear = $parsed_json["almanac"]["temp_high"]["recordyear"];
		    $result_almanac_record_lowc = $parsed_json["almanac"]["temp_low"]["record"]["C"];
		    $result_almanac_record_lowf = $parsed_json["almanac"]["temp_low"]["record"]["F"];
		    $result_almanac_record_lowyear = $parsed_json["almanac"]["temp_low"]["recordyear"];
		

		    $result_weather_txt = $result_location_city . " ( " . $result_current_obs_location_city . " ) - " . $result_weather . ", " . $result_temp_str . ", Humidity " . $result_humidity . ", Wind " . $result_wind_str . ", Heat Index " . $result_heat_str . ", Windchill " .  $result_windchill_str . ", Precip " . $result_precip_today;
		    $result_almanac_txt = ", Almanac high " . $result_almanac_highf . "F (" . $result_almanac_highc . "C), low " . $result_almanac_lowf . "F (" . $result_almanac_lowc . "C)";

 		    $result_record_txt = ", Record HIGH (" . $result_almanac_record_highyear . ") " . $result_almanac_record_highf . "F (" . $result_almanac_record_highc . "C), Record LOW (" . $result_almanac_record_lowyear . ") " . $result_almanac_record_lowf . "F (" . $result_almanac_record_lowc . "C) ";

		    if ($result_lamanac_highc) 
			{
			}

		    $result_weather_txt = $result_weather_txt . $result_almanac_txt;

		    if ($result_almanac_record_highyear) 
			{
			$result_weather_txt = $result_weather_txt . $result_record_txt;
			}
	
		    $bot->privMsg($dest, $result_weather_txt);

		    // do forecast if available

		    $result_forecast_today_day = $parsed_json["forecast"]["txt_forecast"]["forecastday"][0]["title"];
		    $result_forecast_today_text = $parsed_json["forecast"]["txt_forecast"]["forecastday"][0]["fcttext_metric"];

		    $result_forecast_night_day = $parsed_json["forecast"]["txt_forecast"]["forecastday"][1]["title"];
		    $result_forecast_night_text = $parsed_json["forecast"]["txt_forecast"]["forecastday"][1]["fcttext_metric"];

		    $result_forecast_txt = $result_forecast_today_day . " - [ " . $result_forecast_today_text . " ]";
		    $result_forecast_txt = $result_forecast_txt . " | " . $result_forecast_night_day . " - [ " . $result_forecast_night_text . " ]";

		    $result_forecast_txt = str_replace('&deg;', ' ', $result_forecast_txt);
		    $bot->privMsg($dest, $result_forecast_txt);
		}
		else
			{		
			$result_location_city = $parsed_json["current_observation"]["display_location"]["full"];
			$result_current_obs_location_city = $parsed_json["current_observation"]["observation_location"]["city"];
			$result_weather = $parsed_json["current_observation"]["weather"];
			$result_tempf = $parsed_json["current_observation"]["temp_f"];
			$result_tempc = $parsed_json["current_observation"]["temp_c"];
			$result_temp_str = $parsed_json["current_observation"]["temperature_string"];
			$result_humidity = $parsed_json["current_observation"]["relative_humidity"];
			$result_wind_str = $parsed_json["current_observation"]["wind_string"];
			$result_heat_str = $parsed_json["current_observation"]["heat_index_string"];
			$result_windchill_str = $parsed_json["current_observation"]["windchill_string"];
			$result_precip_today = $parsed_json["current_observation"]["precip_today_string"];
			$result_almanac_highc = $parsed_json["almanac"]["temp_high"]["normal"]["C"];
			$result_almanac_highf = $parsed_json["almanac"]["temp_high"]["normal"]["F"];
			$result_almanac_lowc = $parsed_json["almanac"]["temp_low"]["normal"]["C"];
			$result_almanac_lowf = $parsed_json["almanac"]["temp_low"]["normal"]["F"];
			$result_almanac_record_highc = $parsed_json["almanac"]["temp_high"]["record"]["C"];
			$result_almanac_record_highf = $parsed_json["almanac"]["temp_high"]["record"]["F"];
			$result_almanac_record_highyear = $parsed_json["almanac"]["temp_high"]["recordyear"];
			$result_almanac_record_lowc = $parsed_json["almanac"]["temp_low"]["record"]["C"];
			$result_almanac_record_lowf = $parsed_json["almanac"]["temp_low"]["record"]["F"];
			$result_almanac_record_lowyear = $parsed_json["almanac"]["temp_low"]["recordyear"];

	
		        $result_weather_txt = $result_location_city . " ( " . $result_current_obs_location_city . " ) - " . $result_weather . ", " . $result_temp_str . ", Humidity " . $result_humidity . ", Wind " . $result_wind_str . ", Heat Index " . $result_heat_str . ", Windchill " .  $result_windchill_str . ", Precip " . $result_precip_today;

		        $result_almanac_txt = ", Almanac high " . $result_almanac_highf . "F (" . $result_almanac_highc . "C), low " . $result_almanac_lowf . "F (" . $result_almanac_lowc . "C)";

			$result_record_txt = ", Record HIGH (" . $result_almanac_record_highyear . ") " . $result_almanac_record_highf . "F (" . $result_almanac_record_highc . "C), Record LOW (" . $result_almanac_record_lowyear . ") " . $result_almanac_record_lowf . "F (" . $result_almanac_record_lowc . "C) ";

  		        if ($result_lamanac_highc) 
				{
				// have to rethink this 'if' statement ;)
				}

			$result_weather_txt = $result_weather_txt . $result_almanac_txt;

			if ($result_almanac_record_highyear) 
				{
				$result_weather_txt = $result_weather_txt . $result_record_txt;
				}

			$bot->privMsg($dest, $result_weather_txt);

			// do forecast if available

			$result_forecast_today_day = $parsed_json["forecast"]["txt_forecast"]["forecastday"][0]["title"];
			$result_forecast_today_text = $parsed_json["forecast"]["txt_forecast"]["forecastday"][0]["fcttext_metric"];
			
			$result_forecast_night_day = $parsed_json["forecast"]["txt_forecast"]["forecastday"][1]["title"];
			$result_forecast_night_text = $parsed_json["forecast"]["txt_forecast"]["forecastday"][1]["fcttext_metric"];
			
			$result_forecast_txt = $result_forecast_today_day . " - [ " . $result_forecast_today_text . " ]";
			$result_forecast_txt = $result_forecast_txt . " | " . $result_forecast_night_day . " - [ " . $result_forecast_night_text . " ]";
			
		    	$result_forecast_txt = str_replace('&deg;', ' ', $result_forecast_txt);
			$bot->privMsg($dest, $result_forecast_txt);
			}
		}
	}
}

function weather_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }

   if (trim(strToLower(substr($text, 0, 8))) == "!weather")
        {
        if (strlen(trim($text)) < 9)
                {
                $bot->privMsg($nick, "Weather syntax (in metric) : !weather <zipcode> or <city>");
                }
        else    
                {
                $zipcode = trim(substr($text, 9));
                $sorm="m";     // standard .. as opposed to metric :)
                 
                $weather = new xoapWeather();
                $cc = $weather->ccData();
                
                if ($weather->error == 1)
                        {
                        $bot->privMsg($nick, $weather->error_text);
                        
                        // if there are actually any options displayed, lets use the first one right away
                        if (isset($weather->first_zip))
                                {   
                                // automatically look up the first returned zip
                                // as its right most of the time anyway
                                $zipcode=$weather->first_zip;
                                $sorm="m";     // metric / standard:)
                                 
                                $weather_first = new xoapWeather();
                                $cc_first = $weather_first->ccData();
                                
                                // but only print if there is no error this time
                                if ($weather_first->error != 1)
                                        {
                                        $bot->privMsg($nick, $weather_first->ccDetails($cc_first));
                                        }
                                }
                        }
                else    
                        {
                        $bot->privMsg($nick, $weather->ccDetails($cc));
                        }
                }
        }
}


function fweather_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}

   if (trim(strToLower(substr($text, 0, 9))) == "!fweather") 
	{
	if (strlen(trim($text)) < 10) 
		{
		$bot->privMsg($dest, "Weather syntax (in metric) : !fweather <zipcode> or <city>");
		}
	else	
		{
		$zipcode = trim(substr($text, 10));
		$sorm="s";     // standard .. as opposed to metric :)

		$weather = new xoapWeather();
		$cc = $weather->ccData();

		if ($weather->error == 1)
			{
			$bot->privMsg($nick, $weather->error_text);
                        // if there are actually any options displayed, lets use the first one right away
                        if (isset($weather->first_zip))
                                {
                                // automatically look up the first returned zip
                                // as its right most of the time anyway
                                $zipcode=$weather->first_zip;
                                $sorm="s";     // metric / standard:)

                                $weather_first = new xoapWeather();
                                $cc_first = $weather_first->ccData();

                                // but only print if there is no error this time
                                if ($weather_first->error != 1)
                                        {
                                        $bot->privMsg($dest, $weather_first->ccDetails($cc_first));
                                        }
                                }

			}
		else
			{
			$bot->privMsg($dest, $weather->ccDetails($cc));
			}
		}
	}
}

function fweather_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }

   if (trim(strToLower(substr($text, 0, 9))) == "!cweather")
        {
        if (strlen(trim($text)) < 10)
                {
                $bot->privMsg($nick, "Weather syntax (in metric) : !cweather <zipcode> or <city>");
                }
        else
                {
                $zipcode = trim(substr($text, 10));
                $sorm="s";     // standard .. as opposed to metric :)

                $weather = new xoapWeather();
                $cc = $weather->ccData();

                if ($weather->error == 1)
                        {
                        $bot->privMsg($nick, $weather->error_text);
                        // if there are actually any options displayed, lets use the first one right away
                        if (isset($weather->first_zip))
                                {
                                // automatically look up the first returned zip
                                // as its right most of the time anyway
                                $zipcode=$weather->first_zip;
                                $sorm="s";     // metric / standard:)

                                $weather_first = new xoapWeather();
                                $cc_first = $weather_first->ccData();

                                // but only print if there is no error this time
                                if ($weather_first->error != 1)
                                        {
                                        $bot->privMsg($nick, $weather_first->ccDetails($cc_first));
                                        }
                                }

                        }
                else
                        {
                        $bot->privMsg($nick, $weather->ccDetails($cc));
                        }
                }
        }
}

function forecast_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}

   if (trim(strToLower(substr($text, 0, 9))) == "!forecast") 
	{
	if (strlen(trim($text)) < 10) 
		{
		$bot->privMsg($nick, "Forecast syntax : !forecast <zipcode> or <city>");
		}
	else	
		{
		$zipcode = trim(substr($text, 10));
		$sorm="m";     // standard .. as opposed to metric :)

		$weather = new xoapWeather();
		$forecast = $weather->forecastData();

		if ($weather->error == 1)
			{
			$bot->privMsg($dest, $weather->error_text);
                        if (isset($weather->first_zip))
                                {
                                // automatically look up the first returned zip
                                // as its right most of the time anyway
                                $zipcode=$weather->first_zip;
                                $sorm="m";     // metric / standard:)

                                $forecast_first = new xoapWeather();
                                $forecast_result = $forecast_first->forecastData();

                                // but only print if there is no error this time
                                if ($forecast_first->error != 1)
                                        {
                                        $bot->privMsg($dest, $forecast_first->extforecast($forecast_result));
                                        }
                                }
			}
		else
			{
			$bot->privMsg($dest, $weather->extforecast($forecast));
			}
		}
	}
}

function fforecast_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}

   if (trim(strToLower(substr($text, 0, 10))) == "!fforecast") 
	{
	if (strlen(trim($text)) < 11) 
		{
		$bot->privMsg($nick, "Forecast syntax : !forecast <zipcode> or <city>");
		}
	else	
		{
		$zipcode = trim(substr($text, 11));
		$sorm="s";     // standard .. as opposed to metric :)

		$weather = new xoapWeather();
		$forecast = $weather->forecastData();

		if ($weather->error == 1)
			{
			$bot->privMsg($dest, $weather->error_text);
                        if (isset($weather->first_zip))
                                {
                                // automatically look up the first returned zip
                                // as its right most of the time anyway
                                $zipcode=$weather->first_zip;
                                $sorm="s";     // metric / standard:)

                                $forecast_first = new xoapWeather();
                                $forecast_result = $forecast_first->forecastData();

                                // but only print if there is no error this time
                                if ($forecast_first->error != 1)
                                        {
                                        $bot->privMsg($dest, $forecast_first->extforecast($forecast_result));
                                        }
                                }
			}
		else
			{
			$bot->privMsg($dest, $weather->extforecast($forecast));
			}
		}
	}
}

function forecast_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
   global $zipcode, $sorm;
  if ((strcasecmp($nick, "teebag") == 0))
        {
        return;
        }

   if (trim(strToLower(substr($text, 0, 9))) == "!forecast")
        {
        if (strlen(trim($text)) < 10)
                {
                $bot->privMsg($nick, "Forecast syntax : !forecast <zipcode> or <city>");
                }
        else
                {
                $zipcode = trim(substr($text, 10));
                $sorm="m";     // standard .. as opposed to metric :)

                $weather = new xoapWeather();
                $forecast = $weather->forecastData();

                if ($weather->error == 1)
                        {
                        $bot->privMsg($nick, $weather->error_text);
                        }
                else
                        {
                        $bot->privMsg($nick, $weather->extforecast($forecast));
                        }
                }
        }
}
?>
