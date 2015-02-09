<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "SELECT User_Name FROM  house_data ";
$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
$row = mysqli_fetch_array($result,MYSQLI_BOTH);
echo $row[0];
while($row = mysqli_fetch_array($result,MYSQLI_BOTH))
{
	echo ",".$row[0];
}
mysqli_close($con);
?>

