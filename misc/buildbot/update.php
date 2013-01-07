#!/usr/bin/php
<?php

require_once('Update.class.php');

$options = getopt('', array('version:', 'revision:', 'channel:', 'os:'));
if (!array_key_exists('version', $options) || !array_key_exists('revision', $options))
  exit(1);

$channel = 'stable';
$os      = array('win32', 'osx', 'ubuntu');

if (array_key_exists('channel', $options))
  $channel = $options['channel'];

if (array_key_exists('os', $options))
  $os = explode(',', $options['os']);

foreach ($os as $value) {
  if ($value == 'win32') {
    $update = new UpdateWin32($options['version'], $options['revision'], $channel);
    if (!$update->exec())
      exit(2);
  }
  else if ($value == 'osx') {
    $update = new UpdateOSX($options['version'], $options['revision'], $channel);
    if (!$update->exec())
      exit(3);
  }
  else if ($value == 'ubuntu') {
    $update = new UpdateUbuntu($options['version'], $options['revision'], $channel);
    if (!$update->exec())
      exit(4);
  }
}

?>