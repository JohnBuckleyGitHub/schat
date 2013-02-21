<?php

class Download
{
  public $version  = '';
  public $url      = 'http://impomezia.s3.amazonaws.com/schat';
  public $path     = '/var/www/download.schat.me/htdocs/schat2';
  public $fileName = '/var/www/schat/htdocs/json/download.json';

  private $json;

  /**
   * @param array $options Опции для инициализации данных.
   */
  public function __construct($options = array())
  {
    if (array_key_exists('version', $options))
      $this->version = $options['version'];
  }

  /**
   * @return bool
   */
  public function exec()
  {
    $this->json->windows->name      = 'Windows';
    $this->json->windows->version   = $this->version;
    $this->fileInfo($this->json, 'windows', 'schat2-' . $this->version . '.exe');
    $this->fileInfo($this->json->windows, 'server', 'schat2-server-' . $this->version . '.exe');
    $this->fileInfo($this->json->windows, 'msi', 'schat2-' . $this->version . '.msi');

    $this->json->osx->name          = 'Mac OS X';
    $this->json->osx->version       = $this->version;
    $this->fileInfo($this->json, 'osx', 'SimpleChat2-' . $this->version . '.dmg');

    $this->json->sources->version   = $this->version;
    $this->fileInfo($this->json, 'sources', 'schat2-src-' . $this->version . '.tar.bz2');

    $this->json->linux->name        = 'Linux';
    $this->json->linux->version     = $this->version;
    $this->json->linux->size        = '';
    $this->json->linux->url         = 'download';

    $this->json->debian->name       = 'Debian GNU/Linux';
    $this->json->debian->version    = $this->version;
    $this->json->debian->size       = '';
    $this->json->debian->url        = 'download/debian';

    $this->json->ubuntu->name       = 'Ubuntu Linux';
    $this->json->ubuntu->version    = $this->version;
    $this->json->ubuntu->size       = '';
    $this->json->ubuntu->url        = 'download/ubuntu';

    $this->json->ubuntu->lucid->version = $this->version;
    $this->fileInfo($this->json->ubuntu->lucid, 'client',   'schat2_'  . $this->version . '-1~lucid_i386.deb');
    $this->fileInfo($this->json->ubuntu->lucid, 'client64', 'schat2_'  . $this->version . '-1~lucid_amd64.deb');
    $this->fileInfo($this->json->ubuntu->lucid, 'server',   'schatd2_' . $this->version . '-1~lucid_i386.deb');
    $this->fileInfo($this->json->ubuntu->lucid, 'server64', 'schatd2_' . $this->version . '-1~lucid_amd64.deb');

    $this->json->ubuntu->precise->version = $this->version;
    $this->fileInfo($this->json->ubuntu->precise, 'client',   'schat2_'  . $this->version . '-1~precise_i386.deb');
    $this->fileInfo($this->json->ubuntu->precise, 'client64', 'schat2_'  . $this->version . '-1~precise_amd64.deb');
    $this->fileInfo($this->json->ubuntu->precise, 'server',   'schatd2_' . $this->version . '-1~precise_i386.deb');
    $this->fileInfo($this->json->ubuntu->precise, 'server64', 'schatd2_' . $this->version . '-1~precise_amd64.deb');

    return $this->write();
  }


  /**
   * Запись JSON в файл.
   * @return bool
   */
  private function write()
  {
    $result = json_encode($this->json, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    if ($result === FALSE) {
      echo 'json encode failed' . json_last_error() . "\n";
      return FALSE;
    }

    if (file_put_contents($this->fileName, $result) === FALSE) {
      echo 'write ' . $this->fileName . " failed\n";
      return FALSE;
    }

    return TRUE;
  }


  function fileInfo($json, $key, $file)
  {
    $json->$key->size = $this->fileSize($file);
    $json->$key->url  = $this->url . '/' . $this->version . '/' .$file;
  }


  /**
   * @param $file
   * @return float Возвращает размер файла в мегабайтах с точностью до второго знака после запятой.
   */
  function fileSize($file)
  {
    $fileName = $this->path . '/' . $this->version . '/' . $file;
    if (!file_exists($fileName))
      return 0;

    $size = filesize($fileName) / 1024 / 1024;
    return round($size, 2);
  }
}

?>