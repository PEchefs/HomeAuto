<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "SELECT * FROM  node_data ";
$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
while($row = mysqli_fetch_array($result,MYSQLI_BOTH))
{
	echo $row[1].",".$row[3].",".base64_encode($row[5]).";";
}
mysqli_close($con);
?>

