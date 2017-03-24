
#include "ITCH5Messages.h"
#include "Logging.h"
#include "StatsCounter.h"
#include "EngineTM.h"
#include "TMBookPrint.h"
#include "ITCH5Reader.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char** argv)
{
    if(argc < 4)
        critical_err(-1, "Usage: %s <symbol name> <input file> <output file>\n", argv[0]);

    const char* filename = argv[2];

    int fd = open(filename, O_RDONLY);
    if(fd < 0)
        critical_err(errno, "Error opening %s\n", filename);

    struct stat statbuf;
    if(0 > fstat(fd, &statbuf))
        critical_err(errno, "fstat error on %s", filename);

    char* src = (char*)mmap(0, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(src < 0)
        critical_err(errno, "mmap error on %s", filename);


    ITCH5Reader::FilterMap symbolFilter;
    FILE* symbolFile = fopen(argv[1], "r");
    if(symbolFile == NULL)
    {
        Symbol arg = Symbol::itchify(argv[1]);
        symbolFilter[arg.n] = 0;
    }
    else
    {
        while(!feof(symbolFile))
        {
            char line[30];
            if(NULL == fgets(line, 30, symbolFile))
                break;
            Symbol sym = Symbol::itchify(line);
            symbolFilter[sym.n] = 0;
        }
    }

    const char* outfilename = argv[3];
    TMBookPrint::Args engineArgs = { .filename = outfilename, .printSymbol = symbolFilter.size() > 1 };
    EngineTM tm(&engineArgs);

    ITCH5Reader* reader = new ITCH5Reader(symbolFilter, tm);

    for(off_t pos = 0; pos <= statbuf.st_size;)
    {
        pos += reader->processMessage(src + pos);
    }

    printStatsSummary();
}

