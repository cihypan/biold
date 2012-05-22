<?php
function translate($params)
{
	if(!function_exists('curl_init'))
	{
		print ("need curl support :(");
		return;
	}
	if(isset($params[1]))
		{
		$buffer = '';
		$len = strlen($params[0]." ");
		$param = substr(implode(" ",$params), $len);
		$str = 'urltext='.urlencode($param).'&lp='.$params[0].'&submit=Translate';
		$ch = curl_init();
		curl_setopt($ch, CURLOPT_URL,"http://babelfish.altavista.com/babelfish/tr");
		curl_setopt($ch, CURLOPT_FAILONERROR, 1);
		curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
		curl_setopt($ch, CURLOPT_TIMEOUT, 3);
		curl_setopt ($ch, CURLOPT_HEADER, 0);
		curl_setopt($ch, CURLOPT_POST, 1);
		curl_setopt($ch, CURLOPT_POSTFIELDS, $str);
		ob_start();
		curl_exec ($ch);
		$buffer = ob_get_contents();
		ob_end_clean();
		curl_close ($ch);
		$start = strpos ($buffer, 'padding:10px')+14;
		$buffer = substr($buffer, $start);
		$end = strpos ($buffer, '</div>');
		$buffer = trim(substr($buffer, 0, $end));
		if(!empty($buffer))
			{
			$buffer = substr($buffer, 8);
    			return($buffer);
			}
		else
			{
		    	return('There was an error translating your text.');			
			}
		}
	else
		{
		return('Someting wong :(');
		}
}
?>
