#/bin/sh
openssl req -x509 -newkey rsa:1024 -keyout server.key -nodes -days 10592 -out server.crt
