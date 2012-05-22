<?php

    class GeoLocation {
        // XML document
        var $doc  = null;
        // string describing the host of the geo location service
        var $host = "http://api.hostip.info/?ip=<IP>";
        
        // string describing the city
        var $city      = 'unknown';
        // string describing the country
        var $country   = 'unknown';
        // longitude
        var $longitude = '0';
        // latitude
        var $latitude  = '0';

	// blh
	var $xml_text = '';

        // ctor
        function GeoLocation($ip) {
            $this->doc = new DOMDocument();    
            $this->doc->preserveWhiteSpace = false;

            // prepare url of service
            $host  = str_replace( "<IP>", $ip, $this->host);
            $reply = $this->fetch($host);            

            // decode the reply and make it available
            $this->xml_text=$reply;
        }

        function fetch($host) {
            $reply = 'error';
            // try curl or fopen
            if( function_exists('curl_init') ) {
                // use curl too fetch site
                $ch = curl_init();
                curl_setopt($ch, CURLOPT_URL           , $host);
                curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
                $reply = curl_exec($ch);
                curl_close ($ch);
            } else {
                // fall back on fopen
                $reply = file_get_contents($host, 'r');    
            }
            return $reply;
        }

        function decode($text) {
	    return;
            $this->doc->loadXML($text);
            $xpath = new DOMXPath($this->doc);
            $entries = $xpath->query("//gml:name"); # query = "every gml:name element anywhere in the document"            

            $i = 1;
            foreach ($entries as $entry) {
                // first two gml:name entries are bogus
                if( $i++ < 2)
                    continue;
                
                // get the values and save them in the instance
                $this->city    = $entry->nodeValue;
                $this->country = $entry->nextSibling->nodeValue;
                $this->code    = $entry->nextSibling->nextSibling->nodeValue;

                $lnglat        = $entry->nextSibling->nextSibling->nextSibling->nextSibling->nodeValue;
                $lnglat        = split( ',', $lnglat);
                $this->longitude = $lnglat[0];
                $this->latitude  = $lnglat[1];
            }            
        }
    }

?> 
