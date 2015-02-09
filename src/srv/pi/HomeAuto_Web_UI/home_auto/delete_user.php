<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "DELETE FROM  house_data WHERE User_Name='".trim($_POST['uname'])."'";
echo $sql;
$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
mysqli_close($con);
echo "Deleted";
?>

