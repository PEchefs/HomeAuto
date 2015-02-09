<?php
$con=mysqli_connect("localhost","root","root","home_auto");
// Check connection
if (mysqli_connect_errno($con))
{
  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
}
$sql= "INSERT INTO command_data (Cmd_Id,Req_Id,Arg1,Arg2,Arg3,Arg4,User_Name,Status) VALUES ('";
        $sql=$sql.addslashes($_POST['cmd'])."','";
        $sql=$sql.addslashes($_POST['req'])."','";
        $sql=$sql.addslashes($_POST['arg1'])."','";
        $sql=$sql.addslashes($_POST['arg2'])."','";
        $sql=$sql.addslashes($_POST['arg3'])."','";
        $sql=$sql.addslashes($_POST['arg4'])."','";
        $sql=$sql.addslashes($_POST['uname'])."','";
        $sql=$sql."1"."'";
        $sql=$sql." )";
        $result=mysqli_query($con,$sql);
        if (!$result)
          {
                die('Error: ' . mysqli_error($con));
          }

$sql= "SELECT * FROM  command_data WHERE User_Name='".$_POST['uname']."' ";
$sql=$sql."AND Cmd_Id='".$_POST['cmd']."' ";
$sql=$sql."AND Req_Id='".$_POST['req']."' ";
$sql=$sql."AND Arg1='".$_POST['arg1']."' ";
$sql=$sql."AND Arg2='".$_POST['arg2']."' ";
$sql=$sql."AND Arg3='".$_POST['arg3']."' ";
$sql=$sql."AND Arg4='".$_POST['arg4']."' ";

$result=mysqli_query($con,$sql);
if (!$result)
   {
       die('Error: ' . mysqli_error($con));
   }
$row = mysqli_fetch_array($result,MYSQLI_BOTH);
echo "CmdId:".$row["Sl_No"];
mysqli_close($con);

?>

