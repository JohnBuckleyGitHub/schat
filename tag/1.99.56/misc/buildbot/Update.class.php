<?php

class Update
{
  public $channel  = 'stable';
  public $version  = '';
  public $revision = 0;
  public $os       = '';
  public $path     = '/var/www/download.schat.me/htdocs/schat2/snapshots';
  public $json     = '/var/www/download.schat.me/htdocs/schat2/update.json';

  /**
   * @param $version
   * @param $revision
   * @param $channel
   */
  public function __construct($version, $revision, $channel) {
    $this->version  = $version;
    $this->revision = $revision;
    $this->channel  = $channel;
  }


  /**
   * Запись информации об обновлении.
   *
   * @return bool
   */
  public function exec() {
    $file = $this->file();
    if ($file != '' && !file_exists($file))
      return FALSE;

    @$json = json_decode(file_get_contents($this->json));

    $json->{$this->channel}->{$this->os}->version  = $this->version;
    $json->{$this->channel}->{$this->os}->revision = intval($this->revision);

    if ($file != '') {
      $json->{$this->channel}->{$this->os}->file   = $this->url();
      $json->{$this->channel}->{$this->os}->size   = filesize($file);
      $json->{$this->channel}->{$this->os}->hash   = sha1_file($file);
    }

    $page = $this->page();
    if ($page != '')
      $json->{$this->channel}->{$this->os}->page   = $page;

    return $this->write($json);
  }


  /**
   * Запись JSON в файл.
   *
   * @param $json Объект для записи в файл.
   * @return bool
   */
  protected final function write($json)
  {
    $result = json_encode($json, JSON_PRETTY_PRINT | JSON_UNESCAPED_SLASHES);
    if ($result === FALSE) {
      echo 'json encode failed' . json_last_error() . "\n";
      return FALSE;
    }

    if (file_put_contents($this->json, $result) === FALSE) {
      echo 'write ' . $this->json . " failed\n";
      return FALSE;
    }

    return TRUE;
  }


  /**
   * @return string Возвращает локальное имя файла обновления.
   */
  protected function file() { return ''; }


  /**
   * @return string Возвращает публичный адрес файла обновления.
   */
  protected function url() { return ''; }


  /**
   * @return string Адрес страницы для загрузки обновления.
   */
  protected function page() { return ''; }
}


class UpdateWin32 extends Update
{
  public function __construct($version, $revision, $channel) {
    parent::__construct($version, $revision, $channel);
    $this->os = 'win32';
  }


  protected function file() {
    return $this->path . '/' . $this->version . '/r' . $this->revision . '/schat2-' . $this->version . '-dev.exe';
  }


  protected function url() {
    if ($this->channel == 'stable')
      return 'http://impomezia.s3.amazonaws.com/schat/' . $this->version . '/schat2-' . $this->version . '.exe';
    else
      return 'http://download.schat.me/schat2/snapshots/' . $this->version . '/r' . $this->revision . '/schat2-' . $this->version . '-dev.exe';
  }


  protected function page() {
    return 'https://schat.me/download/windows';
  }
}


class UpdateOSX extends Update
{
  public function __construct($version, $revision, $channel) {
    parent::__construct($version, $revision, $channel);
    $this->os = 'osx';
  }


  protected function file() {
    return $this->path . '/' . $this->version . '/r' . $this->revision . '/SimpleChat2-' . $this->version . '-dev.dmg';
  }


  protected function url() {
    return 'http://impomezia.s3.amazonaws.com/schat/' . $this->version . '/SimpleChat2-' . $this->version . '.dmg';
  }
}


class UpdateUbuntu extends Update
{
  public function __construct($version, $revision, $channel) {
    parent::__construct($version, $revision, $channel);
    $this->os = 'ubuntu';
  }


  protected function page() {
    return 'https://schat.me/download/ubuntu';
  }
}

?>