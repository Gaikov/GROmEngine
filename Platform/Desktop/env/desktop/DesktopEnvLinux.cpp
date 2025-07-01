//
// Created by roman on 7/1/25.
//

#include "env/Env.h"
#include "nsLib/StrTools.h"

void nsEnv::MessagePopup(const char *title, const char *message) {
    nsString    command;
    command.Format("zenity --info --title='%s' --text='%s'", title, message);
    system(command);
}

void nsEnv::OpenUrl(const char *url) {
    nsString    command;
    command.Format("xdg-open %s", url);
    system(command);
}
