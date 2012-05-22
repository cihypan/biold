<?
// 
// call like so : 
//        $d = new elizachat;
//        $eliza_text = $d -> say("I have a tumor");
//        echo $eliza_text;

class elizachat 
	{

        function say($say_text)
		{
		
                $params = array(
                        'Say'              => $say_text,
                );


                $sslient                = new soapclient('wsdl/eliza.wsdl','wsdl');

                $result                         = $sslient->call('Eliza',$params);

		return ($result);
        	}
	}
?>
