<?php
    
    /*
        CURRENCYCONVERTER
        Date - Feb 23,2005
        Author - Harish Chauhan
        Email - harishc@ultraglobal.biz

	modified by bio : bioh@biodome.org
	-- edited to fit into biosubtility and added country_to_currency function

        ABOUT
        This PHP script will use for conversion of currency.
        you can find it is tricky but it is usefull.
    */

    Class CURRENCYCONVERTER
    {
        var $_amt=1;
        var $_to="";
        var $_from="";
        var $_error="";
        function CURRENCYCONVERTER($amt=1,$to="",$from="")
        {
            $this->_amt=$amt;
            $this->_to=$to;
            $this->_from=$from;
        }
        function error()
        {
            return $this->_error;
        }
        function convert($amt=NULL,$to="",$from='USD')
        {
            if($amt>1)
                $this->_amt=$amt;
            if(!empty($to))
                $this->_to=$to;
            if(!empty($from))
                $this->_from=$from;

            //$host="www.iraqidinar.org";
            $host="www.xe.com";
            $fp = @fsockopen($host, 80, $errno, $errstr, 30);
            if (!$fp)
            {
                $this->_error="$errstr ($errno)<br />\n";
                return false;
            }
            else
            {
                //$file="/conversiontool2.asp";
                $file="/ucc/convert/";
                //$str = "?amount=".$this->_amt."&ConvertFrom=".$this->_from."&ConvertTo=".$this->_to;
                $str = "?language=xe&Amount=".$this->_amt."&From=".$this->_from."&To=".$this->_to;
                $out = "GET ".$file.$str." HTTP/1.0\r\n";
                $out .= "Host: $host\r\n";
                $out .= "Connection: Close\r\n\r\n";

                @fputs($fp, $out);
                while (!@feof($fp))
                {
                    $data.= @fgets($fp, 128);
                }
                @fclose($fp);
                
                @preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $data, $match);
                $data =$match[2];
                $search = array ("'<script[^>]*?>.*?</script>'si",  // Strip out javascript
                                 "'<[\/\!]*?[^<>]*?>'si",           // Strip out HTML tags
                                 "'([\r\n])[\s]+'",                 // Strip out white space
                                 "'&(quot|#34);'i",                 // Replace HTML entities
                                 "'&(amp|#38);'i",
                                 "'&(lt|#60);'i",
                                 "'&(gt|#62);'i",
                                 "'&(nbsp|#160);'i",
                                 "'&(iexcl|#161);'i",
                                 "'&(cent|#162);'i",
                                 "'&(pound|#163);'i",
                                 "'&(copy|#169);'i",
                                 "'&#(\d+);'e");                    // evaluate as php

                $replace = array ("",
                                  "",
                                  "\\1",
                                  "\"",
                                  "&",
                                  "<",
                                  ">",
                                  " ",
                                  chr(161),
                                  chr(162),
                                  chr(163),
                                  chr(169),
                                  "chr(\\1)");

                $data = @preg_replace($search, $replace, $data);
		// print ("DEBUG DATA for CURRENCY :" . $data . " -- END DEBUG");
                @preg_match_all("/(\d[^\.]*(\.\d+)?)/",$data,$mathces);
		
		// print_r($mathces);

                $return=preg_replace("/[^\d\.]*/","",$mathces[0][3]);
                return (double)$return;
            }
        }
	function country_to_currency ($country)
		{
		include ("db.php");
		$country = mysql_real_escape_string($country);
		$query = "SELECT * FROM currency WHERE COUNTRY LIKE '%".$country."%'";
		$query_result = mysql_query ($query);
		// if (mysql_errno()) { echo  "currency (): ERROR: ".mysql_errno().":".mysql_error(); }
		if ($query_result > 0)
			{
			$result_object = mysql_fetch_object ($query_result);
			$result_currency = $result_object->CURRENCY;
			$result_code = $result_object->CURRENCY_CODE;

		// 	echo $result_code;
			$result_array[0] = $result_code;
			$result_array[1] = $result_currency;
			return $result_array;
			}
		}
	function check_currency_code ($code)
		{
		include ("db.php");
		$code = mysql_real_escape_string($code);
		$query = "SELECT * FROM currency WHERE CURRENCY_CODE = '".$code."'";
	 	$query_result = mysql_query ($query);
		// if (mysql_errno()) { echo  "code (): ERROR: ".mysql_errno().":".mysql_error(); }
		if ($query_result > 0)
			{
			if (mysql_num_rows ($query_result) > 0)
				{
				$result_array = mysql_fetch_array($query_result);
				// print_r ($result_array);
				return true;
				}
			else
				{
				return false;
				}
			}
		else
			{
				return false;
			}
		}
    }
?> 
