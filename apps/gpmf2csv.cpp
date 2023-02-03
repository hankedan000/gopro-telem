#include <cstdio>
#include <csignal>
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
	 , raw(0)
    {}

	std::string inputFile;
	int raw;
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
	printf("--raw                : sensor values are dumped raw (ie. not resampled to match frame-rate timeline)\n");
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
		{"raw"                , no_argument       , &opts.raw              , 1   },
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
dumpRawToCSV(
    gpt::MP4_Source &mp4,
    const ProgOptions &opts)
{
    // if sourceFilePath = "/tmp/GH010143.MP4", then
    // csvFileName = "GH010143_telemetry_raw.csv"
    // csvFilePath = "/tmp/GH010143_telemetry_raw.csv"
    auto sourceFilePath = gpt::filesystem::path(opts.inputFile);
    auto csvFileName = sourceFilePath.stem().string() + "_telemetry_raw.csv";
    auto csvFilePath = sourceFilePath.parent_path() / csvFileName;
    auto csvFile = fopen(csvFilePath.c_str(),"w");
    if (csvFile == nullptr)
    {
        printf("failed to open CSV output file '%s'\n", csvFilePath.c_str());
        return;
    }

	auto acclSamps = gpt::getAcclSamples(mp4);
	gpt::MP4_SensorInfo acclInfo;
	bool acclValid = mp4.getSensorInfo(gpt::GPMF_KEY_ACCL,acclInfo);
	auto gyroSamps = gpt::getGyroSamples(mp4);
	gpt::MP4_SensorInfo gyroInfo;
	bool gyroValid = mp4.getSensorInfo(gpt::GPMF_KEY_GYRO,gyroInfo);
	auto gravSamps = gpt::getGravSamples(mp4);
	gpt::MP4_SensorInfo gravInfo;
	bool gravValid = mp4.getSensorInfo(gpt::GPMF_KEY_GRAV,gravInfo);
	auto coriSamps = gpt::getCoriSamples(mp4);
	gpt::MP4_SensorInfo coriInfo;
	bool coriValid = mp4.getSensorInfo(gpt::GPMF_KEY_CORI,coriInfo);
	auto gpsSamps = gpt::getGPS_Samples(mp4);
	gpt::MP4_SensorInfo gpsInfo;
	bool gpsValid = mp4.getSensorInfo(gpt::GPMF_KEY_GPS5,gpsInfo);

    printf("dumping telemetry data to '%s'\n", csvFilePath.c_str());
	// column headings (sensor names)
    fprintf(csvFile,"accl_x (m/s^2),accl_y (m/s^2),accl_z (m/s^2)");
    fprintf(csvFile,",gyro_x (rad/s),gyro_y (rad/s),gyro_z (rad/s)");
    fprintf(csvFile,",grav_x (G),grav_y (G),grav_z (G)");
    fprintf(csvFile,",cori_w,cori_x,cori_y,cori_z");
    fprintf(csvFile,",gps_lat (deg),gps_lon (deg),gps_altitude (m),gps_speed2D (m/s),gps_speed3D (m/s)");
    fprintf(csvFile,"\n");

	// samples counts per sensor
    fprintf(csvFile,"%ld,%ld,%ld",acclSamps.size(),acclSamps.size(),acclSamps.size());
    fprintf(csvFile,",%ld,%ld,%ld",gyroSamps.size(),gyroSamps.size(),gyroSamps.size());
    fprintf(csvFile,",%ld,%ld,%ld",gravSamps.size(),gravSamps.size(),gravSamps.size());
    fprintf(csvFile,",%ld,%ld,%ld,%ld",coriSamps.size(),coriSamps.size(),coriSamps.size(),coriSamps.size());
    fprintf(csvFile,",%ld,%ld,%ld,%ld,%ld",gpsSamps.size(),gpsSamps.size(),gpsSamps.size(),gpsSamps.size(),gpsSamps.size());
    fprintf(csvFile,"\n");

	// samples rates per sensor
	const auto acclRate = (acclValid ? acclInfo.measuredRate_hz : 0.0);
	const auto gyroRate = (gyroValid ? gyroInfo.measuredRate_hz : 0.0);
	const auto gravRate = (gravValid ? gravInfo.measuredRate_hz : 0.0);
	const auto coriRate = (coriValid ? coriInfo.measuredRate_hz : 0.0);
	const auto gpsRate = (gpsValid ? gpsInfo.measuredRate_hz : 0.0);
    fprintf(csvFile,"%0.6f,%0.6f,%0.6f",acclRate,acclRate,acclRate);
    fprintf(csvFile,",%0.6f,%0.6f,%0.6f",gyroRate,gyroRate,gyroRate);
    fprintf(csvFile,",%0.6f,%0.6f,%0.6f",gravRate,gravRate,gravRate);
    fprintf(csvFile,",%0.6f,%0.6f,%0.6f,%0.6f",coriRate,coriRate,coriRate,coriRate);
    fprintf(csvFile,",%0.6f,%0.6f,%0.6f,%0.6f,%0.6f",gpsRate,gpsRate,gpsRate,gpsRate,gpsRate);
    fprintf(csvFile,"\n");

	// raw sensor values
	size_t mostSamps = 0;
	mostSamps = std::max(mostSamps,acclSamps.size());
	mostSamps = std::max(mostSamps,gyroSamps.size());
	mostSamps = std::max(mostSamps,gravSamps.size());
	mostSamps = std::max(mostSamps,coriSamps.size());
	mostSamps = std::max(mostSamps,gpsSamps.size());
	for (size_t ss=0; ss<mostSamps; ss++)
	{
		// accl
		if (ss < acclSamps.size())
		{
			auto accl = acclSamps.at(ss).sample;
			fprintf(csvFile,"%+0.06f,%+0.06f,%+0.06f",accl.x,accl.y,accl.z);
		}
		else
		{
			fprintf(csvFile,"%d,%d,%d",0,0,0);
		}

		// gyro
		if (ss < gyroSamps.size())
		{
			auto gyro = gyroSamps.at(ss).sample;
			fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",gyro.x,gyro.y,gyro.z);
		}
		else
		{
			fprintf(csvFile,",%d,%d,%d",0,0,0);
		}
		
		// grav
		if (ss < gravSamps.size())
		{
			auto grav = gravSamps.at(ss).sample;
			fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",grav.x,grav.y,grav.z);
		}
		else
		{
			fprintf(csvFile,",%d,%d,%d",0,0,0);
		}
		
		// cori
		if (ss < coriSamps.size())
		{
			auto cori = coriSamps.at(ss).sample;
			fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f,%+0.06f",cori.w,cori.x,cori.y,cori.z);
		}
		else
		{
			fprintf(csvFile,",%d,%d,%d,%d",0,0,0,0);
		}
		
		// gps
		if (ss < gpsSamps.size())
		{
			auto gps = gpsSamps.at(ss).sample;
			fprintf(csvFile,",%+0.06f,%+0.06f,%0.03f,%0.03f,%0.03f",gps.coord.lat,gps.coord.lon,gps.altitude,gps.speed2D,gps.speed3D);
		}
		else
		{
			fprintf(csvFile,",%d,%d,%d,%d,%d",0,0,0,0,0);
		}
        fprintf(csvFile,"\n");
	}

    fclose(csvFile);
}

void
dumpCombinedToCSV(
    gpt::MP4_Source &mp4,
    const ProgOptions &opts)
{
    auto samples = gpt::getCombinedTimedSamples(mp4);

    // if sourceFilePath = "/tmp/GH010143.MP4", then
    // csvFileName = "GH010143_telemetry_combined.csv"
    // csvFilePath = "/tmp/GH010143_telemetry_combined.csv"
    auto sourceFilePath = gpt::filesystem::path(opts.inputFile);
    auto csvFileName = sourceFilePath.stem().string() + "_telemetry_combined.csv";
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
    fprintf(csvFile,",grav_x (G),grav_y (G),grav_z (G)");
    fprintf(csvFile,",cori_w,cori_x,cori_y,cori_z");
    fprintf(csvFile,",gps_lat (deg),gps_lon (deg),gps_altitude (m),gps_speed2D (m/s),gps_speed3D (m/s)");
    fprintf(csvFile,"\n");
    for (const auto &tSample : samples)
    {
		const auto &sample = tSample.sample;
        fprintf(csvFile,"%0.06f",tSample.t_offset);
        auto accl = sample.accl;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",accl.x,accl.y,accl.z);
        auto gyro = sample.gyro;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",gyro.x,gyro.y,gyro.z);
        auto grav = sample.grav;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f",grav.x,grav.y,grav.z);
        auto cori = sample.cori;
        fprintf(csvFile,",%+0.06f,%+0.06f,%+0.06f,%+0.06f",cori.w,cori.x,cori.y,cori.z);
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

	if (opts.raw)
	{
		dumpRawToCSV(mp4, opts);
	}
	else
	{
    	dumpCombinedToCSV(mp4, opts);
	}

	return 0;
}