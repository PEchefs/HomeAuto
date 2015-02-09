<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "SELECT * FROM  house_data WHERE User_Name='".trim($_POST['uname'])."'";

$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
$row = mysqli_fetch_array($result,MYSQLI_BOTH);
	echo $row["Name"].",".$row["User_Name"].",".$row["Password"].","
		.$row["Email_Id"].",".$row["Type"].",".$row["Picture"];
mysqli_close($con);
?>

