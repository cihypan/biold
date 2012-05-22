<html>
<head>
<title>Weather Current Conditions</title>
<link rel="stylesheet" href="weather.css">
</head>
<body>
<?
include("xoapWeather.php");
$weather = new xoapWeather();
$cc = $weather->ccData();
$weather->currentConditions($cc);
?>
</body>
</html>