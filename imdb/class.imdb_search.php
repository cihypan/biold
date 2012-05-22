<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
* class.imdb_search.php (php4)
* Fetch search results from the internet movie database - imdb.com
* @author       Bernd Essl <bernd_at_b23.at>
* @copyright    Bernd Essl <bernd_at_b23.at>
* @license      license   http://gnu.org/copyleft/gpl.html GNU GPL
* @version      SVN: 2
* @link         http://b23.at/
*/


class IMDB_Search
{

    var $imdb_searchform = 'http://imdb.com/find?q=';


    function cleanUpQuery($q)
    {
        return urlencode($q);
    }


    function searchTitles($q)
    {
        if ($this->cleanUpQuery($q))
        {
            $imdb_site_result = file_get_contents ($this->imdb_searchform.$this->cleanUpQuery($q));

            if (preg_match('/No Matches/i', $imdb_site_result))
            {
                //nothing found
                return false;                
            }
            else
            {
                if (preg_match_all('/<a href="\/title\/(tt\d{1,8})\/">([^<]+)<\/a>/',$imdb_site_result,$results, PREG_SET_ORDER ))
                {
                    return $results;
                }
            }
        }
        else
        {
            return false;
        }
    }

}  
?> 
