<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="google" value="notranslate">
 <!-- <meta http-equiv="refresh" content="5">-->

  <title>PhotoElectricChefs</title>

  <!-- Bootcards CSS files for desktop, iOS and Android -->
  <!-- You'll only need to load one of these (depending on the device you're using) in production -->
  <!-- (these include the required Bootstrap CSS) -->
  <!--<link href="../dist/css/bootcards-ios.min.css" rel="stylesheet">--> 
  <!--<link href="../dist/css/bootcards-desktop.min.css" rel="stylesheet"-->
  <link href="../dist/css/bootcards-android.min.css" rel="stylesheet">

  <!-- Font awesome -->
  <link href="//netdna.bootstrapcdn.com/font-awesome/4.1.0/css/font-awesome.min.css" rel="stylesheet" />
  
   <script src="../lib/js/jquery.js" type="text/javascript"></script>
  <script src="../lib/js/service.js" type="text/javascript"></script>
  
<style>
        .onoffswitch {
    position: relative; width: 90px;
    -webkit-user-select:none; -moz-user-select:none; -ms-user-select: none;
    }
    .onoffswitch-checkbox {
    display: none;
    }
    .onoffswitch-label {
    display: block; overflow: hidden; cursor: pointer;
    border: 2px solid #999999; border-radius: 20px;
    }
    .onoffswitch-inner {
    display: block; width: 200%; margin-left: -100%;
    -moz-transition: margin 0.3s ease-in 0s; -webkit-transition: margin 0.3s ease-in 0s;
    -o-transition: margin 0.3s ease-in 0s; transition: margin 0.3s ease-in 0s;
    }
    .onoffswitch-inner:before, .onoffswitch-inner:after {
    display: block; float: left; width: 50%; height: 30px; padding: 0; line-height: 30px;
    font-size: 14px; color: white; font-family: Trebuchet, Arial, sans-serif; font-weight: bold;
    -moz-box-sizing: border-box; -webkit-box-sizing: border-box; box-sizing: border-box;
    }
    .onoffswitch-inner:before {
    content: "ON";
    padding-left: 10px;
    background-color: #34A7C1; color: #FFFFFF;
    }
    .onoffswitch-inner:after {
    content: "OFF";
    padding-right: 10px;
    background-color: #EEEEEE; color: #999999;
    text-align: right;
    }
    .onoffswitch-switch {
    display: block; width: 18px; margin: 15px;
    background: #FFFFFF;
    border: 2px solid #999999; border-radius: 20px;
    position: absolute; top: 15px; bottom: 0; right: 88%;
    -moz-transition: all 0.3s ease-in 0s; -webkit-transition: all 0.3s ease-in 0s;
    -o-transition: all 0.3s ease-in 0s; transition: all 0.3s ease-in 0s;
    }
    .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-inner {
    margin-left: 0;
    }
    .onoffswitch-checkbox:checked + .onoffswitch-label .onoffswitch-switch {
    right: 0px;
    }
	</style>
</head>

<body>

  <!-- fixed top navbar -->
  <div class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="container">
      
      <!-- menu button to show/ hide the off canvas menu -->
      <button type="button" class="btn btn-default btn-menu pull-left" data-toggle="offcanvas">
        <i class="fa fa-lg fa-bars"></i>
        <span>Menu</span>
      </button>

      <a class="navbar-brand" title="PhotoElectricChefs" href="#">PhotoElectricChefs</a>  
      <!--right aligned button-->
      <button type="button" class="btn btn-warning navbar-right">
        <i class="fa fa-lg fa-refresh"></i>
        <span>Refresh</span>
      </button> 

      <!--navbar menu options: shown on desktop only -->
      <div class="navbar-collapse collapse">
        <ul class="nav navbar-nav">
          <li>
            <a href="#">
              <i class="fa fa-dashboard"></i>Dashboardx
            </a>
          </li>
          <li>
            <a href="#">
              <i class="fa fa-building-o"></i>Companies
            </a>
          </li>
          <li class="active">
            <a href="#">
              <i class="fa fa-font"></i>Contacts
            </a>
          </li>
        </ul>
      </div>          
    </div>
  </div>   

  <!-- slide in off canvas menu (mobile only) -->
  <nav class="navmenu offcanvas offcanvas-left">
    <ul class="nav">
	<li class="active">
        <a href="starter-template.php">
          <i class="fa fa-lg fa-lightbulb-o"></i>Rooms
        </a> 
      </li>
      <li>
        <a href="#">
          <i class="fa fa-lg fa-dashboard"></i>Node Setup
        </a>
      </li>
      <li class="active">
        <a href="starter-template.php">
          <i class="fa fa-lg fa-users"></i>User Setup
        </a>
      </li>
      <li>
        <a href="#">
          <i class="fa fa-lg fa-cog"></i>Server Setup
        </a>
      </li>
	  <li>
        <a href="#">
          <i class="fa fa-lg fa-list"></i>Register
        </a>
      </li>
	  <li>
        <a href="#">
          <i class="fa fa-lg fa-magic"></i>Feedback
        </a>
      </li>
    </ul>
  </nav><!--nav-->

  <div class="container bootcards-container push-right">

    <!-- This is where you come in... -->
    <!-- I've added some sample data below so you can get a feel for the required markup -->

    <div class="row">

      <!-- left list column -->
      <div class="col-sm-5 bootcards-list" id="list" data-title="Contacts">
        <div class="panel panel-default">       
          <div class="panel-body">
            <div class="search-form">
              <div class="row">
                <div class="col-xs-9">
                  <div class="form-group">
                    <input type="text" class="form-control" placeholder="Search Nodes...">
                    <i class="fa fa-search"></i>
                  </div>
                </div>
                <div class="col-xs-3">
                  <a class="btn btn-primary btn-block" href="#" onclick="bootcards.hideOffCanvasMenu()" >
                    <i class="fa fa-plus"></i> 
                    <span>Search</span>
                  </a>
                </div>
              </div>                
            </div>          
          </div><!--panel body-->

          <div class="list-group">

            <!-- list of sample data -->
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
					echo " <a class=\"list-group-item\" href=\"?node_id=".$row["Id"]."\">
						   <img src=\"data:image/jpeg;base64,".base64_encode( $row["Picture"])."\" class=\"img-rounded pull-left\">
						   <h4 class=\"list-group-item-heading\">".$row["Name"]."</h4>
						   <p class=\"list-group-item-text\">".$row["Status"]."</p>
						   </a>";
				}
				mysqli_close($con);
			?>
            
          </div><!--list-group-->

          <div class="panel-footer">
            <small class="pull-left">Built by PEC - List</small>
            <a class="btn btn-link btn-xs pull-right" href="#">
              Footer link</a>
          </div>	
        </div><!--panel-->

        </div><!--list-->

        <!--list details column-->
        <div class="col-sm-7 bootcards-cards hidden-xs" style="border:1px"

          <!--contact details -->
          <div id="nodeCard">

            <div class="panel panel-default">
              <div class="panel-heading clearfix">
                <h3 class="panel-title pull-left">Node Details</h3>
                <div class="btn-group pull-right visible-xs">
                  <a class="btn btn-primary" href="./starter-template-android.html" data-toggle="modal" data-target="#editModal">
                    <i class="fa fa-pencil"></i>
                    <span>Edit</span>
                  </a>
                </div>  
                <a class="btn btn-primary pull-right hidden-xs" href="#" data-toggle="modal" data-target="#editModal">
                  <i class="fa fa-pencil"></i>
                  <span>Edit</span>
                </a>
              </div>
              <div class="list-group">
				  <?php
					$con=mysqli_connect("localhost","root","root","home_auto");
					// Check connection
					if (mysqli_connect_errno($con))
					{
					  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
					}
					$sql= "SELECT * FROM  node_data ";						
					if(isset($_GET["node_id"]))
						$sql= $sql."WHERE Id='".$_GET["node_id"]."'";					
					$result=mysqli_query($con,$sql);
					if (!$result)
					   {
						   die('Error: ' . mysqli_error($con));
					   }
					$row = mysqli_fetch_array($result,MYSQLI_BOTH);
					echo " <div class=\"list-group-item\">
						   <img src=\"data:image/jpeg;base64,".base64_encode( $row["Picture"])."\" class=\"img-rounded pull-left\">						  <label>Name</label>
						   <h4 class=\"list-group-item-heading\">".$row["Name"]."</h4>
						   </div>";
					echo " <div class=\"list-group-item\">
						  <label>ID</label>
						  <h4 class=\"list-group-item-heading\">".$row["Id"]."</h4>
							</div>";
					echo " <div class=\"list-group-item\">
						  <label>RFID</label>
						  <h4 class=\"list-group-item-heading\">".$row["Rf_Id"]."</h4>
							</div>";
					echo " <div class=\"list-group-item\">
						  <label>Status</label>
						  <h4 class=\"list-group-item-heading\">".$row["Status"]."</h4>
							</div>";
					mysqli_close($con);
						?>
              </div>
				<div class="list-group">
				  <?php
				   function isChecked($String) {
						return ((strcasecmp($String,"ON")) == 0) ? 'checked' : '';
					}

					$con=mysqli_connect("localhost","root","root","home_auto");
					// Check connection
					if (mysqli_connect_errno($con))
					{
					  echo "Failed to connect to databse please try again later: " . mysqli_connect_error($con);
					}
					$sql= "SELECT * FROM  switch_data WHERE Node_Id='".$_GET["node_id"]."'";

					$result=mysqli_query($con,$sql);
					if (!$result)
					   {
						   die('Error: ' . mysqli_error($con));
					   }
					
					while($row = mysqli_fetch_array($result,MYSQLI_BOTH))
					{
					$str = isChecked($row["Status"]);
					echo " <div class=\"list-group-item\">
						   <img src=\"data:image/jpeg;base64,".base64_encode( $row["Picture"])."\" class=\"img-rounded pull-left\">	
							<label>".$row["Name"]."</label>
								<input type=\"checkbox\" name=".$row["Id"]." id=".$row["Name"]." class=\"onoffswitch-checkbox\" value=".$row["Node_Id"]." $str onchange = \"generateCommand(this)\">
							<label class=\"onoffswitch-label\" for=".$row["Name"].">
							<span class=\"onoffswitch-inner\"></span>
							<span class=\"onoffswitch-switch\"></span>
							</label>
						   <h4 class=\"list-group-item-heading\"></h4>
						   </div>";
					}
					
					mysqli_close($con);
					?>
              </div>
              <div class="panel-footer">
                <small class="pull-left">Built with Bootcards - Base Card</small>
                <a class="btn btn-link btn-xs pull-right" href="#">
                  Card footer link</a>
                </div>    
              </div>

            </div><!--contact card-->

            

        </div><!--list-details-->

    </div><!--row-->


  </div><!--container-->

  <!-- fixed tabbed footer -->
  <div class="navbar navbar-default navbar-fixed-bottom">

    <div class="container">

      <div class="bootcards-desktop-footer clearfix">
        <p class="pull-left">Bootcards</p>
      </div>

      <div class="btn-group">
        <a href="#" class="btn btn-default">
          <i class="fa fa-2x fa-dashboard"></i>Dashboard
        </a>
        <a href="starter-template.html" class="btn btn-default active">
          <i class="fa fa-2x fa-users"></i>Contacts
        </a>
        <a href="az-picker.html" class="btn btn-default">
          <i class="fa fa-2x fa-font"></i>AZ Picker
        </a>
      </div>
    </div>

  </div><!--footer-->

    <!-- Bootstrap & jQuery core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="//code.jquery.com/jquery-2.1.1.min.js"></script>
    <script src="//maxcdn.bootstrapcdn.com/bootstrap/3.2.0/js/bootstrap.min.js"></script>

    <!-- Bootcards JS -->
    <script src="../src/js/bootcards.js"></script>

    <!--recommended: FTLabs FastClick library-->
    <script src="//cdnjs.cloudflare.com/ajax/libs/fastclick/1.0.2/fastclick.min.js"></script>

    <script type="text/javascript">

      /*
       * Initialize Bootcards.
       * 
       * Parameters:
       * - offCanvasBackdrop (boolean): show a backdrop when the offcanvas is shown
       * - offCanvasHideOnMainClick (boolean): hide the offcanvas menu on clicking outside the off canvas
       * - enableTabletPortraitMode (boolean): enable single pane mode for tablets in portraitmode
       * - disableRubberBanding (boolean): disable the iOS rubber banding effect
       * - disableBreakoutSelector (boolean) : for iOS apps that are added to the home screen:
                            jQuery selector to target links for which a fix should be added to not
                            allow those links to break out of fullscreen mode.
       */
      bootcards.init( {
        offCanvasBackdrop : true,
        offCanvasHideOnMainClick : true,
        enableTabletPortraitMode : true,
        disableRubberBanding : true 
      });

      //enable FastClick
      window.addEventListener('load', function() {
          FastClick.attach(document.body);
      }, false);

    </script>
  </body>
</html>
