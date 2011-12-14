@echo off
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.js -o ..\..\res\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.js -o ..\..\res\html\js\about.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.css -o ..\..\res\html\css\about.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.css -o ..\..\res\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ui.css -o ..\..\res\html\css\ui.min.css