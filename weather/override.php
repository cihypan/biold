<?
##################################################################################################
#               ************************* WARNING ***********************************            #
#   Accessing this page too often can result in you violating the cache Rules set by             #
#   The Weather Channel and result in a Violation of thier terms.								 #
##################################################################################################

##################################################################################################
# Use this page to by pass the Caching restrictions of the main program. The main use is for when#
# an error has occured, correct the error and run this file to update the XML file with usefull  #
# data.																							 #
##################################################################################################
include("xoapWeather.php");
$weather = new xoapWeather();
if($_GET['zip'])
	{
	$weather->getXMLdata($_GET['zip'],'cc'); 
	$weather->getXMLdata($_GET['zip'],'forcast');
	}
	else
	{
	echo "No Zip Code";
	}
?>