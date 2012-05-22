<?php
/* Dadan Ramdan
 CurConv release 3.0
 License : Free
 thenewhiking@yahoo.com
 USD for USA
 IDR for Indonesian
 -- history Software --
 release 3.0
 modification from Jeffrey Hill script, www.flash-db.com
 release 2.0
 Modification from v1.0 becoming class
 release 1.0
 modification from METEO live v1.0 by Martin Bolduc
*/

class curconv {
        // afghanistan albania algeria andorra andorra angola argentina aruba australia austria bahrain
	// bangladesh barbados belgium belize bermuda bhutan bolivian botswana brazil england united 
	// kingdom uk great britain brunei burundi cambodia canada cape verde cayman islands chile china 
	// colombia comoros costa rica croatia cuba cyprus czech republic denmark dijibouti dominican 
	// republic netherlands east caribbean ecuador egypt el salvador estonia ethiopia euro falkland 
	// islands fiji finland france gambia germany ghana gibraltar greece guatemala guinea guyana 
	// haiti honduras hong kong hungary iceland india indonesia iraq ireland israel italy jamaica 
	// japan jordan kazakhstan kenya korea kuwait laos latvia lebanon lesotho liberia libya 
	// lithuania luxembourg macau macedonia malaga malawi kwacha malaysia maldives malta mauritania 
	// mauritius mexico moldova mongolia morocco mozambique myanmar namibia nepal new Zealand 
	// nicaragua nigeria north korea norway oman pakistan panama papua new guinea paraguay peru 
	// philippines poland portugal qatar romania russia samoa sao tome saudi arabia seychelles 
	// sierra leone singapore slovakia slovenia solomon islands somalia south africa spain sri lanka 
	// st helena sudan suriname swaziland sweden switzerland syria taiwan tanzania thailand tonga 
	// trinidad tunisia turkey united states us usa uae united arib emirates uganda ukraine 
	// uzbekistan vanuatu venezuela vietnam yemen yugoslavua zambia zimbabwe

	function uang($from,$to,$amount) {	
		$curval1 	= $from;
		$curval2 	= $to;
		$amount		= $amount;
		$country_arr	= array();

		// require_once('currency/nusoap.php');

		$params1 = array(
			'country1' 		=> $from, 
			'country2' 		=> $to
		);	

		// need to read in valid list of countries... :(
		// list is long so i stuck it in a file

                $counter=0;
                $fd = fopen("/disk3/bioh/bot/subtility/currency/valid_countries.txt","r");
                while ($country_in = fscanf ($fd, "%s\n"))
                        {
                        $country_arr[$counter] = trim($country_in[0]);
                        $counter++;
                        // print ($country_in[0]);
                        }

		// have to get underscores back for countries with spaces :(
		$from = str_replace(" ", "_", $from);
		$to = str_replace(" ", "_", $to);

		if (in_array($from, $country_arr) && in_array($to, $country_arr))
			{
			// $sslient 		= new soapclient('currency/Currency.wsdl','wsdl');
			$sslient = new soapclient('currency/CurrencyExchangeService.wsdl','wsdl');
			$result = $sslient->call('getRate',$params1);
		
			if ($result)
				{
				$hasil= ($amount * $result);
				return $hasil;
				}
			else
				{
				return 0;
				}
			}
		else
			{
			if (!in_array($from, $country_arr))
				{
				// invalid 'from' 
				$suggest_text = $from." country is invalid... suggestions : ";
				$first_letter_from = substr($from, 0, 1);
				for ($i=0; $i<count($country_arr); $i++)
					{
					if (substr($country_arr[$i], 0, 1) == $first_letter_from)
						{
						// print ("[DEBUG] $country_arr[$i] - $first_letter_from\n");
						$suggest_text = $suggest_text." ".$country_arr[$i];
						}
					}
				}
			else
				{
				// must be invalid 'to'
				$suggest_text = $to." country ... suggestions : ";
				$first_letter_to = substr($to, 0, 1);
                                for ($i=0; $i<count($country_arr); $i++)
                                        {
                                        if (substr($country_arr[$i], 0, 1) == $first_letter_to)
                                                {
                                                $suggest_text = $suggest_text." ".$country_arr[$i];
                                                }
                                        }

				}
			return $suggest_text;
			}
		}
}
?>
