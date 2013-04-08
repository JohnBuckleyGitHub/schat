#!/usr/bin/php
<?php
require_once('Download.class.php');
$download = new Download(getopt('', array('version:')));
if (!$download->exec())
  exit(1);
?>