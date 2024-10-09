//
// Created by Roman Gaikov on 2019-08-30.
//

#include "UnitCommon.h"
#include "Core/Args.h"
#include "nsLib/StrTools.h"

TEST(Args, ArgsConstructFromArray) {
    const char *argv[] = {"arg1", "arg2", "arg3"};
    nsArgs args;
    args.FromArgs(3, argv);

    ASSERT_EQ(args.ArgCount(), 3);
    ASSERT_TRUE(StrEqual(args.Arg(0), "arg1"));
    ASSERT_TRUE(StrEqual(args.Arg(1), "arg2"));
    ASSERT_TRUE(StrEqual(args.Arg(2), "arg3"));
    ASSERT_TRUE(args.HasArg("arg2"));
    ASSERT_TRUE(!args.HasArg("arg10"));
}



