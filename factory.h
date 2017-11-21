#ifndef FACTORY_H
#define FACTORY_H
#include <linphone/linphonecore.h>
#include "signal.h"
#include <Windows.h>

class Factory
{
private:
    LinphoneCoreVTable vtable={0};
    LinphoneCore *lc;
    LinphoneCall *call=NULL;
    const char *dest="1000@192.168.1.14";
    bool_t running = TRUE;


public:

    Factory();

    int basic_Call();
    static void call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg);
    static void registration_state_changed(struct _LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message);
    int basic_Registration();
};

#endif // FACTORY_H
