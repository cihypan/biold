<?
// $zipcode="new york";
global $zipcode;
include("xoapWeather.php");
$zipcode="calgary";
$weather = new xoapWeather();
$cc = $weather->ccData();
echo $weather->ccDetails($cc)."\n";
echo $weather->error_text."\n";
echo $weather->first_zip."\n";

$zipcode=$weather->first_zip;

$weather2 = new xoapWeather();
$cc = $weather2->ccData();
echo $weather2->ccDetails($cc);
echo $weather2->error_text;

?>
