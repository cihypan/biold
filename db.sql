-- MySQL dump 8.23
--
-- Host: 127.0.0.1    Database: biold
---------------------------------------------------------
-- Server version	3.23.58

--
-- Table structure for table `olded`
--

CREATE TABLE olded (
  id int(10) unsigned NOT NULL auto_increment,
  nick varchar(255) NOT NULL default '',
  datestmp datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (id),
  UNIQUE KEY id (id),
  KEY id_2 (id)
) TYPE=MyISAM;

--
-- Table structure for table `release_list`
--

CREATE TABLE release_list (
  id int(11) NOT NULL auto_increment,
  release varchar(255) NOT NULL default '',
  timestamp datetime NOT NULL default '0000-00-00 00:00:00',
  nick varchar(100) NOT NULL default '',
  PRIMARY KEY  (id),
  UNIQUE KEY id (id),
  KEY id_2 (id)
) TYPE=MyISAM;

--
-- Table structure for table `url_list`
--

CREATE TABLE url_list (
  id int(10) unsigned NOT NULL auto_increment,
  url varchar(255) NOT NULL default '',
  timestamp datetime default NULL,
  nick varchar(100) NOT NULL default '',
  PRIMARY KEY  (id),
  UNIQUE KEY id (id),
  KEY id_2 (id)
) TYPE=MyISAM;

