@echo off
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.js -o ..\..\res\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\res\html\src\server.js -o ..\..\res\html\js\server.min.js
java -jar yuicompressor.jar ..\..\res\html\src\user.js -o ..\..\res\html\js\user.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.js -o ..\..\res\html\js\about.min.js
java -jar yuicompressor.jar ..\..\res\html\src\bootstrap-tooltip.js -o ..\..\res\html\js\bootstrap-tooltip.min.js

java -jar yuicompressor.jar ..\..\res\html\src\about.css -o ..\..\res\html\css\about.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.css -o ..\..\res\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\res\html\src\server.css -o ..\..\res\html\css\server.min.css
java -jar yuicompressor.jar ..\..\res\html\src\user.css -o ..\..\res\html\css\user.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ui.css -o ..\..\res\html\css\ui.min.css
java -jar yuicompressor.jar ..\..\res\html\src\flags.css -o ..\..\res\html\css\flags.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.js -o ..\..\src\common\plugins\RawFeeds\res\js\RawFeeds.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\KelpJSONView.js -o ..\..\src\common\plugins\RawFeeds\res\js\KelpJSONView.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.css -o ..\..\src\common\plugins\RawFeeds\res\css\RawFeeds.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\Profile\res\src\Profile.js -o ..\..\src\common\plugins\Profile\res\js\Profile.min.js
pause