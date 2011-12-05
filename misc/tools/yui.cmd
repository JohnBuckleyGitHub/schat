@echo off
java -jar yuicompressor.jar ..\..\data\html\js\ChatView.js -o ..\..\data\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\data\html\js\about.js -o ..\..\data\html\js\about.min.js
java -jar yuicompressor.jar ..\..\data\html\css\about.css -o ..\..\data\html\css\about.min.css
java -jar yuicompressor.jar ..\..\data\html\css\ChatView.css -o ..\..\data\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\data\html\css\ui.css -o ..\..\data\html\css\ui.min.css