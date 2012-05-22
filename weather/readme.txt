xoapWeather - Readme
===================

  xoapWeather - Process XML feeds from weather.com for display on a web site            
			    keeping with in weather.com's standards for caching.

  Version 1.1 - December 2003
  -------------------------
  http://www.spectre013.com
  
    Copyright (C) 2003  Brian Paulson <spectre013@spectre013.com>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Requirements:
    4.1.0 or >
	Subscription to The Weather Channel's XML feed Service (FREE @ http://www.weather.com)


  Summary:
   	xoapWeather was developed keeping in mind the standards of weather.com's Terms
	regarding Caching, links, and proper displaying of Information. 
	
  Download:
    You can get the newest version at http://www.spectre013.com/.

  Credits:
   	Thanks to Kris Zawadka [kris@h3x.com] for the XML Parsing Functions.
    

  Installation:
    1. Upload all the file to the Directory called weather
    2. Make the directory wxCache writable by your web server
    3. Subscribe to weather.com's XML Feed http://www.weather.com/services/oap.html - its FREE
    4. Fill out the variables at the top of the xoapWeather.php
	
  Upgrade: (Customised output)
  	1. If you have customized the output at all you will need to copy all your HTML output
	   into the new xoapWeather.php file and change all referances from forcast to forecast.
	2. Update all support pages (pages that call the functions) changing all referances from 
	   forcast to forecast.
	3. Fill out the variables at the top of the xoapWeather.php
	
  Upgrade: (non-Customised output)
  	1. replace all the pages with the new version. (due to spelling corrections in functin names)
	3. Fill out the variables at the top of the xoapWeather.php
	
    
  ChangeLog:
    Added the abbility to search for a location by city name.
	revised code that retrives the XML file to handle errors better.
	fixed spelling errors.


  Support:
    There is support forums located http://www.spectre013.com


    Enjoy!

