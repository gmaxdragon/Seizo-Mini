/* Native test harness only; no GPIO, USB device, UART or hardware access.
 * Uses the actual unmodified core.h supplied with the reviewed firmware.
 * @ lines are simulator injection controls, NOT controller commands.
 */
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

int main(int argc, char **argv) {
    R3Core c;
    r3_init(&c, !(argc > 1 && !strcmp(argv[1], "--unlocked-simulation")));
    char line[2048]; uint32_t now=1000, nonce=91; uint8_t good=7;
    while(fgets(line,sizeof line,stdin)) {
        size_t n=strcspn(line,"\n");
        if(!strncmp(line,"@ADVANCE ",9)) {
            now+=(uint32_t)strtoul(line+9,NULL,10);
            r3_tick(&c,now,c.hold_open,true);
            continue;
        }
        if(!strncmp(line,"@CHECK_MASK ",12)) {good=(uint8_t)strtoul(line+12,NULL,10);continue;}
        if(!strncmp(line,"@FAULT ",7)) {r3_fault(&c,(uint8_t)strtoul(line+7,NULL,10));continue;}
        if(!strncmp(line,"@HOLD ",6)) {c.hold_open=atoi(line+6)!=0;r3_tick(&c,now,c.hold_open,true);continue;}
        if(!strncmp(line,"@LOCK ",6)) {c.locked=atoi(line+6)!=0;continue;}
        if(!strncmp(line,"@TICKS ",7)) {
            unsigned count=(unsigned)strtoul(line+7,NULL,10);
            for(unsigned i=0;i<count;i++){now+=R3_TICK_US;r3_tick(&c,now,c.hold_open,true);}
            continue;
        }
        if(!strncmp(line,"@USB_LOST",9)) {r3_tick(&c,now,c.hold_open,false);continue;}
        R3Command command; const char *result="invalid_command";
        if(r3_parse(line,n,&command)) {
            result=r3_command(&c,&command,now,++nonce);
            if(!strcmp(result,"CHECK")) {
                c.verified=good;
                if(good==SEIZO_R41_POPULATED_AXIS_MASK){c.fault_latched=false;c.fault_axes=0;result="check_complete";}
                else {r3_fault(&c,(uint8_t)(SEIZO_R41_POPULATED_AXIS_MASK^good));result="check_failed";}
            }
        }
        printf("{\"result\":\"%s\",\"revision\":\"R4.1 three-driver bench\",\"populated_mask\":7,\"locked\":%s,\"session\":%" PRIu32 ",\"seq\":%" PRIu32 ",\"armed\":%s,\"moving\":%s,\"hold_open\":%s,\"verified_mask\":%u,\"enabled_mask\":%u,\"fault_mask\":%u,\"fault_latched\":%s,\"position_known\":false,\"commanded_pulses\":[%" PRId64 ",%" PRId64 ",%" PRId64 ",%" PRId64 ",%" PRId64 "],\"rms_mA\":[704,704,337,0,0]}\n",
               result,c.locked?"true":"false",c.session,c.sequence,c.armed?"true":"false",c.moving?"true":"false",c.hold_open?"true":"false",c.verified,c.enabled,c.fault_axes,c.fault_latched?"true":"false",c.commanded[0],c.commanded[1],c.commanded[2],c.commanded[3],c.commanded[4]);
        fflush(stdout);
    }
    return 0;
}
