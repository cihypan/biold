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

            $host="www.xe.com";
            $file="/currencyconverter/convert/";
            $str = "?Amount=".$this->_amt."&From=".$this->_from."&To=".$this->_to;

                //$str = "?amount=".$this->_amt."&ConvertFrom=".$this->_from."&ConvertTo=".$this->_to;
		// http://www.xe.com/currencyconverter/convert/?Amount=1&From=USD&To=CAD
	// hack it :/
	// wget --user-agent="Mozilla/5.0 (Windows NT 5.2; rv:2.0.1) Gecko/20100101 Firefox/4.0.1" "http://www.xe.com/currencyconverter/convert/?Amount=1&From=USD&To=CAD"

		$wget_command = "wget -O /tmp/xe.com.txt -nv --user-agent=\"Mozilla/5.0 (Windows NT 5.2; rv:2.0.1) Gecko/20100101 Firefox/4.0.1\" ";
		$url = "\"http://".$host.$file.$str."\"";

		$command = $wget_command.$url;

		exec ($command);

		$data = file_get_contents("/tmp/xe.com.txt");

                // @preg_match("/^(.*?)\r?\n\r?\n(.*)/s", $data, $match);
		
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
		
		print_r($mathces);
		//            [2] => 1.00
		//            [3] => 1.03220
		//            [4] => 1 USD = 1.03220
		//            [5] => 1 CAD = 0.968803


                $return=preg_replace("/[^\d\.]*/","",$mathces[0][3]);
                return (double)$return;
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
