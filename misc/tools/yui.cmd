@echo off
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.js -o ..\..\res\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\res\html\src\server.js -o ..\..\res\html\js\server.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.js -o ..\..\res\html\js\about.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.css -o ..\..\res\html\css\about.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.css -o ..\..\res\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ui.css -o ..\..\res\html\css\ui.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.js -o ..\..\src\common\plugins\RawFeeds\res\js\RawFeeds.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\KelpJSONView.js -o ..\..\src\common\plugins\RawFeeds\res\js\KelpJSONView.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.css -o ..\..\src\common\plugins\RawFeeds\res\css\RawFeeds.min.css
pause