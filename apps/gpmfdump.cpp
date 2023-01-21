#include <csignal>
#include <getopt.h>
#include <iostream>
#include <limits>
#include <stack>
#include <time.h>
#include <unistd.h>// sleep

#include "GoProTelem/GoProTelem.h"

const char *PROG_NAME = "gpmfdump";
bool stop_app = false;

struct ProgOptions
{
    ProgOptions()
     : inputFile("")
     , payloadStartIdxSet(false)
     , payloadStartIdx(0)
     , numPayloadsToDumpSet(false)
     , numPayloadsToDump(0)
     , showInfo(0)
     , showStructure(0)
    {}

	std::string inputFile;
    bool payloadStartIdxSet;
    size_t payloadStartIdx;
    bool numPayloadsToDumpSet;
    size_t numPayloadsToDump;
    int showInfo = 0;
	int showStructure = 0;
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
	printf(" -p                  : payload index to start dumping at\n");
	printf(" -P                  : number of payloads to dump\n");
	printf(" --show-info         : dump payload information\n");
	printf(" --show-structure    : dump payload stream structures\n");
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
		{"show-info"          , no_argument       , &opts.showInfo         , 1   },
		{"show-structure"     , no_argument       , &opts.showStructure    , 1   },
		{"help"               , no_argument       , 0                      , 'h' },
		{0, 0, 0, 0}
	};

    opts.payloadStartIdxSet = false;
    opts.numPayloadsToDump = false;

	while (true)
	{
		int option_index = 0;
		int c = getopt_long(
			argc,
			argv,
			"hi:p:P:",
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
            case 'p':
                opts.payloadStartIdx = atoll(optarg);
                opts.payloadStartIdxSet = true;
                break;
            case 'P':
                opts.numPayloadsToDump = atoll(optarg);
                opts.numPayloadsToDumpSet = true;
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

std::string
getTabs(
    unsigned int tabDepth,
    std::string tabStr = "\t")
{
    std::string tabs;
    while (tabDepth-- > 0)
    {
        tabs += tabStr;
    }
    return tabs;
}

void
printMP4_Info(
    gpt::MP4_Source &mp4)
{
    printf("duration:     %0.3f s\n", mp4.duration());
    printf("payloadCount: %ld\n", mp4.payloadCount());
    printf("fps:          %0.3f\n", mp4.fps());

    gpt::MP4_SensorInfo sensorInfo;
    printf("ACCL:\n");
    if (mp4.getSensorInfo(gpt::GPMF_KEY_ACCL, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }

    printf("GYRO:\n");
    if (mp4.getSensorInfo(gpt::GPMF_KEY_GYRO, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }

    printf("GPS5:\n");
    if (mp4.getSensorInfo(gpt::GPMF_KEY_GPS5, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }
}

void
printPayloadInfo(
    gpt::GPMF_PayloadPtr payload)
{
    double inTime = payload->inTime();
    double outTime = payload->outTime();
    printf("inTime:   %8.3f s\n", inTime);
    printf("outTime:  %8.3f s\n", outTime);
    printf("duration: %8.3f s\n", (outTime - inTime));

    gpt::PayloadSensorInfo sensorInfo;
    printf("ACCL:\n");
    if (payload->getSensorInfo(gpt::GPMF_KEY_ACCL, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }

    printf("GYRO:\n");
    if (payload->getSensorInfo(gpt::GPMF_KEY_GYRO, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }

    printf("GPS5:\n");
    if (payload->getSensorInfo(gpt::GPMF_KEY_GPS5, sensorInfo))
    {
        printf("%s\n", sensorInfo.toString("   ").c_str());
    }
    else
    {
        printf("   none\n");
    }
}

void
printPayloadStructure(
    gpt::GPMF_PayloadPtr payload)
{
    auto stream = payload->getStream();
    struct StructInfo
    {
        size_t totalStructSize;
        size_t bytesSeenInStruct;
    };
    bool currStructKnown = false;
    std::stack<StructInfo> infoStack;
    StructInfo currStructInfo;
    currStructInfo.totalStructSize = 0;
    currStructInfo.bytesSeenInStruct = 0;
    if (stream->type() == '\0')
    {
        currStructKnown = true;
        currStructInfo.totalStructSize = stream->structSize() * stream->repeat();
    }
    while (true)
    {
        printf("%s%s\n",
            getTabs(infoStack.size(),"   ").c_str(),
            stream->klvToString().c_str());

        if (currStructKnown)
        {
            currStructInfo.bytesSeenInStruct += stream->klvTotalSizePadded();
            // printf("struct progress: %ld/%ld bytes\n",
            //     currStructInfo.bytesSeenInStruct,
            //     currStructInfo.totalStructSize);
        }
        
        if (stream->type() == '\0')
        {
            // when type is null, it means it's a type defined of other types (a struct)
            if (currStructKnown)
            {
                infoStack.push(currStructInfo);
                currStructInfo.totalStructSize = stream->structSize() * stream->repeat();
                currStructInfo.bytesSeenInStruct = 0;
            }
            else
            {
                currStructKnown = true;
                currStructInfo.totalStructSize = stream->structSize() * stream->repeat();
                currStructInfo.bytesSeenInStruct = 0;
            }
        }
        else if (currStructKnown && currStructInfo.bytesSeenInStruct >= currStructInfo.totalStructSize)
        {
            // finished parsing all bytes in a struct
            if (infoStack.empty())
            {
                currStructKnown = false;
            }
            else
            {
                currStructInfo = infoStack.top();
                infoStack.pop();
            }
            
        }

        if ( ! stream->next(gpt::GPMF_Levels::GPMF_RECURSE_LEVELS))
        {
            break;// nothing left to parse, so stop
        }
    }

    if ( ! infoStack.empty())
    {
        currStructInfo = infoStack.top();
        infoStack.pop();
    }
    if (currStructInfo.bytesSeenInStruct < currStructInfo.totalStructSize)
    {
        printf("missing bytes in final structure! only parsed %ld of %ld.\n",
            currStructInfo.bytesSeenInStruct,
            currStructInfo.totalStructSize);
    }
}

void
printPayloadDump(
    gpt::MP4_Source &mp4,
    const ProgOptions &opts)
{
    size_t startIdx = 0;
    if (opts.payloadStartIdxSet)
    {
        startIdx = opts.payloadStartIdx;
        if (opts.payloadStartIdx >= mp4.payloadCount())
        {
            printf("payload start idx %ld is >= mp4 payloadCount %ld\n",
                startIdx,
                mp4.payloadCount());
            exit(-1);
        }
    }
    size_t endIdx = mp4.payloadCount() - 1;
    if (opts.numPayloadsToDumpSet)
    {
        endIdx = startIdx + opts.numPayloadsToDump - 1;
        if (endIdx >= mp4.payloadCount())
        {
            printf("payload end idx %ld is >= mp4 payloadCount %ld\n",
                endIdx,
                mp4.payloadCount());
            exit(-1);
        }
    }
    for (size_t payloadIdx=startIdx; payloadIdx<=endIdx; payloadIdx++)
    {
        printf("====================== BEGIN PAYLOAD %04ld ======================\n", payloadIdx);
        auto payload = mp4.getPayload(payloadIdx);
        if (opts.showInfo)
        {
            printf("------------------------- PAYLOAD INFO --------------------------\n");
            printPayloadInfo(payload);
        }
        if (opts.showStructure)
        {
            printf("--------------------------- STRUCTURE ---------------------------\n");
            printPayloadStructure(payload);
        }
        printf("======================= END PAYLOAD %04ld =======================\n", payloadIdx);
    }
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

	printf("opening %s\n", opts.inputFile.c_str());
    gpt::MP4_Source mp4;
    if (mp4.open(opts.inputFile) != 0)
    {
        printf("failed to open MP4 source file. %s\n", opts.inputFile.c_str());
        exit(-1);
    }

    printf("--------------------------- MP4 INFO ----------------------------\n");
    printMP4_Info(mp4);

    if (opts.showInfo || opts.showStructure)
    {
        printPayloadDump(mp4, opts);
    }

	return 0;
}