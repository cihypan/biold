<?
// lalal

class horoscope 
	{

        function get_all()
		{
                $sslient = new soapclient('wsdl/horoscope.wsdl','wsdl');

                $result = $sslient->call('GetHoroscope');

		return ($result);
        	}
	}
?>
