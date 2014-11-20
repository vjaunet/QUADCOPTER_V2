<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8" />
        <link rel="stylesheet" href="quadpilot.css" />
        <title>QuadPilot</title>
    </head>

    <body>

      <script src="jquery.js">   </script>
      <script src="tabchanging.js"></script>
      <script src="script_min.js"></script>


      <div class="tabs">
	<ul class="tab-links">
          <li class="active"><a href="#tab1">Live</a></li>
          <li><a href="#tab2">PID</a></li>
          <li><a href="#tab3">System</a></li>
          <li><a href="#tab4">Options</a></li>
	</ul>

	<div class="tab-content">
          <div id="tab1" class="tab active">

	    <!-- <?php include("live-view.php"); ?> -->

          </div>

          <div id="tab2" class="tab">

	    <?php include("pid.php"); ?>

          </div>

          <div id="tab3" class="tab">
	    toto
          </div>

          <div id="tab4" class="tab">
	    toto2
          </div>
	</div>
      </div>
    </body>
</html>
