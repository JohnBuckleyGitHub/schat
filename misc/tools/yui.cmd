@echo off
java -jar yuicompressor.jar ..\..\data\html\src\ChatView.js -o ..\..\data\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\data\html\src\about.js -o ..\..\data\html\js\about.min.js
java -jar yuicompressor.jar ..\..\data\html\src\about.css -o ..\..\data\html\css\about.min.css
java -jar yuicompressor.jar ..\..\data\html\src\ChatView.css -o ..\..\data\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\data\html\src\ui.css -o ..\..\data\html\css\ui.min.css