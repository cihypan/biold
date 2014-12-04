<?php
/*
 * imdb.class.php
 * 
 * Copyright 2012 Tiago AndrÃ© <geral.t8bit@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */
 
 /*Usage:
  * $imdb=new imdb('hunger games');
  * $imdb->get();
  * Doing that you will recive an object with the movie information,
  * 
  * Aditional options
  *   * $imdb=new imdb(movie_name,year,id_imdb);
  *   * you can search by id, year or name.
  *   * or if you want you can set that parameters like this
  *   * $imbd->set('title','hunger_games');
  *   * $imbd->set('year','2012');
  *   * $imbd->set('id','12453635');
  * 
  *   * Get Values return a string with the value
  *   * $imbd->get('Title');
  *   * $imbd->get('Year');
  *   * $imbd->get('Rated');
  *   * $imbd->get('Released');
  *   * $imbd->get('Runtime');
  *   * $imbd->get('Genre');
  *   * $imbd->get('Director');
  *   * $imbd->get('Writer');
  *   * $imbd->get('Actores');
  *   * $imbd->get('Plot');
  *   * $imbd->get('Poster');
  *   * $imbd->get('imdbRating');
  *   * $imbd->get('imdbVotes');
  *   * $imbd->get('imdbID');
  */
class imdb
{
    /*private*/ var $id;
    /*private*/ var $title;
    /*private*/ var $year;
    /*private*/ var $data;
    
    function imdb /*__construct*/ ($title=null,$year=null,$id=null)
    {
        $this->title=$this->space($title);
        $this->year=$this->space($year);
        $this->id=$this->space($id);
        $this->search();

    }
    
    /*private*/ function space($string)
    {
        $string = str_replace(" ","%20",$string);
        return $string;
    }
    
    /*private*/ function search()
    {
        if($this->id!=null){$query .="i=".$this->id.'&';}
        if($this->title!=null){$query .="t=".$this->title.'&';}
        if($this->year!=null){$query .="i=".$this->year.'&';}
        $link=file_get_contents('http://www.imdbapi.com/?'.$query);
        $data=json_decode($link);
        $this->data=$data;

    }
    
    /*public*/ function set($id,$value)
    {
        $this->$id=$value;
        $this->search();
    }
    
    function get($what='all')
    {
        if($what=='all'){return $this->data;}
        else{return $this->data->$what;}
    }
}
?> 
