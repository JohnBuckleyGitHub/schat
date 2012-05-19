var YouTubeVideoData = [];

function ShowYouTubePlayer(videoid, uuid) {
    var player = document.getElementById("YouTubePlayer_" + videoid + "_" + uuid);
    var text = document.getElementById("YouTubePlayerLabel_" + videoid + "_" + uuid);

	if(player.style.display == "block") {
        player.style.display = "none";
        text.innerHTML = "[YouTube: " + YouTubeVideoData[videoid] + "(Show)]";
  	}
	else {
		player.style.display = "block";
        text.innerHTML = "[YouTube: " + YouTubeVideoData[videoid] + "(Hide)]";
	}
} 

function GetYouTubeVideoName(videoid, uuid)
{
    var player = document.getElementById("YouTubePlayer_" + videoid + "_" + uuid);
    var text = document.getElementById("YouTubePlayerLabel_" + videoid + "_" + uuid);

    if(YouTubeVideoData[videoid] != 'undefined')
    {
        if(player.style.display == "block") {
            text.innerHTML = "[YouTube: " + YouTubeVideoData[videoid] + "(Hide)]";
        }
        else {
             text.innerHTML = "[YouTube: " + YouTubeVideoData[videoid] + "(Show)]";
        }
    }

    $.getJSON('http://gdata.youtube.com/feeds/api/videos/' + videoid + '?alt=json', function(data) {
            var title = data.entry["title"].$t;

            if(player.style.display == "block") {
                text.innerHTML = "[YouTube: " + title + "(Hide)]";
            }
            else {
                 text.innerHTML = "[YouTube: " + title + "(Show)]";
            }
            YouTubeVideoData[videoid] = title;
    });
}
