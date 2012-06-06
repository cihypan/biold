<?php
function currency_fn(&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 8))) == "!convert") {
	if (strlen(trim($text)) < 9) 
		{
		$help_text = "Convert Currencies syntax : !convert <from> <to> [amt]      (amt defaults to 1)";
		$bot->privMsg($dest, $help_text);

		// $help_text = "[From/To Options] usa euro canada uk germany france japan italy switzerland algeria argentina australia austria bermuda brazil chile china denmark finland greece hong_kong india indonesia ireland israel jamaica jordan korea mexico pakistan philippines poland portugal russia saudi_arabia slovakia south_africa south_korea spain sudan weden";
		$help_text = "<from> and <to> can be a country or curency you wish to convert.";

		$bot->privMsg($dest, $help_text);
		}
	else
		{
		$convert_args = trim(substr($text, 9));
		$convert_args_array = explode(" ", $convert_args); 

		$convert_from = $convert_args_array[0];
		$convert_from = str_replace("_", " ", $convert_from);

		$convert_to = $convert_args_array[1];
		$convert_to = str_replace("_", " ", $convert_to);

		$convert_amt = $convert_args_array[2];

		if (!is_string($convert_from) || !is_string($convert_to)) {
			$help_text = "[Convert Currencies syntax] !convert <from> <to> [amt]";
			$bot->privMsg($dest, $help_text);

			return;
			}
		if (is_null ($convert_amt))
			{
			$convert_amt = 1;
			}

		$curr=new CURRENCYCONVERTER();
		if (strlen($convert_from) != 3 || (strcasecmp($convert_from, 'usa') == 0))
			{
			// echo "$convert_from :".strlen($convert_from)."\n";
			$convert_from_array=$curr->country_to_currency($convert_from);
			if (! $curr->check_currency_code($convert_from_array[0]))
				{
				$bot->privMsg($dest, "Currency $convert_from not found");
				return;
				}
			}
		else
			{
			if (! $curr->check_currency_code($convert_from))
				{
				$bot->privMsg($dest, "Currency $convert_from not found");
				return;
				}
			}
		if (strlen($convert_to) != 3 || (strcasecmp($convert_from, 'usa') == 0))
			{
			// echo "$convert_to :".strlen($convert_to)."\n";
			$convert_to_array=$curr->country_to_currency($convert_to);
			if (! $curr->check_currency_code($convert_to_array[0]))
				{
				$bot->privMsg($dest, "Currency $convert_to not found :(");
				return;
				}
			}
		else
			{
			if (! $curr->check_currency_code($convert_to))
				{
				$bot->privMsg($dest, "Currency $convert_to not found :(");
				return;
				}
			}

		if (!is_null ($convert_to_array[0]))
			{
			$convert_to = $convert_to_array[0];
			$convert_to_txt = $convert_to_array[1];
			}
		if (!is_null ($convert_from_array[0]))
			{
			$convert_from = $convert_from_array[0];
			$convert_from_txt = $convert_from_array[1];
			}

		// echo "$amt $from = ".$curr->convert($amt,$to,$from)." $to";
		$convert_total = $curr->convert($convert_amt, $convert_to, $convert_from);

		$bot->privMsg($dest, "$convert_amt $convert_from_txt $convert_from -> $convert_total $convert_to $convert_to_txt"); 
		}
	}
}
?>
