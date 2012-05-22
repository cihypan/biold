<?
$calc_text = urlencode('7 pounds in kilograms');
$url = 'http://www.google.ca/search?source=ig&hl=en&q='.$calc_text.'&btnG=Google+Search&meta=';

  // $fp = popen("wget -q --user=bioh --password=gupihuj -O /tmp/ecn.txt '$url' && cat /tmp/ecn.txt","r");
  // $fp = popen ("cat /tmp/ecn.txt","r");
  // need user-agent :(
  // $fp = fopen($url, "r");
  $fp = popen("wget -q -O - --user-agent='Gecko/2008052906 Firefox/3.0' '$url'","r");
  $result = '';
  if (!$fp) {
  	print "wtf";
	}

  while(!feof($fp)) {
	$result .= fread($fp, 1024);
  }
//  print "------\n";
//  print $result;
//  print "------\n";

//	<td nowrap><h2 class=r><font size=+1><b>1 kilometer = 0.621371192 miles</b></h2></td>
//      <td nowrap dir=ltr><h2 class=r><font size=+1><b>1516 days = 4.15067045 years</b></h2></td></tr><tr>
//	$calc_google_pattern ='/<div id=res class=med>(.*)<td nowrap dir=ltr><.*><b>(.*)<\/b><\/h2><\/td>/';
	$calc_google_pattern ='/<div id=res class=med role=main>(.*)<h2 class=r style=\"font-size:138\%\"><b>(.*)<\/b><\/h2><div style/';
	if (@preg_match ($calc_google_pattern, $result, $calc_array))
	{
		print_r($calc_array);
		print ($calc_array[2]);
	}

?>
