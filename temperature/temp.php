<?
// blah blah, i know i can write a function to convert 
// celcius to fahrenheit, but this is more fun :) heh
//
class temperature 
	{

        function ctof($temp)
		{
		
                $params = array(
                        'temp'              => $temp,
                );


                $sslient = new soapclient('wsdl/temp.wsdl','wsdl');

                $result = $sslient->call('CtoF',$params);

		return ($result);
        	}

        function ftoc($temp)
                {

                $params = array(
                        'temp'              => $temp,
                );


                $sslient = new soapclient('wsdl/temp.wsdl','wsdl');

                $result = $sslient->call('FtoC',$params);

                return ($result);
                }
        }

?>
