<?php

// +----------------------------------------------------------------------+

// Insert your google key here - obtainable from http://api.google.com
define("GOOGLE_KEY", "xxx");

if (!file_exists(dirname(__FILE__).'/google/nusoap/lib/nusoap.php')) {
  print ("i needs da soap, eh");
  exit();
}

require_once dirname(__FILE__).'/google/nusoap/lib/nusoap.php';

class Google {

  var $soap_client;

  function Google () {
    $this->soap_client = &new soapclient("http://api.google.com/search/beta2");
  }

  function google_lookup ($search_term) {
    
    if (strlen($search_term) > 0) {
      $params = array('key' => GOOGLE_KEY,
		      'q'   => $search_term,
		      'start' => 0,
		      'maxResults' => 1,
		      'filter' => false,
		      'restrict' => '',
		      'safeSearch' => false,
		      'lr' => '',
		      'ie' => '',
		      'oe' => '');

      $result = $this->soap_client->call("doGoogleSearch", $params, "urn:GoogleSearch", "urn:GoogleSearch");

      if ($result['estimatedTotalResultsCount'] == '0') {
	$out = "Sorry - No result(s)!";
      } else {
	$out = sprintf("\"%s\" :: %s (About %s results)",
		       strip_tags($result['resultElements'][0]['title']),
		       $result['resultElements'][0]['URL'],
		       number_format($result['estimatedTotalResultsCount'], 0, '.', ','));
      }
	
      return $out;

    }

  }

}
?>
