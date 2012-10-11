#!/usr/bin/php
<?php
require_once('Update.class.php');
$update = new Update(getopt('', array('channel:', 'version:', 'revision:', 'os:')));
if (!$update->exec())
  exit(1);
?>