<?php

	echo $_POST['regName'].'<br>';
	echo $_POST['regMobile'].'<br>';
	echo $_POST['regEmail'].'<br>';
	
	$name = trim($_POST['regName']);
	$mobile =  trim($_POST['regMobile']);
	$email =  trim($_POST['regEmail']);
	
	if(!(strlen($name)>0)){
	echo "Name is empty";
		header("Location: register.php");
		$_SESSION['errorMsg'] = 'Please fill in the fields appropriately1';
		exit;
	}elseif(!(strlen($name)>0)){
	echo "Mobile is empty";
		header("Location: register.php");
		$_SESSION['errorMsg'] = 'Please fill in the fields appropriately2';
		exit;
	}elseif(!($mobile>0)){
	echo "email is empty";
		header("Location: register.php");
		$_SESSION['errorMsg'] = 'Please fill in the fields appropriately3';
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
		$sql= "INSERT INTO register VALUES('$name', $mobile,'$email')";
		echo "3";
		$result=mysqli_query($con,$sql);
		echo "4";
		if (!$result)
		   {
			   die('Error in inserting the data: check teh syntax: ' . mysqli_error($con));
		   }
		echo "5";
		mysqli_close($con);
		echo "<script>alert(\"Thank you for registering with PEC\");</script>"; 
		header("Location: register.php");
		exit;
	}
	
	
	
		?>