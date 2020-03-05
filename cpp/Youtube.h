#pragma once

#include <string>
#include <vector>
#include "Http.h"

time_t DateToTime_t(const std::string& date);
void ShowProgressBar(int cur, int total);

class YoutubeItem
{
protected:
	static HttpClient http;
};

class Video : public YoutubeItem
{
private:
	int views;
	time_t uploadTime_t;

public:
	Video(const std::string& id, const std::string& apiKey);
	int GetViews() const { return views; }
	time_t GetUploadTime_t() const { return uploadTime_t; }
};

class Playlist : public YoutubeItem
{
private:
	std::string title;
	std::string channel;
	std::vector<Video> videos;

public:
	Playlist(const std::string& id, const std::string& apiKey);
	const std::string& GetTitle() const { return title; }
	const std::string& GetChannel() const { return channel; }
	const std::vector<Video>& GetVideos() const { return videos; }
};