#include "core.h"
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL line%d: %s\n",__LINE__,#x);exit(1);}checks++;}while(0)
static R3Command parse(const char *line){R3Command c;CHECK(r3_parse(line,strlen(line),&c));return c;}
static const char *send(R3Core *c,const char *line,unsigned now){R3Command p=parse(line);return r3_command(c,&p,now,77);}
static void ready(R3Core *c,bool locked){
    r3_init(c,locked);CHECK(!strcmp(send(c,"HELLO",100),"OK"));
    c->verified=7;CHECK(!strcmp(send(c,"PING 77 1",100),"OK"));
}
int main(void){
    CHECK(SEIZO_R41_POPULATED_AXIS_MASK==7);
    for(unsigned axis=0;axis<8;axis++)CHECK(r41_axis_populated(axis)==(axis<3));
    CHECK(!r41_mask_populated(0));
    R3Core c;
    for(unsigned mask=1;mask<=31;mask++){
        ready(&c,false);R3Command arm=parse("ARM 77 2 7");arm.mask=mask;
        const char *r=r3_command(&c,&arm,100,77);
        if(mask<=7){CHECK(!strcmp(r,"OK"));CHECK(c.armed&&c.enabled==mask);}
        else{CHECK(!strcmp(r,"axis_unpopulated"));CHECK(!c.armed&&!c.enabled);}
    }
    ready(&c,true);CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"commissioning_locked"));
    CHECK(!c.armed&&!c.enabled&&!c.moving);
    ready(&c,false);c.verified=3;
    CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"driver_check_required"));
    ready(&c,false);c.verified=31;
    CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"driver_check_required"));
    for(unsigned absent=3;absent<5;absent++)for(int step=-1;step<=1;step+=2){
        ready(&c,false);CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"OK"));
        R3Command p=parse("MOVE 77 3 1000 10 0 0 0 0");p.step[absent]=step;
        CHECK(!strcmp(r3_command(&c,&p,100,77),"axis_unpopulated"));
        CHECK(!c.moving&&!c.pulse_mask&&c.enabled==7);
    }
    /* Count/sign conservation for all fitted joints; omitted joints stay inert. */
    for(unsigned sample=0;sample<25;sample++){
        ready(&c,false);CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"OK"));
        R3Command p=parse("MOVE 77 3 1000 1 1 1 0 0");
        for(unsigned a=0;a<3;a++)p.step[a]=(int32_t)((sample*37+a*19)%400)-200;
        CHECK(!strcmp(r3_command(&c,&p,100,77),"OK"));
        unsigned inactive_pulses=0;
        for(unsigned tick=1;tick<=20002;tick++){
            if(tick%1000==0)c.heartbeat_us=100+tick*50;
            r3_tick(&c,100+tick*50,false,true);
            inactive_pulses|=c.pulse_mask&24u;
        }
        CHECK(!inactive_pulses&&!c.moving&&c.armed&&c.enabled==7);
        for(unsigned a=0;a<3;a++)CHECK(c.commanded[a]==p.step[a]);
        for(unsigned a=3;a<5;a++)CHECK(c.commanded[a]==0&&c.count[a]==0&&c.emitted[a]==0);
    }
    ready(&c,false);CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"OK"));
    r3_fault(&c,2);CHECK(c.fault_latched&&c.enabled==5&&c.verified==5);
    CHECK(!strcmp(send(&c,"DISARM 77 3",100),"OK"));
    CHECK(!strcmp(send(&c,"CHECK 77 4",100),"CHECK"));CHECK(!c.verified&&!c.enabled);
    ready(&c,false);CHECK(!strcmp(send(&c,"ARM 77 2 7",100),"OK"));
    r3_tick(&c,200,false,false);CHECK(!c.armed&&!c.moving&&!c.pulse_mask&&c.enabled==7&&!c.session);
    printf("PASS %u three-driver checks; no GPIO/USB/UART hardware exercised\n",checks);
}
