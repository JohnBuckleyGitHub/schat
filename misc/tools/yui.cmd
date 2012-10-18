@echo off
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.js -o ..\..\res\html\js\ChatView.min.js
java -jar yuicompressor.jar ..\..\res\html\src\PluginsView.js -o ..\..\res\html\js\PluginsView.min.js
java -jar yuicompressor.jar ..\..\res\html\src\server.js -o ..\..\res\html\js\server.min.js
java -jar yuicompressor.jar ..\..\res\html\src\user.js -o ..\..\res\html\js\user.min.js
java -jar yuicompressor.jar ..\..\res\html\src\about.js -o ..\..\res\html\js\about.min.js
java -jar yuicompressor.jar ..\..\res\html\src\bootstrap-tooltip.js -o ..\..\res\html\js\bootstrap-tooltip.min.js
java -jar yuicompressor.jar ..\..\res\html\src\bootstrap-modal.js -o ..\..\res\html\js\bootstrap-modal.min.js

java -jar yuicompressor.jar ..\..\res\html\src\about.css -o ..\..\res\html\css\about.min.css
java -jar yuicompressor.jar ..\..\res\html\src\PluginsView.css -o ..\..\res\html\css\PluginsView.min.css
java -jar yuicompressor.jar ..\..\res\html\src\ChatView.css -o ..\..\res\html\css\ChatView.min.css
java -jar yuicompressor.jar ..\..\res\html\src\server.css -o ..\..\res\html\css\server.min.css
java -jar yuicompressor.jar ..\..\res\html\src\user.css -o ..\..\res\html\css\user.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.js -o ..\..\src\common\plugins\RawFeeds\res\js\RawFeeds.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\KelpJSONView.js -o ..\..\src\common\plugins\RawFeeds\res\js\KelpJSONView.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\RawFeeds\res\src\RawFeeds.css -o ..\..\src\common\plugins\RawFeeds\res\css\RawFeeds.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\Profile\res\src\Profile.js -o ..\..\src\common\plugins\Profile\res\js\Profile.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\Profile\res\src\flags.css -o ..\..\src\common\plugins\Profile\res\css\flags.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\YouTube\res\src\YouTube.js -o ..\..\src\common\plugins\YouTube\res\js\YouTube.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\YouTube\res\src\YouTube.css -o ..\..\src\common\plugins\YouTube\res\css\YouTube.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\SendFile\res\src\SendFile.js -o ..\..\src\common\plugins\SendFile\res\js\SendFile.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\SendFile\res\src\SendFile.css -o ..\..\src\common\plugins\SendFile\res\css\SendFile.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\History\res\src\History.js -o ..\..\src\common\plugins\History\res\js\History.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\History\res\src\days.js -o ..\..\src\common\plugins\History\res\js\days.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\History\res\src\History.css -o ..\..\src\common\plugins\History\res\css\History.min.css

java -jar yuicompressor.jar ..\..\src\common\plugins\Console\res\html\src\console.js -o ..\..\src\common\plugins\Console\res\js\console.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\Console\res\html\src\console-login.js -o ..\..\src\common\plugins\Console\res\js\console-login.min.js
java -jar yuicompressor.jar ..\..\src\common\plugins\Console\res\html\src\console.css -o ..\..\src\common\plugins\Console\res\css\console.min.css
java -jar yuicompressor.jar ..\..\src\common\plugins\Console\res\html\src\bootstrap.css -o ..\..\src\common\plugins\Console\res\css\bootstrap.min.css
pause