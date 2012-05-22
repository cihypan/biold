-- MySQL dump 10.11
--
-- Host: localhost    Database: oldbot
-- ------------------------------------------------------
-- Server version	5.0.51a-24+lenny2-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `brainteaser`
--

DROP TABLE IF EXISTS `brainteaser`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `brainteaser` (
  `teaser_id` tinyint(3) unsigned NOT NULL auto_increment,
  `teaser_question` text NOT NULL,
  `teaser_answer` tinytext NOT NULL,
  `temp_name` varchar(255) NOT NULL default '',
  UNIQUE KEY `teaser_id` (`teaser_id`)
) ENGINE=MyISAM AUTO_INCREMENT=248 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `currency`
--

DROP TABLE IF EXISTS `currency`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `currency` (
  `CURRENCY_ID` int(11) NOT NULL auto_increment,
  `CURRENCY_CODE` char(10) default '',
  `COUNTRY` varchar(255) default NULL,
  `CURRENCY` varchar(255) default NULL,
  PRIMARY KEY  (`CURRENCY_ID`)
) ENGINE=MyISAM AUTO_INCREMENT=311 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;


--
-- Table structure for table `do_not_old`
--

DROP TABLE IF EXISTS `do_not_old`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `do_not_old` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `url` varchar(1024) NOT NULL,
  `url_date` datetime NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;


--
-- Table structure for table `karma`
--

DROP TABLE IF EXISTS `karma`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `karma` (
  `karma_id` int(11) NOT NULL auto_increment,
  `karma_word` varchar(255) default NULL,
  `karma_value` int(11) default NULL,
  `karma_date` date default NULL,
  PRIMARY KEY  (`karma_id`)
) ENGINE=MyISAM AUTO_INCREMENT=17057 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `olded`
--

DROP TABLE IF EXISTS `olded`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `olded` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `nick` varchar(255) NOT NULL default '',
  `datestmp` datetime NOT NULL default '0000-00-00 00:00:00',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `id` (`id`),
  KEY `id_2` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=3329 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `questions`
--

DROP TABLE IF EXISTS `questions`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `questions` (
  `trivia_id` int(10) unsigned NOT NULL auto_increment,
  `trivia_category` varchar(255) NOT NULL default '',
  `trivia_question` varchar(255) NOT NULL default '',
  `trivia_answer` varchar(255) NOT NULL default '',
  UNIQUE KEY `trivia_id` (`trivia_id`),
  KEY `trivia_category` (`trivia_category`)
) ENGINE=MyISAM AUTO_INCREMENT=339411 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `quotes`
--

DROP TABLE IF EXISTS `quotes`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `quotes` (
  `quote_id` int(11) NOT NULL auto_increment,
  `quote_nick` varchar(100) default 'unknown',
  `quote_text` text,
  `quote_date` datetime default '0000-00-00 00:00:00',
  PRIMARY KEY  (`quote_id`)
) ENGINE=MyISAM AUTO_INCREMENT=990 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `release_list`
--

DROP TABLE IF EXISTS `release_list`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `release_list` (
  `id` int(11) NOT NULL auto_increment,
  `release` varchar(255) NOT NULL default '',
  `timestamp` datetime NOT NULL default '0000-00-00 00:00:00',
  `nick` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=9735 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `seen`
--

DROP TABLE IF EXISTS `seen`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `seen` (
  `seen_id` int(10) unsigned NOT NULL auto_increment,
  `seen_name` varchar(128) NOT NULL,
  `seen_date` datetime NOT NULL,
  `seen_ident` varchar(255) default NULL,
  `seen_saying` varchar(255) default NULL,
  PRIMARY KEY  (`seen_id`),
  KEY `seen_name` (`seen_name`)
) ENGINE=MyISAM AUTO_INCREMENT=860 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `stfu`
--

DROP TABLE IF EXISTS `stfu`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `stfu` (
  `stfu_id` int(10) unsigned NOT NULL auto_increment,
  `stfu_text` text NOT NULL,
  UNIQUE KEY `stfu_id` (`stfu_id`)
) ENGINE=MyISAM AUTO_INCREMENT=76 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `stock_players`
--

DROP TABLE IF EXISTS `stock_players`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `stock_players` (
  `player_id` int(11) NOT NULL auto_increment,
  `player_nick` varchar(255) NOT NULL,
  `player_cash` float NOT NULL,
  `player_last_activitydt` datetime NOT NULL,
  `player_last_resetdt` date NOT NULL,
  `player_host` varchar(255) NOT NULL,
  `player_trades` int(11) NOT NULL default '0',
  `player_stocks_traded` int(11) NOT NULL default '0',
  PRIMARY KEY  (`player_id`)
) ENGINE=MyISAM AUTO_INCREMENT=16 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;


--
-- Table structure for table `stock_portfolio`
--

DROP TABLE IF EXISTS `stock_portfolio`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `stock_portfolio` (
  `portfolio_id` int(11) NOT NULL auto_increment,
  `nick_id` int(11) NOT NULL,
  `stock_ticker` varchar(50) NOT NULL,
  `stocks_owned` int(11) NOT NULL,
  `stock_price` float NOT NULL,
  `timestamp` datetime NOT NULL,
  `exchange_rate` float NOT NULL,
  PRIMARY KEY  (`portfolio_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1693 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `teaser_current`
--

DROP TABLE IF EXISTS `teaser_current`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `teaser_current` (
  `last_id` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;


--
-- Table structure for table `trivia`
--

DROP TABLE IF EXISTS `trivia`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `trivia` (
  `last_id` int(10) unsigned NOT NULL default '0'
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;


--
-- Table structure for table `trivia_questions`
--

DROP TABLE IF EXISTS `trivia_questions`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `trivia_questions` (
  `question_id` int(10) unsigned NOT NULL auto_increment,
  `question_trivia_id` int(11) NOT NULL default '0',
  `question_nick` varchar(100) NOT NULL default '',
  `question_status` char(2) NOT NULL default '',
  UNIQUE KEY `question_id` (`question_id`),
  KEY `question_trivia_id` (`question_trivia_id`)
) ENGINE=MyISAM AUTO_INCREMENT=894 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `trivia_scores`
--

DROP TABLE IF EXISTS `trivia_scores`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `trivia_scores` (
  `score_id` int(10) unsigned NOT NULL auto_increment,
  `score_nick` varchar(100) NOT NULL default '',
  `score_val` int(11) NOT NULL default '0',
  UNIQUE KEY `score_id` (`score_id`),
  KEY `score_nick` (`score_nick`)
) ENGINE=MyISAM AUTO_INCREMENT=43 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `url_list`
--

DROP TABLE IF EXISTS `url_list`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `url_list` (
  `id` int(10) unsigned NOT NULL auto_increment,
  `url` varchar(255) NOT NULL default '',
  `timestamp` datetime default NULL,
  `nick` varchar(100) NOT NULL default '',
  PRIMARY KEY  (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=67420 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-08-11 12:23:30
