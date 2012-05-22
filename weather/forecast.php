<?
include("xoapWeather.php");
$weather = new xoapWeather();
$forecast = $weather->forecastData();
echo $weather->extforecast($forecast);
?>
