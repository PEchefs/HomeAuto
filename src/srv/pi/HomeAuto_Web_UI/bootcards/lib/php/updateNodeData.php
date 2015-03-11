<?php

echo $_FILES["fileToUpload"]["name"];
function diverse_array($vector) { 
    $result = array(); 
    foreach($vector as $key1 => $value1) 
        foreach($value1 as $key2 => $value2) 
            $result[$key2][$key1] = $value2; 
    return $result; 
} 

if(isset($_POST['submit'])){
	echo $_POST['nodeid'].'<br>';
	echo $_POST['rfid'].'<br>';
	echo $_POST['nodename'].'<br>';
	echo $_POST['nodestatus'].'<br>';
	echo $_POST['masterid'].'<br>';
	 
}
else {
        echo 'please uploaded';
    }

	$nodeid = trim($_POST['nodeid']);
	$rfid =  trim($_POST['rfid']);
	$nodename =  trim($_POST['nodename']);
	$nodestatus =  trim($_POST['nodestatus']);
	$masterid =  trim($_POST['masterid']);
	$photo =  addslashes(file_get_contents($_FILES['filename']['name']));
	
	if(!(strlen($nodeid)>0 && strlen($rfid)>0 && strlen($nodename)>0 && strlen($masterid)>0 )){
	echo "Invalid entry";
		header("Location: nodeSetUp.php");
		exit;
	}else{
		echo "1";
		$con=mysqli_connect("localhost","root","root","home_auto");
		// Check connection
		if (mysqli_connect_errno($con))
		{
		  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
		}
		echo "2";
		$sql= "INSERT INTO node_data( Id, Rf_Id, Name, Status, Picture, Master_Id) VALUES(".$nodeid.",". $rfid.",".$nodename."," .$nodestatus.",".$photo."," .$masterid.")";
		echo "3";
		$result=mysqli_query($con,$sql);
		echo "4";
		if (!$result)
		   {
			   die('Error in inserting the data: check teh syntax: ' . mysqli_error($con));
		   }
		echo "5";
		mysqli_close($con);
		echo "<script>alert(\"update successful\");</script>"; 
		//header("Location: register.php");
		exit;
	}
	
	
	
		?>