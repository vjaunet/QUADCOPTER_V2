<!DOCTYPE html>
<style>
.fit { /* set relative picture size */
    max-width: 100%;
    max-height: 100%;
  }
  .center {
    display: block;
    margin: auto;
  }
</style>

<!-- <div><img class="center fit" id="mjpeg_dest"></div> -->

<script type="text/javascript" language="JavaScript">

  //Set the image size depending on the widow size

  function set_body_height() { // set body height = window height
    var wh = $(window).height();
    $('body').height(wh);
  }
  $(document).ready(function() {
    set_body_height();
    $(window).bind('resize', function() { set_body_height(); });
  });

</script>
