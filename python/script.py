import requests
import datetime
import sys
import matplotlib.pyplot as plt
import numpy as np

GOOGLE_API_YOUTUBE = "https://www.googleapis.com/youtube/v3/"

def column(matrix, i):
	return [row[i] for row in matrix]

class Video(object):
    def __init__(self, id, api):
        url = GOOGLE_API_YOUTUBE + "videos?id=%s&part=snippet,statistics&key=%s" % (id, api)
        result = requests.get(url)

        if result.status_code != 200:
            print("Failed initializing video with id: %s\n" % id)
            print("Request returned %d\n" % result.status_code)
        else:
            jsonDict = result.json()
            # self.title = jsonDict["items"][0]["snippet"]["title"]
            self.views = int(jsonDict["items"][0]["statistics"]["viewCount"])
            ymd: list = jsonDict["items"][0]["snippet"]["publishedAt"][:10].split("-")
            self.uploadDate = datetime.date(int(ymd[0]), int(ymd[1]), int(ymd[2]))

class Playlist(object):
    def __init__(self, id, api):
        result = requests.get(GOOGLE_API_YOUTUBE + "playlists?id=%s&part=snippet&key=%s" % (id, api))
        if result.status_code != 200:
            print("Failed initializing playlist with id: %s\n" % id)
            print("Request returned %d\n" % result.status_code)
            return
        else:
            jsonDict = result.json()
            self.title = jsonDict["items"][0]["snippet"]["title"]
            self.channel = jsonDict["items"][0]["snippet"]["channelTitle"]
        
        url = GOOGLE_API_YOUTUBE + "playlistItems?playlistId=%s&part=contentDetails&key=%s&maxResult=50" % (id, api)
        result = requests.get(url)
        if result.status_code == 200:
            jsonDict = result.json()
            self.videos = []

            while True:
                for vidDict in jsonDict["items"]:  # list
                    self.videos.append(Video(vidDict["contentDetails"]["videoId"], api))
                if "nextPageToken" in jsonDict:
                    nextPageURL = url + "&pageToken=" + jsonDict["nextPageToken"]
                    jsonDict = requests.get(nextPageURL).json()
                else:
                    break

def main():
    if len(sys.argv) != 3:
        print("Usage: python %s <Playlist ID> <API Key>" % __file__)
        return 1

    playlistID = sys.argv[1]
    API_Key = sys.argv[2]

    playlist = Playlist(playlistID, API_Key)

    dataList = []
    n = 1
    for video in playlist.videos:
        viewsPerWeek = video.views / (int(str(datetime.date.today() - video.uploadDate).split(" ")[0]) / 7)
        dataList.append((n, video.views, viewsPerWeek))
        n += 1

    indicesList = column(dataList, 0)
    viewsList = column(dataList, 1)
    viewsPerWeekList = column(dataList, 2)

    title = "View analysis of [%s] by [%s] on %s" % (playlist.title, playlist.channel, datetime.date.today()) 
    print(title)
    print("index\tviews\tviews/wk")
    for data in dataList:
        print("%2d\t%d\t%5.2f" % (data[0], data[1], data[2]))

    plt.subplot(211)
    plt.title(title)
    plt.plot(indicesList, viewsList, "r", label = "Total Views")
    plt.legend()

    plt.subplot(212)
    plt.plot(indicesList, viewsPerWeekList, label = "Views Per Week")
    plt.legend()
    plt.show()

    return 0

if __name__ == "__main__":
    sys.exit(main())
