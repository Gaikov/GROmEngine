#include "Args.h"
#include "UnpackStrategy.h"
#include "nsLib/log.h"
#include "Core/debug/LogStdOut.h"
#include "Engine/Platform.h"
#include "Core/AppInfo.h"
#include "PackStrategy.h"
#include "Options.h"

Platform *App_GetPlatform() {
    return nullptr;
}

IAppInfo *App_GetInfo() {
    return nullptr;
}

void ShowHelp() {
    Log::Info("DataPacker v%s\n"\
        "GROm Games (c) 2002-2024\n\n"\
        "usage to pack: DataPacker [directory] [outfile] [--pass [encoding phrase]]\n"\
              "usage to unpack: DataPacker --unpack [packedfile] [outdirectory]\n", PACKER_VERSION
    );
}

int main(int argc, char *argv[]) {
    LogStdOut out;
    Log::Init();
    Log::Shared()->AddPolicy(&out);

    ShowHelp();

    Args args(argc, argv);
    if (args.HasOption(OPT_UNPACK)) {
        if (args.Length() < 4) {
            printf("Not enough params to unpack data!");
            return -1;
        }

        UnpackStrategy unPacker(args.GetParam(2), args.GetParam(3));
        if (!unPacker.Perform()) {
            return -1;
        }
    } else {
        nsPackStrategy packer(args);
        if (!packer.Perform()) {
            return -1;
        }
    }

    Log::Release();
    return 0;
}


