#include <iostream>
#include <iomanip>
#include <ctime>
#include "Youtube.h"

struct Data
{
	int index;
	int views;
	double viewsPerWeek;
	Data() = default;
	Data(int index, int views, float viewsPerWeek)
		: index(index), views(views), viewsPerWeek(viewsPerWeek)
	{}
};

int main(int argc, const char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: playlist <Playlist ID> <API Key>";
		return 1;
	}

	std::string playlistID(argv[1]);
	std::string apiKey(argv[2]);

	std::vector<Data> datas;
	time_t now = std::time(nullptr);
	std::tm* tm = std::gmtime(&now);
	double viewsPerWeek;
	int n = 0;
	constexpr int secondsInAWeek = 60 * 60 * 24 * 7;

	Playlist playlist(playlistID, apiKey);

	datas.reserve(playlist.GetVideos().size());
	for (const Video& vid : playlist.GetVideos())
	{
		viewsPerWeek = vid.GetViews() / (static_cast<double>(now - vid.GetUploadTime_t()) / secondsInAWeek);
		datas.emplace_back(++n, vid.GetViews(), viewsPerWeek);
	}

	std::cout << "\n\nView analysis of [" << playlist.GetTitle()
		<< "] by [" << playlist.GetChannel()
		<< "] on " << std::put_time(tm, "%Y-%m-%d");
	std::cout << "\n\n";
	std::cout << "index\tviews\tviews per week\n";
	std::cout << "_______________________________________\n";
	for (const Data& d : datas)
	{
		std::cout << d.index << '\t'
			<< d.views << '\t'
			<< d.viewsPerWeek << '\n';
	}

	return 0;
}