#!/usr/bin/php
<?php
$options = getopt('', array('channel:', 'version:', 'rev:', 'os:'));
if (!array_key_exists('channel', $options))
  exit(1);

if (!array_key_exists('version', $options))
  exit(1);

if (!array_key_exists('rev', $options))
  exit(1);

if (!array_key_exists('os', $options))
  exit(1);

if ($options['rev'] == 0)
  exit(1);

$json    = json_decode(file_get_contents('/var/www/download.schat.me/htdocs/schat2/update.json'));
$channel = $options['channel'];
$os      = $options['os'];

$basePath = 'schat2/snapshots/' . $options['version'] . '/r' . $options['rev'] . '/schat2-' . $options['version'] . '-dev.exe';
$json->$channel->version   = $options['version'];
$json->$channel->rev       = $options['rev'];
$json->$channel->$os->file = 'http://download.schat.me/' . $basePath;
$json->$channel->$os->size = filesize('/var/www/download.schat.me/htdocs/' . $basePath);
$json->$channel->$os->hash = sha1_file('/var/www/download.schat.me/htdocs/' . $basePath);

$result = json_encode($json);
if ($result === FALSE) {
  echo 'json encode failed' . json_last_error() . "\n";
  exit(1);
}

if (file_put_contents('/var/www/download.schat.me/htdocs/schat2/update.json', $result) === FALSE) {
  echo "write update.json failed\n";
  exit(1);
}
?>