<?php
/**
* Author   : MA Razzaque Rupom (rupom_315@yahoo.com, rupom.bd@gmail.com)
* Version  : 1.0
* Date     : 02 March, 2006
* Purpose  : Creating Hierarchical Array from XML Data
* Released : Under GPL
*/

require_once "class.xmltoarray.php";

//XML Data
$xml_data = "
<result>
   <studentname>
      MA Razzaque
   </studentname>
   <institute>
      RUET
   </institute>
   <dept>
      CSE
   </dept>
   <roll>
      99315
   </roll>
   <class>
      First
   </class>
</result>";

//Creating Instance of the Class
$xmlObj    = new XmlToArray($xml_data);
//Creating Array
$arrayData = $xmlObj->createArray();

//Displaying the Array 
echo "<pre>";
print_r($arrayData);
echo "</pre>";
?>
