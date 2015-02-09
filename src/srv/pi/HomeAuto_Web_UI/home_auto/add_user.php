<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "REPLACE INTO house_data (Name,User_Name,Password,Picture,Email_Id,Type) VALUES ('";
        $sql=$sql.addslashes($_POST['name'])."','";
        $sql=$sql.addslashes($_POST['uname'])."','";
        $sql=$sql.$_POST['pass']."','";
        $sql=$sql.addslashes($_POST['profile'])."','";
        $sql=$sql.addslashes($_POST['email'])."','";
        $sql=$sql.$_POST['type']."'";
        $sql=$sql." )";
        $result=mysqli_query($con,$sql);
        if (!$result)
          {
                die('Error: ' . mysqli_error($con));
          }
mysqli_close($con);

echo "Submitted";
?>

