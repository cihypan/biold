<?
class Dict {
        function lookup($term)
		{
		// $debug = 1;
                $ssclient = new soapclient('wsdl/dict.wsdl','wsdl');
		// $ssclient->debug_flag = true;

		// $result_strategy = $ssclient->call('StrategyList'); 

		// $err = $ssclient->getError();
		// if ($err) {
			// Display the error
		// 	echo 'Constructor error: ' . $err ;
			// At this point, you know the call that follows will fail
			// exit;
		//	}

		// print ("DEBUG [Class Dict(lookup($term))] : about to call 'Define' in dict.wsdl\n");
                $result = $ssclient->call('Define', "<Define xmlns=\"http://services.aonaware.com/webservices/\"><word>$term</word></Define>");

		// echo "\n--------------- DEBUG -----------------------\n";
		// echo $ssclient->debug_str;
		// echo "\n--------------- END DEBUG -------------------\n";

		$err = $ssclient->getError();
		if ($err) {
			// Display the error
			echo 'Constructor error: ' . $err ;
			// At this point, you know the call that follows will fail
			// exit;
			}

		// print ("DEBUG [Class Dict(lookup($term))] : contents of result variable\n");
		// print ("---------------------------------------------------------\n");
		// print_r ($result);
		// print ("---------------------------------------------------------\n");
		return $result;
        	}

	}
?>
