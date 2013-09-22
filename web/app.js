#!/usr/bin/env node

var express    = require('express');
var sockjs     = require('sockjs');
var http       = require('http');
var ChatServer = require('./app/net/server').ChatServer;
var core       = require('./app/core');

core.config = require('./config.json');
core.sockjs = sockjs.createServer({sockjs_url: '/js/sockjs-0.3.js'});
core.server = new ChatServer();
core.server.init();

core.express = express();
core.express.use(express.static(__dirname + '/public'));

core.express.get('*', function(req, res) {
  res.sendfile(__dirname + '/public/index.html');
});

var server = http.createServer(core.express);
core.sockjs.installHandlers(server, {prefix:'/chat'});

server.listen(9999);
