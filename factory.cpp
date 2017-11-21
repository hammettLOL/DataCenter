#include "factory.h"

Factory::Factory()
{

}


int Factory::basic_Call()
{
        basic_Registration();

        linphone_core_enable_logs(NULL); /*enable liblinphone logs.*/

        /*
         Fill the LinphoneCoreVTable with application callbacks.
         All are optional. Here we only use the call_state_changed callbacks
         in order to get notifications about the progress of the call.
         */
        vtable.call_state_changed=call_state_changed;
        /*
         Instanciate a LinphoneCore object given the LinphoneCoreVTable
        */
        lc=linphone_core_new(&vtable,NULL,NULL,NULL);
        if (dest){
                /*
                 Place an outgoing call
                */
                call=linphone_core_invite(lc,dest);
                if (call==NULL){

                        printf("Could not place call to %s\n",dest);
                        goto end;
                }else printf("Call to %s is in progress...",dest);
                linphone_call_ref(call);
        }
        /* main loop for receiving notifications and doing background linphonecore work: */
        while(running){
                linphone_core_iterate(lc);
                Sleep(10000);
        }
        if (call && linphone_call_get_state(call)!=LinphoneCallEnd){
                /* terminate the call */
                printf("Terminating the call...\n");
                linphone_core_terminate_call(lc,call);
                /*at this stage we don't need the call object */
                linphone_call_unref(call);
        }
    end:
        printf("Shutting down...\n");
        linphone_core_destroy(lc);
        printf("Exited\n");
        return 0;
}

void Factory::call_state_changed(LinphoneCore *lc, LinphoneCall *call, LinphoneCallState cstate, const char *msg)
{
    switch(cstate){
            case LinphoneCallOutgoingRinging:
                    printf("It is now ringing remotely !\n");
            break;
            case LinphoneCallOutgoingEarlyMedia:
                    printf("Receiving some early media\n");
            break;
            case LinphoneCallConnected:
                    printf("We are connected !\n");
            break;
            case LinphoneCallStreamsRunning:
                    printf("Media streams established !\n");
            break;
            case LinphoneCallEnd:
                    printf("Call is terminated.\n");
            break;
            case LinphoneCallError:
                    printf("Call failure !");
            break;
            default:
                    printf("Unhandled notification %i\n",cstate);
    }

}

void Factory::registration_state_changed(struct _LinphoneCore *lc, LinphoneProxyConfig *cfg, LinphoneRegistrationState cstate, const char *message){
                printf("New registration state %s for user id [%s] at proxy [%s]\n"
                                ,linphone_registration_state_to_string(cstate)
                                ,linphone_proxy_config_get_identity(cfg)
                                ,linphone_proxy_config_get_addr(cfg));
}

int Factory::basic_Registration(){
        LinphoneCoreVTable vtable={0};
        LinphoneProxyConfig* proxy_cfg;
        LinphoneAddress *from;
        LinphoneAuthInfo *info;
        char* identity="sip:1000@192.168.1.3";
        char* password="maver123";
        const char* server_addr="sip:192.168.1.3";
        /* takes   sip uri  identity from the command line arguments */


#ifdef DEBUG
        linphone_core_enable_logs(NULL); /*enable liblinphone logs.*/
#endif
        /*
         Fill the LinphoneCoreVTable with application callbacks.
         All are optional. Here we only use the registration_state_changed callbacks
         in order to get notifications about the progress of the registration.
         */
        vtable.registration_state_changed=registration_state_changed;
        /*
         Instanciate a LinphoneCore object given the LinphoneCoreVTable
        */
        lc=linphone_core_new(&vtable,NULL,NULL,NULL);
        /*create proxy config*/
        proxy_cfg = linphone_proxy_config_new();
        /*parse identity*/
        from = linphone_address_new(identity);
        if (from==NULL){
                printf("%s not a valid sip uri, must be like sip:toto@sip.linphone.org \n",identity);
                goto end;
        }else{
            printf("Se logro conectar");
        }
        if (password!=NULL){
                info=linphone_auth_info_new(linphone_address_get_username(from),NULL,password,NULL,NULL,NULL); /*create authentication structure from identity*/
                linphone_core_add_auth_info(lc,info); /*add authentication info to LinphoneCore*/
        }
        // configure proxy entries
        linphone_proxy_config_set_identity(proxy_cfg,identity); /*set identity with user name and domain*/
        server_addr = linphone_address_get_domain(from); /*extract domain address from identity*/
        linphone_proxy_config_set_server_addr(proxy_cfg,server_addr); /* we assume domain = proxy server address*/
        linphone_proxy_config_enable_register(proxy_cfg,TRUE); /*activate registration for this proxy config*/
        linphone_address_unref(from); /*release resource*/
        linphone_core_add_proxy_config(lc,proxy_cfg); /*add proxy config to linphone core*/
        linphone_core_set_default_proxy(lc,proxy_cfg); /*set to default proxy*/
        /* main loop for receiving notifications and doing background linphonecore work: */
        while(running){
                linphone_core_iterate(lc);
                Sleep(10000);
        }

        proxy_cfg = linphone_core_get_default_proxy_config(lc); /* get default proxy config*/
        linphone_proxy_config_edit(proxy_cfg); /*start editing proxy configuration*/
        linphone_proxy_config_enable_register(proxy_cfg,TRUE); /*de-activate registration for this proxy config*/
        linphone_proxy_config_done(proxy_cfg); /*initiate REGISTER with expire = 0*/
        while(linphone_proxy_config_get_state(proxy_cfg) !=  LinphoneRegistrationCleared){
                linphone_core_iterate(lc);
        }


end:
        printf("Shutting down...\n");
        linphone_core_destroy(lc);
        printf("Exited\n");
        return 1;
}
