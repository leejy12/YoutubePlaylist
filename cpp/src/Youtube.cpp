#include <sstream>
#include <string>
#include <iostream>
#include <ctime>
#include <nlohmann/json.hpp>
#include "Youtube.h"
#include "Http.h"

HttpClient YoutubeItem::http;

time_t DateToTime_t(const std::string& date)
{
	// date is in YYYY-MM-DDThh:mm:ss.sZ format
	std::tm tm = { 0 };
	tm.tm_year = std::stoi(date.substr(0, 4)) - 1900;
	tm.tm_mon  = std::stoi(date.substr(5, 2)) - 1;
	tm.tm_mday = std::stoi(date.substr(8, 2));
	tm.tm_hour = std::stoi(date.substr(11, 2));
	tm.tm_min  = std::stoi(date.substr(14, 2));
	tm.tm_sec  = std::stoi(date.substr(17, 2));
	
	return std::mktime(&tm);
}

void ShowProgressBar(int cur, int total)
{
	constexpr static int barLength = 50;
	float workDone = static_cast<float>(cur) / total;
	int barCount = static_cast<int>(std::round(workDone * barLength));
	std::cout << "\rIn progress... [";
	std::cout << std::string(barCount, '|');
	std::cout << std::string(barLength - barCount, ' ');
	std::cout << "]  (" << int(workDone * 100) << "%)" << std::flush;
}

Video::Video(const std::string& id, const std::string& apiKey) 
	: views(-1), uploadTime_t(-1)
{
	std::stringstream url;
	url << "https://www.googleapis.com/youtube/v3/"
		<< "videos?id=" << id
		<< "&part=snippet,statistics&key=" << apiKey;

	HttpResponse response = http.Get(url.str());

	if (response.httpStatus != 200)
	{
		std::cout << "Failed initializng video with id: " << id << '\n';
		std::cout << "Request returned " << response.httpStatus << '\n';
	}
	else
	{
		auto json = nlohmann::json::parse(response.contents.c_str());
		views = std::stoi(std::string(json["items"][0]["statistics"]["viewCount"]));
		std::string publishDate = json["items"][0]["snippet"]["publishedAt"];
		uploadTime_t = DateToTime_t(publishDate);
	}
}

Playlist::Playlist(const std::string& id, const std::string& apiKey)
{
	std::stringstream url;
	std::string nextUrl;
	url << "https://www.googleapis.com/youtube/v3/"
		<< "playlists?id=" << id
		<< "&part=snippet&key=" << apiKey;

	HttpResponse response = http.Get(url.str());

	if (response.httpStatus != 200)
	{
		std::cout << "Failed initializng playlist with id: " << id << '\n';
		std::cout << "Request returned " << response.httpStatus << '\n';
		return;
	}
	else
	{
		auto json = nlohmann::json::parse(response.contents.c_str());
		title = json["items"][0]["snippet"]["title"];
		channel = json["items"][0]["snippet"]["channelTitle"];
	}

	url.str("");
	url << "https://www.googleapis.com/youtube/v3/"
		<< "playlistItems?playlistId=" << id
		<< "&part=contentDetails&key=" << apiKey
		<< "&maxResults=50";

	response = http.Get(url.str());

	if (response.httpStatus == 200)
	{
		auto json = nlohmann::json::parse(response.contents.c_str());
		int numVideos = json["pageInfo"]["totalResults"];
		videos.reserve(numVideos);
		int cnt = 0;
		while (true)
		{
			for (const auto& videoDesc : json["items"])
			{
				videos.push_back(Video(videoDesc["contentDetails"]["videoId"], apiKey));
				ShowProgressBar(++cnt, numVideos);
			}
			if (json.contains("nextPageToken"))
			{
				nextUrl = url.str() + "&pageToken=" + std::string(json["nextPageToken"]);
				json = nlohmann::json::parse(http.Get(nextUrl).contents.c_str());
			}
			else
			{
				break;
			}
		}
	}
}
