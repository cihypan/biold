<?php

/* vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4: */

/**
* class.imdb_parser.php (php4)
* Parse information of any film - need a fetched imdb.com video-detail-site
* @author       Bernd Essl <bernd_at_b23.at>
* @copyright    Bernd Essl <bernd_at_b23.at>
* @license      license   http://gnu.org/copyleft/gpl.html GNU GPL
* @link         http://b23.at/
* @version      $LastChangedRevision: 19 $
*/


class IMDB_Parser
{

	
    function getMovieActors($imdb_website, $name_and_id=False)
    {
        if (preg_match('/<table class="cast">(.+)<\/table>/', $imdb_website, $hit))
        {
            if ($name_and_id === True)
            {
                #name and id
                if (preg_match_all('/<a href="\/name\/(nm\d{1,8})\/">([^<]+)<\/a>/',$hit[0],$results, PREG_PATTERN_ORDER))
                {
                    return $results;
                }    
                else
                {
                    return False;
                }
            }
            else
            {
                #only name, old version
                if (preg_match_all('/<a href="\/name\/nm\d{1,8}\/">([^<]+)<\/a>/',$hit[0],$results, PREG_PATTERN_ORDER))
                {
                    return $results[1];
                }
                else
                {
                    return False;
                }
            }
        }
        else
        {
            return False;
        }
    }
	

    function getMovieDirectedBy($imdb_website, $all_directors=False)
    {
        if ($all_directors === True)
        {
            return False;
        }
        else
        {
            if (preg_match('/Directors?:<\/h5>.?<a href="\/name\/[a-z0-9]+\/">(.+)<\/a>/sU', $imdb_website, $hit)) 
            {
                return $hit[1];
            }
            else
            {
                return False;
            }
        }
    }
    
    
    function getMovieColor($imdb_website)
    {
        if (preg_match('/<a href="\/List\?color-info.+>([^<]+)<\/a>/i',$imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieCountry($imdb_website)
    {
        if (preg_match_all('/<a href="\/Sections\/Countries\/([a-z]+)\/">/i',$imdb_website,$results, PREG_PATTERN_ORDER))
        {
            return $results[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieLanguage($imdb_website)
    {
        if (preg_match_all('/<a href="\/Sections\/Languages\/([a-z]+)\/">/i',$imdb_website,$results, PREG_PATTERN_ORDER))
        {
            return $results[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieRating($imdb_website)
    { 
        if (preg_match('/<b>([0-9]{1,2}\.[0-9]{1,2}\/10)<\/b>/', $imdb_website, $hit)) 
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieGenres($imdb_website)
    {
        if (preg_match_all('/\/Sections\/Genres\/(.+?)\//', $imdb_website, $hit, PREG_PATTERN_ORDER)) 
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMoviePlot($imdb_website)
    {
        if (preg_match('/Plot:<\/h5>([^<]+)</', $imdb_website, $hit))        
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieTagline($imdb_website)
    {
        if (preg_match('/Tagline:<\/h5>([^<]+)</', $imdb_website, $hit))        
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMovieTitle($imdb_website)
    {
        if (preg_match('/\<title\>([^"]+)\<\/title\>/', $imdb_website, $hit)) 
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }
    
    
    function getMoviePictureHtml($imdb_website)
    {
        if (preg_match('/<a name="poster".+title=".+">(.+)<\/a>/', $imdb_website, $hit)) 
        {
            return str_replace('ec.', '', $hit[1]);
        }
        else
        {
            return False;
        }
    }
    
    
    function getMoviePicture($imdb_website)
    {
        if (preg_match('/ src="([^"]+)/', $this->getMoviePictureHtml($imdb_website), $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieYear($imdb_website)
    {
        if (preg_match('/\/Sections\/Years\/([^"]+)/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }

    
    function getMovieRuntime($imdb_website)
    {
        #contributed by Guvenc KAPLAN
        if (preg_match('/Runtime:<\/h5>([^<]+)<\/div>/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }

    
    function getMovieStars($imdb_website)
    {
        if (preg_match('/<b>([0-9]{1,2}\.[0-9]{1,2})\/10<\/b>/', $imdb_website, $hit)) 
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieAka($imdb_website)
    {
        if (preg_match('/Also Known As:<\/h5>([^<]+)<br>/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieAwards($imdb_website)
    {
        #contributed by D3FiANC3 
        if (preg_match('/Awards:<\/h5>([^<]+)<a class/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieVotes($imdb_website)
    {
        #contributed by D3FiANC3 
        if (preg_match('/\<small\>\(\<a href="ratings"\>([^<]+)\<\/a\>\)\<\/small\>/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieMPAA($imdb_website)
    {
        #contributed by Edward Husar         
        if (preg_match('/MPAA<\/a>:<\/h5>([^<]+)<\/div>/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;

        }
    }


    function getMovieAspectRatio($imdb_website)
    {
        if (preg_match('/<h5>Aspect Ratio:<\/h5>([^<]+)<a class/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;

        }
    }


    function getMovieSoundMix($imdb_website)
    {
        if (preg_match('/<a href="\/List\?sound-mix.+>([^<]+)<\/a>/i',$imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;

        }
    }


    function getMovieReleaseDate($imdb_website)
    {
        #contributed by Edward Husar
        if (preg_match('/Release Date:<\/h5>([^<]+)<a class/', $imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


    function getMovieCompany($imdb_website)
    {
        if (preg_match('/<a href="\/company.+>([^<]+)<\/a>/i',$imdb_website, $hit))
        {
            return $hit[1];
        }
        else
        {
            return False;
        }
    }


}  
?>
