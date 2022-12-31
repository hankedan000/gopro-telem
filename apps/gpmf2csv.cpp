#include <cstdio>
#include <csignal>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <stack>
#include <time.h>
#include <unistd.h>// sleep

#include "GoProTelem/GoProTelem.h"

const char *PROG_NAME = "gpmf2csv";
bool stop_app = false;

struct ProgOptions
{
    ProgOptions()
     : inputFile("")
    {}

	std::string inputFile;
};

void
handleSIGINT(
	int signal)
{
	stop_app = true;
}

void
displayUsage()
{
	printf("usage: %s -i <video_file> [options]\n",PROG_NAME);
	printf(" -i,--inputFile      : the input video file\n");
	printf(" -h,--help           : display this menu\n");
}

void
parseArgs(
	int argc,
	char *argv[],
	ProgOptions &opts)
{
	static struct option long_options[] =
	{
		{"inputFile"          , required_argument , 0                      , 'i' },
		{"help"               , no_argument       , 0                      , 'h' },
		{0, 0, 0, 0}
	};

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(
			argc,
			argv,
			"hi:",
			long_options,
			&option_index);

		// detect the end of the options
		if (c == -1)
		{
			break;
		}

		switch (c)
		{
			case 0:
				// flag setting
				break;
			case 'i':
				opts.inputFile = optarg;
				break;
			case 'h':
			case '?':
			default:
				displayUsage();
				exit(0);
				break;
		}
	}
}

void
dumpToCSV(
    gpt::MP4_Source &mp4,
    const ProgOptions &opts)
{
    auto samples = gpt::getCombinedSamples(mp4);

    // if sourceFilePath = "/tmp/GH010143.MP4", then
    // csvFileName = "GH010143.csv"
    // csvFilePath = "/tmp/GH010143.csv"
    auto sourceFilePath = std::filesystem::path(opts.inputFile);
    auto csvFileName = sourceFilePath.stem().string() + "_telemetry.csv";
    auto csvFilePath = sourceFilePath.parent_path() / csvFileName;
    auto csvFile = fopen(csvFilePath.c_str(),"w");
    if (csvFile == nullptr)
    {
        printf("failed to open CSV output file '%s'\n", csvFilePath.c_str());
        return;
    }

    printf("dumping telemetry data to '%s'\n", csvFilePath.c_str());
    fprintf(csvFile,"time (s)");
    fprintf(csvFile,",accl_x (m/s^2),accl_y (m/s^2),accl_z (m/s^2)");
    fprintf(csvFile,",gyro_x (rad/s),gyro_y (rad/s),gyro_z (rad/s)");
    fprintf(csvFile,",gps_lat (deg),gps_lon (deg),gps_altitude (m),gps_speed2D (m/s),gps_speed3D (m/s)");
    fprintf(csvFile,"\n");
    for (const auto &sample : samples)
    {
        fprintf(csvFile,"%0.06f",sample.t_offset);
        auto accl = sample.accl;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",accl.x,accl.y,accl.z);
        auto gyro = sample.gyro;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",gyro.x,gyro.y,gyro.z);
        auto gps = sample.gps;
        fprintf(csvFile,",%+0.06f,%+0.06f,%0.03f,%0.03f,%0.03f",gps.coord.lat,gps.coord.lon,gps.altitude,gps.speed2D,gps.speed3D);
        fprintf(csvFile,"\n");
    }
    fclose(csvFile);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, handleSIGINT);// ctrl+c to stop application

	ProgOptions opts;
	parseArgs(argc,argv,opts);

	if (opts.inputFile.empty())
	{
		printf("no input video file provided.\n");
		displayUsage();
		exit(0);
	}

	printf("opening MP4 file '%s'\n", opts.inputFile.c_str());
    gpt::MP4_Source mp4;
    if (mp4.open(opts.inputFile) != 0)
    {
        printf("failed to open MP4 source file. %s\n", opts.inputFile.c_str());
        exit(-1);
    }

    dumpToCSV(mp4, opts);

	return 0;
}