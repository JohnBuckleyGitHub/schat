<?php

class Update
{
  public $channel  = 'devel';
  public $version  = '';
  public $revision = 0;
  public $os       = 'win32';
  public $path     = '/var/www/download.schat.me/htdocs/schat2/snapshots';
  public $fileName = '/var/www/download.schat.me/htdocs/schat2/update.json';

  private $json;


  /**
   * @param array $options Опции для инициализации данных.
   */
  public function __construct($options = array())
  {
    if (array_key_exists('channel', $options))
      $this->channel = $options['channel'];

    if (array_key_exists('version', $options))
      $this->version = $options['version'];

    if (array_key_exists('revision', $options))
      $this->revision = $options['revision'];

    if (array_key_exists('os', $options))
      $this->os = $options['os'];
  }


  /**
   * Запись информации об обновлении.
   * @return bool
   */
  public function exec()
  {
    $file = $this->fileName();
    if (!file_exists($file))
      return FALSE;

    $this->json = json_decode(file_get_contents($this->fileName));

    $this->json->{$this->channel}->{$this->os}->version  = $this->version;
    $this->json->{$this->channel}->{$this->os}->revision = intval($this->revision);
    $this->json->{$this->channel}->{$this->os}->file     = $this->fileUrl();
    $this->json->{$this->channel}->{$this->os}->size     = filesize($file);
    $this->json->{$this->channel}->{$this->os}->hash     = sha1_file($file);

    return $this->write();
  }


  /**
   * Запись JSON в файл.
   * @return bool
   */
  private function write()
  {
    $result = json_encode($this->json);
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


  /**
   * @return string Возвращает локальное имя файла обновления.
   */
  private function fileName()
  {
    return $this->path . '/' . $this->version . '/r' . $this->revision . '/schat2-' . $this->version . '-dev.exe';
  }


  /**
   * @return string Возвращает публичный адрес файла обновления.
   */
  private function fileUrl()
  {
    return 'http://download.schat.me/schat2/snapshots/' . $this->version . '/r' . $this->revision . '/schat2-' . $this->version . '-dev.exe';
  }
}

?>