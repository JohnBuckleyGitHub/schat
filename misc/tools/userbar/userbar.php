<?php
/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

userbar("/var/lib/schatd/stats.xml", "/usr/share/schatd/userbar/", "/var/www/userbar.png");

function userBarText($image, $x, $y, $fontfile, $text)
{
  $size = 6;
  $white = imageColorAllocate($image, 255, 255, 255);
  $black = imageColorAllocate($image, 0, 0, 0);
  
  imagettftext($image, $size, 0, $x + 1, $y, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x - 1, $y, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x, $y + 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x, $y - 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x + 1, $y + 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x + 1, $y - 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x - 1, $y + 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x - 1, $y - 1, $black, $fontfile, $text);
  imagettftext($image, $size, 0, $x, $y, $white, $fontfile, $text);
}

function userbar($statsFile, $basePath, $outFile)
{
  $image = imagecreatefrompng($basePath . "userbar-template.png");
  if (!$image)
    return;
 
  $count = 0;
  if (function_exists('simplexml_load_file')) {
    $xml = simplexml_load_file($statsFile);
    if ($xml) {
      $count = $xml->users;
      if ($count == "")
        $count = 0;
    }
  }
  else
    echo("SimpleXML not found");

  userBarText($image, 328, 12, $basePath . "hoog_mini.ttf", $count);
  imagepng($image, $outFile);
  imagedestroy($image);
}

?>