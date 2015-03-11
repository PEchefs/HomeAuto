<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1" />
    <meta name="description" content="" />
    <meta name="author" content="" />
    <!--[if IE]>
        <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
        <![endif]-->
    <title>PhotoElectricChefs</title>
	
    <link href="../../dist/css/bootstrap.css" rel="stylesheet" />
    <link href="assets/css/font-awesome.min.css" rel="stylesheet" />
    <script src="../../lib/js/jquery.js" type="text/javascript"></script>
</head>

<body>
	 
	<div> 
    <div class="container">
	
        <div class="row text-center pad-top ">
            <div class="col-md-12">
                <h2>Node set up</h2>
            </div>
        </div>
		
         <div class="row  pad-top">
                <div class="col-md-4 col-md-offset-4 col-sm-6 col-sm-offset-3 col-xs-10 col-xs-offset-1">
                    <div class="panel panel-default">
                        <div class="panel-heading">
							<strong> Please enter the new Node configuration details</strong>  
                        </div>
                        <div class="panel-body">
						<form role="form" method="post" enctype="multipart/form-data" action="updateNodeData.php"> <br/>
								<div class="form-group input-group">
									<span class="input-group-addon"><i class="fa fa-share-alt-square"  ></i></span>
									<input name="nodeid" type="text" class="form-control" placeholder="Node ID" />
								</div>
								<div class="form-group input-group">
										<span class="input-group-addon"><i class="fa fa-share-alt"  ></i></span>
										<input name="rfid" type="text" class="form-control" placeholder="RF ID" />
								</div>
								<div class="form-group input-group">
									<span class="input-group-addon"><i class="fa fa-header"  ></i></span>
									<!--<span class="input-group-addon">@</span>-->
									<input  name="nodename"  type="text" class="form-control" placeholder="Name" />
								</div>
								<div class="form-group input-group">
									<span class="input-group-addon"><i class="fa fa-circle-thin"  ></i></span>
									<input  name="nodestatus" type="text" class="form-control" placeholder="Status" />
								</div>
                                <div class="form-group input-group">
                                            <span class="input-group-addon"><i class="fa fa-lock"  ></i></span>
                                            <input name="masterid" type="text" class="form-control" placeholder="Master ID" />
								</div>
								<div class="form-group input-group">
									<input type="file" name="fileToUpload" id="fileToUpload">
    
									
								</div>
								<div>
                                    <input type="submit" value="Upload Image" name="submit">
                                    </div>
									<hr /> 
									
                            </form>

									
                        </div>
                           
                    </div>
                </div>
                
                
        </div>
    </div>
	
        <div class="row text-center pad-top "> 
		<form action="../../samples/starter-template.php">
		<input type="submit" value="Go to Home" onclick="" class="btn btn-success"/> 
		</form>
		</div> 

    
    
   
</body>
</html>
