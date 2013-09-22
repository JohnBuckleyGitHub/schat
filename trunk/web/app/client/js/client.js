(function() {
  'use strict';

  SockJS.prototype.send = function(data) {
    if (this.readyState === SockJS.CONNECTING)
      throw new Error('INVALID_STATE_ERR');
    if (this.readyState === SockJS.OPEN) {
      this._transport.doSend(JSON.stringify(data));
    }
    return true;
  };

  var Client = {
    url: '/chat',
    _pending: {},
    _debug: false,

    /**
     * Запуск подключения к серверу.
     *
     * @returns {boolean}
     */
    start: function() {
      if (this !== Client)
        Client.start.call(Client);

      this._debug = !!localStorage.getItem('debug') && console && console.log && console.log.apply;

      this.socket = new SockJS(this.url);
      this.socket.addEventListener('open',    this._open.bind(this));
      this.socket.addEventListener('close',   this._close.bind(this));
      this.socket.addEventListener('message', this._message.bind(this));

      return true;
    },


    /**
     * Отправка пакета.
     *
     * @param packet
     * @param callback Фунция вызываемая при получении ответа на запрос, получает 2 аргумента оригинальный запрос и ответ на него.
     * @returns {boolean}
     */
    send: function(packet, callback) {
      if (this !== Client)
        Client.send.call(Client, packet, callback);

      var array = schat.net.request(packet);
      if (array === null)
        return false;

      if (schat.utils.isFunction(callback))
        this._pending[packet.id] = [packet, callback];

      if (this._debug)
        console.log('Client.send:', array);

      this.socket.send(array);
      return true;
    },


    get: function(path, callback) {
      if (this !== Client)
        Client.send.call(Client, path, callback);

      var exist = schat.net.cache.get(path);
      if (exist !== null && !exist.dirty) {
        callback({id:'', date:exist.date, status:200, data:exist.data}, path);
        return true;
      }

      return this.send({request:path, date:(exist ? exist.date : 0)}, callback);
    },


    /**
     * Добавление слушателя событий.
     *
     * @param eventType
     * @param listener
     */
    on: function(eventType, listener) {
      this._listeners = this._listeners || {};
      var list = this._listeners[eventType] = this._listeners[eventType] || [];
      if (list.indexOf(listener) < 0)
        list.push(listener);
    },


    /**
     * Удаление слушателей события.
     *
     * @param eventType
     * @param listener
     */
    off: function(eventType, listener) {
      if (!this._listeners || !this._listeners[eventType])
        return;

      if (!listener) {
        delete this._listeners[eventType];
        return;
      }

      var list = this._listeners[eventType];
      var i    = list.length;

      while (i--) {
        if (listener !== list[i])
          continue;

        list.splice(i, 1);
      }
    },


    /**
     * Вызов слушателей события.
     *
     * @param eventType
     * @param event
     */
    emit: function(eventType, event) {
      if (!this._listeners || !this._listeners[eventType])
        return;

      var list = this._listeners[eventType];
      for (var i = 0; i < list.length; i++)
        list[i](event);
    },


    /**
     * Обработка входящего пакета.
     *
     * @param event
     */
    _message: function(event) {
      var reply = schat.net.reply(event.data);
      if (reply === null)
        return;

      if (this._debug)
        console.log('Client._message:', reply);

      if (this._pending.hasOwnProperty(reply.id)) {
        var req = this._pending[reply.id][0];
        if (req.method === 'GET') {
          if (reply.status === 304) {
            var exists = schat.net.cache.get(req.request);
            if (exists !== null)
              schat.net.cache.put(req.request, exists.data, reply.date);
          }
          else if (reply.status === 200)
            schat.net.cache.put(req.request, reply.data, reply.date);
        }

        this._pending[reply.id][1](reply, req.request);
        delete this._pending[reply.id];
      }

      this.emit('packet', reply);

      if (reply.type === 'RES') {
        if (reply.id === this.serverId + '/parent') {
          if (reply.data[0] === this.serverId) {
            schat.net.cache.open();
            this.emit('open');
          }
          else
            schat.net.cache.close();
        }
        else
          schat.net.cache.put(reply.id, reply.data, reply.date);

        reply.status = 200;
        this.emit('res.' + reply.id, reply);
      }
    },


    _close: function() {
      if (this._debug)
        console.log('Client._close:');

      schat.net.cache.close();
      this.emit('close');

      window.setTimeout(this.start.bind(this), 2000);
    },


    _open: function() {
      if (this._debug)
        console.log('Client._open:', this.socket.protocol);

      schat.net.cache.open();
      this.serverId = localStorage.getItem('serverId');

      if (typeof this.serverId !== 'string' || this.serverId.length !== 34) {
        this.send({request:'server/server'}, function(reply) {
          if (reply.status === 200) {
            this.serverId = reply.data[0].id;
            localStorage.setItem('serverId', this.serverId);
          }

          this.emit('open');
        }.bind(this));
      }
      else
        this.emit('open');
    }
  };

  window.schat.net.client = Client;
})();

