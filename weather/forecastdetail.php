<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>
<title>Detailed forecast</title>
</head>
<link rel="stylesheet" href="weather.css">
<body>
<?
include("xoapWeather.php");
$weather = new xoapWeather();
$forecast = $weather->forecastData();
$weather->detailforecast($forecast,$_GET['did']);
?>
</body>
</html>
