<?php 
//change the values with your own hosting setting
 $mysql_host = "localhost";
 $mysql_database = "home_auto";
 $mysql_user = "root";
 $mysql_password = "root";

 $con = mysql_connect($mysql_host,$mysql_user,$mysql_password);
 mysql_connect($mysql_host,$mysql_user,$mysql_password);
 mysql_select_db($mysql_database);
?>