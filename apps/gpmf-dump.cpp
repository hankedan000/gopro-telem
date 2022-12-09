#include <csignal>
#include <getopt.h>
#include <iostream>
#include <stack>
#include <time.h>
#include <unistd.h>// sleep

#include "GoProTelem/GoProTelem.h"

const char *PROG_NAME = "gpmf-dump";
bool stop_app = false;

struct ProgOptions
{
	std::string inputFile;
	int dumpStructure = 0;
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
	printf(" --structure         : dump stream structures\n");
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
		{"structure"          , no_argument       , &opts.dumpStructure    , 1   },
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
dumpStructure(
    gpt::MP4_Source &mp4)
{
    for (size_t payloadIdx=0; payloadIdx<mp4.payloadCount(); payloadIdx++)
    {
        printf("====================== BEGIN PAYLOAD %04ld ======================\n", payloadIdx);
        auto payload = mp4.getPayload(payloadIdx);
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

    if (opts.dumpStructure)
    {
        dumpStructure(mp4);
    }

	return 0;
}