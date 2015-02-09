<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}

$sql= "SELECT * FROM  command_data WHERE User_Name='".$_POST['uname']."' ";
$sql=$sql."AND Sl_No=".$_POST['id']." ";

$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
$row = mysqli_fetch_array($result,MYSQLI_BOTH);
echo "Status:".$row["Status"];
mysqli_close($con);

?>

