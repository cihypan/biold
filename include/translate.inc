<?
function http_build_query($a,$b='',$c=0){
	if (!is_array($a)) return false;
	foreach ((array)$a as $k=>$v){
		if ($c) $k=$b."[".$k."]"; elseif (is_int($k)) $k=$b.$k;
		if (is_array($v)||is_object($v)) {$r[]=http_build_query($v,$k,1);continue;}
		$r[]=$k."=".urlencode($v);
	}
	return implode("&",$r);
}

function translate_fn(&$bot, $nick, $ident, $host, $dest, $text) {
  if ((strcasecmp($nick, "teebag") == 0)) 
  	{
   	return;
  	}
   if (trim(strToLower(substr($text, 0, 10))) == "!translate") {
	if (strlen(trim($text)) < 11) 
		{
		$bot->privMsg($dest, "Translate syntax: !translate [language_code_from] [language_code_to] [text]");
		$bot->privMsg($dest, "   Valid combinations : en de, en es, en fr, en it, en pt, en ar, en ja, en ko, en cn, de en, de fr, es en, fr en, fr de, it en, pt en, ar en, ja en, ko en, cn en");
		$bot->privMsg($dest, "   Languages : English [en], French [fr], Spanish [es], German [de], Italian [it], Portugese [pt], Arabic [ar], Japanese [ja], Korean [ko], Chinese (Simplified) [cn]");
		}
	else
		{
		$translate_code_from = trim(substr($text, 11, 2));
		$translate_code_to = trim(substr($text, 14, 2));
		$translate_text = trim(substr($text, 17));

		// special case : chinese
		if ($translate_code_from == "cn") {
			$translate_code_from = "zh-CN";
			}
		if ($translate_code_to == "cn") {
			$translate_code_to = "zh-CN";
			}

		$translate_code = $translate_code_from." ".$translate_code_to;

		$valid_translate_codes = array (
                "en de", "en es", "en fr", "en it", "en pt", "en ar", "en ja", "en ko", "en zh-CN",
                "de en", "de fr", 
		"es en", 
		"fr en", "fr de", 
		"it en", 
		"pt en", 
		"ar en", 
		"ja en", 
		"ko en",
		"zh-CN en"
	        );

		if (!in_array($translate_code, $valid_translate_codes))
			{
			$bot->privMsg($dest, "Translate syntax: !translate [language_code_from] [language_code_to] [text]");
			$bot->privMsg($dest, "   Valid combinations : en de, en es, en fr, en it, en pt, en ar, en ja, en ko, en cn, de en, de fr, es en, fr en, fr de, it en, pt en, ar en, ja en, ko en, cn en");
			$bot->privMsg($dest, "   Languages : English [en], French [fr], Spanish [es], German [de], Italian [it], Portugese [pt], Arabic [ar], Japanese [ja], Korean [ko], Chinese (Simplified) [cn]");
			return;
			}

		// old way... pretty lame imo
		// $translate_obj = new GoogleTranslate;
		// $translate_obj->setLanguageFrom($translate_code_from);
		// $translate_obj->setLanguageTo($translate_code_to);
		// $translate_obj->text=$translate_text.".";
		// $translated_text=$translate_obj->translate();
		// $translated_text=urldecode($translated_text);

		// new way
		//Create new object
		$translate = new GoogleTranslateApi;
                
		//How it works
		$translate->FromLang = $translate_code_from;
		$translate->ToLang = $translate_code_to;
		// echo $translate->translate('Hej jag heter Jamie!');
		// Would output; Hello my name is Jamie!
		$translate->translate($translate_text);

		// Any translation is also saved in TranslatedText
		// echo $translate->TranslatedText 
		
		$bot->privMsg($dest, $translate->TranslatedText);
		}
	}
}

function translate_fn_msg(&$bot, $nick, $ident, $host, $dest, $text) {
   if (trim(strToLower(substr($text, 0, 10))) == "!translate") {
	if (strlen(trim($text)) < 11) 
		{
		$bot->privMsg($nick, "Translate syntax: !translate [language_code_from] [language_code_to] [text]");
		$bot->privMsg($nick, "   Valid combinations : en de, en es, en fr, en it, en pt, en ar, en ja, en ko, en cn, de en, de fr, es en, fr en, fr de, it en, pt en, ar en, ja en, ko en, cn en");
		$bot->privMsg($nick, "   Languages : English [en], French [fr], Spanish [es], German [de], Italian [it], Portugese [pt], Arabic [ar], Japanese [ja], Korean [ko], Chinese (Simplified) [cn]");
		}
	else
		{
		$translate_code_from = trim(substr($text, 11, 2));
		$translate_code_to = trim(substr($text, 14, 2));
		$translate_text = trim(substr($text, 17));

		// special case : chinese
		if ($translate_code_from == "cn") {
			$translate_code_from = "zh-CN";
			}
		if ($translate_code_to == "cn") {
			$translate_code_to = "zh-CN";
			}

		$translate_code = $translate_code_from." ".$translate_code_to;

		$valid_translate_codes = array (
                "en de", "en es", "en fr", "en it", "en pt", "en ar", "en ja", "en ko", "en zh-CN",
                "de en", "de fr", 
		"es en", 
		"fr en", "fr de", 
		"it en", 
		"pt en", 
		"ar en", 
		"ja en", 
		"ko en",
		"zh-CN en"
	        );

		if (!in_array($translate_code, $valid_translate_codes))
			{
			$bot->privMsg($nick, "Translate syntax: !translate [language_code_from] [language_code_to] [text]");
			$bot->privMsg($nick, "   Valid combinations : en de, en es, en fr, en it, en pt, en ar, en ja, en ko, en cn, de en, de fr, es en, fr en, fr de, it en, pt en, ar en, ja en, ko en, cn en");
			$bot->privMsg($nick, "   Languages : English [en], French [fr], Spanish [es], German [de], Italian [it], Portugese [pt], Arabic [ar], Japanese [ja], Korean [ko], Chinese (Simplified) [cn]");
			return;
			}

		// returnLanguageTo() {
		// returnLanguageFrom() {
		// setLanguageTo($langT) {
		// setLanguageFrom($langF) {
		// setText($text) {
		// validateLang($langPair) {
		// translate() {
		// filterOutput($output) {
		// buildQuery() {

		$translate_obj = new GoogleTranslate;

		$translate_obj->setLanguageFrom($translate_code_from);
		$translate_obj->setLanguageTo($translate_code_to);
		$translate_obj->text=$translate_text.".";
		$translated_text=$translate_obj->translate();
		$translated_text=urldecode($translated_text);
		
		$bot->privMsg($nick, $translated_text);
		}
	}
}
?>
