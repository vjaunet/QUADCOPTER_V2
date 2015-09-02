<!DOCTYPE html>
<html lang="en" class="no-js">
	<head>
		<meta charset="UTF-8" />
		<meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>QuadPilot</title>
		<meta name="description" content="Blueprint: Horizontal Drop-Down Menu" />
		<meta name="author" content="vjaunet" />

		<link rel="stylesheet" type="text/css" href="css/default.css" />
		<link rel="stylesheet" type="text/css" href="css/component.css" />
		<link rel="stylesheet" type="text/css" href="css/switch.css" />

		<script src="js/modernizr.custom.js"></script>
		<script src="js/cam_script_min.js"></script>
	</head>
	<body>
		<div class="container">
		  <header class="clearfix">
		    <h1>QuadPilot v2.0</h1>
		  </header>
		  <div class="main">
		    <nav id="cbp-hrmenu" class="cbp-hrmenu">
		      <ul>

			<li>
			  <a href="#">PID</a>
			  <div class="cbp-hrsub">

			    <div class="content">
			      <?php include("pid.php"); ?>
			    </div>

			  </div><!-- /cbp-hrsub -->
			</li>


			<li>
			  <a href="#">Live</a>
			  <div class="cbp-hrsub">

			    <div class="content">
			      <script>
			        // Auto-reload the page every N milliseconds
				setTimeout('init();', 2000);
			      </script>
			      <?php include("live-view.php"); ?>
			    </div>

			  </div><!-- /cbp-hrsub -->
			</li>


		      </ul>
		    </nav>
		  </div>
		</div>
		<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.9.1/jquery.min.js"></script>
		<script src="js/cbpHorizontalMenu.min.js"></script>
		<script>
			$(function() {
				cbpHorizontalMenu.init();
			});
		</script>
	</body>
</html>
