#include "core.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks=0;
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL %s:%d %s\n",__FILE__,__LINE__,#x);exit(1);} checks++; }while(0)
static R3Command parse(const char *s){R3Command p;CHECK(r3_parse(s,strlen(s),&p));return p;}
static const char *send(R3Core *c,const char *s,uint32_t now){R3Command p=parse(s);return r3_command(c,&p,now,77);}
static void ready(R3Core *c){r3_init(c,false);CHECK(!strcmp(send(c,"HELLO",100),"OK"));c->verified=31;CHECK(!strcmp(send(c,"PING 77 1",100),"OK"));CHECK(!strcmp(send(c,"ARM 77 2 31",100),"OK"));}
int main(void){
    /* A lost/extra UART write must fail the configuration acknowledgment,
     * including when the real8-bit counter wraps from255 to0. */
    for(unsigned before=0;before<256;before++){
        CHECK(r4_ifcnt_increment_matches(before,(before+R4_TMC_VERIFIED_CONFIG_WRITES)&255u,R4_TMC_VERIFIED_CONFIG_WRITES));
        CHECK(!r4_ifcnt_increment_matches(before,(before+R4_TMC_VERIFIED_CONFIG_WRITES-1u)&255u,R4_TMC_VERIFIED_CONFIG_WRITES));
        CHECK(!r4_ifcnt_increment_matches(before,(before+R4_TMC_VERIFIED_CONFIG_WRITES+1u)&255u,R4_TMC_VERIFIED_CONFIG_WRITES));
    }
    CHECK(!r4_ifcnt_increment_matches(0,0,256));
    CHECK(R4_TMC_SENDDELAY_REGISTER==0x03u);
    CHECK(((R4_TMC_SENDDELAY_VALUE>>8)&15u)>=2u);
    CHECK((uint64_t)R4_UART_IDLE_US*R4_UART_BAUD>=12u*1000000u);
    const uint32_t entropy_cases[]={0,1,77,0x7fffffffu,0x80000000u,0xfffffffeu,0xffffffffu};
    const uint32_t previous_cases[]={0,1,77,0x7fffffffu};
    for(size_t e=0;e<sizeof entropy_cases/sizeof entropy_cases[0];e++){
        for(size_t old=0;old<sizeof previous_cases/sizeof previous_cases[0];old++){
            uint32_t nonce=r3_session_nonce(entropy_cases[e],previous_cases[old]);
            CHECK(nonce>=1&&nonce<=0x7fffffffu);
            CHECK(nonce!=previous_cases[old]);
        }
    }
    uint8_t req[3]={5,0,0};CHECK(r3_crc(req,3)==0x48);
    R3Command p;
    const char *bad[]={"","ARM","ARM 77 2 0","ARM 77 2 32","ARM -1 2 1","ARM 2147483648 2 1","PING 77 0",
       "MOVE 77 1 50 2000 0 0 0 0","MOVE 77 1 50 1 0 0 0 0 extra","MOVE 77 1 49 1 0 0 0 0",
       "MOVE 77 1 30001 1 0 0 0 0","MOVE 77 1 1000 0 0 0 0 0","MOVE 77 1 1000 -2147483648 0 0 0 0",
       "MOVE 77 1 1000 1.5 0 0 0 0","MOVE 77 1 1000 1\r 0 0 0 0","STATUS x","status","HOLD\n","STATUS\x01"};
    for(size_t i=0;i<sizeof bad/sizeof bad[0];i++)CHECK(!r3_parse(bad[i],strlen(bad[i]),&p));
    CHECK(r3_parse("STATUS\r",7,&p));CHECK(r3_parse("\tSTATUS ",8,&p));
    char huge[161];memset(huge,'A',sizeof huge);CHECK(!r3_parse(huge,sizeof huge,&p));
    R3Core c;r3_init(&c,true);CHECK(!c.enabled&&!c.pulse_mask&&!c.armed);
    send(&c,"HELLO",100);c.verified=31;send(&c,"PING 77 1",100);
    CHECK(!strcmp(send(&c,"ARM 77 2 31",100),"commissioning_locked"));CHECK(!c.enabled);
    ready(&c);CHECK(c.enabled==31);CHECK(!strcmp(send(&c,"ARM 77 2 31",100),"session_or_sequence"));
    CHECK(!strcmp(send(&c,"CHECK 77 3",100),"disable_before_check"));
    CHECK(!strcmp(send(&c,"MOVE 77 4 100 10 -5 3 0 -2",100),"OK"));
    uint32_t pulses[5]={0};uint8_t prior=0;
    for(uint32_t t=1;t<2200;t++){
        if(t%1000==0)c.heartbeat_us=100+t*50;
        r3_tick(&c,100+t*50,false,true);
        CHECK(!(prior&c.pulse_mask));prior=c.pulse_mask;
        for(unsigned a=0;a<5;a++)if(c.pulse_mask&(1u<<a))pulses[a]++;
    }
    CHECK(!c.moving&&c.armed&&c.pulse_mask==0);CHECK(pulses[0]==10&&pulses[1]==5&&pulses[2]==3&&pulses[3]==0&&pulses[4]==2);
    CHECK(c.commanded[0]==10&&c.commanded[1]==-5&&c.commanded[4]==-2);
    CHECK(!strcmp(send(&c,"HOLD",110000),"OK"));CHECK(!c.armed&&c.enabled==31);
    r3_tick(&c,110050,false,true);CHECK(!c.armed&&!c.pulse_mask);
    CHECK(!strcmp(send(&c,"MOVE 77 5 100 1 0 0 0 0",110050),"not_armed_or_busy"));
    CHECK(!strcmp(send(&c,"HELLO",110100),"OK"));CHECK(c.enabled==31&&!c.armed&&!c.heartbeat_seen);
    CHECK(!strcmp(send(&c,"ARM 77 1 31",110100),"heartbeat_required"));
    ready(&c);r3_tick(&c,200,true,true);CHECK(!c.armed&&c.enabled==31&&c.hold_open);
    r3_tick(&c,250,false,true);CHECK(!c.armed);
    ready(&c);r3_tick(&c,500101,false,true);CHECK(!c.armed&&c.enabled==31);
    CHECK(c.session==0&&!c.heartbeat_seen);
    CHECK(!strcmp(send(&c,"PING 77 3",500102),"session_or_sequence"));
    ready(&c);r3_tick(&c,200,false,false);CHECK(!c.armed&&c.enabled==31);
    CHECK(c.session==0&&!c.heartbeat_seen&&!c.sequence);
    CHECK(!strcmp(send(&c,"ARM 77 3 31",201),"session_or_sequence"));
    CHECK(!strcmp(send(&c,"HELLO",202),"OK"));
    CHECK(c.enabled==31&&!c.armed&&!c.heartbeat_seen);
    CHECK(!strcmp(send(&c,"ARM 77 1 31",203),"heartbeat_required"));
    ready(&c);r3_fault(&c,2);CHECK(c.fault_latched&&c.enabled==29&&c.fault_axes==2);
    CHECK(!strcmp(send(&c,"ARM 77 3 1",200),"fault_latched"));
    CHECK(!strcmp(send(&c,"DISARM 77 4",200),"OK"));CHECK(c.enabled==0);
    CHECK(!strcmp(send(&c,"CHECK 77 5",200),"CHECK"));CHECK(c.verified==0);
    unsigned previous=0;
    for(unsigned ma=31;ma<=979;ma++){
        unsigned scale,actual;CHECK(r3_current_scale(ma,&scale,&actual));CHECK(scale<=31&&actual<=ma&&actual>=previous);previous=actual;
    }
    unsigned cs,actual;CHECK(!r3_current_scale(30,&cs,&actual));CHECK(!r3_current_scale(980,&cs,&actual));
    CHECK(r3_current_scale(707,&cs,&actual));CHECK(cs==22&&actual<=707);
    CHECK(r3_current_scale(354,&cs,&actual));CHECK(cs==10&&actual<=354);
    // Deterministic trajectory sweep: count/sign conservation and no rearming
    // after an asynchronously observed HOLD at every tested point.
    for(unsigned sample=0;sample<100;sample++){
        ready(&c);p=parse("MOVE 77 3 1000 1 1 1 1 1");
        for(unsigned a=0;a<5;a++)p.step[a]=(int32_t)((sample*37+a*19)%400)-200;
        CHECK(!strcmp(r3_command(&c,&p,100,77),"OK"));
        for(uint32_t t=1;t<=20002;t++){
            if(t%1000==0)c.heartbeat_us=100+t*50;
            r3_tick(&c,100+t*50,false,true);
        }
        for(unsigned a=0;a<5;a++)CHECK(c.commanded[a]==p.step[a]);
        CHECK(!c.moving);
    }
    for(unsigned stop=1;stop<1000;stop+=17){
        ready(&c);send(&c,"MOVE 77 3 1000 100 0 0 0 0",100);
        for(unsigned t=1;t<stop;t++)r3_tick(&c,100+t*50,false,true);
        r3_tick(&c,100+stop*50,true,true);int64_t position=c.commanded[0];
        for(unsigned t=stop+1;t<stop+500;t++)r3_tick(&c,100+t*50,false,true);
        CHECK(c.commanded[0]==position&&!c.armed&&!c.moving&&!c.pulse_mask);
    }
    /* Reject a previous boot/session's command after a fresh handshake. */
    ready(&c);p=parse("HELLO");
    uint32_t new_nonce=r3_session_nonce(0x12345678u,c.session);
    CHECK(!strcmp(r3_command(&c,&p,200,new_nonce),"OK"));
    CHECK(!c.armed&&!c.moving&&!c.pulse_mask&&!c.heartbeat_seen);
    p=parse("MOVE 77 3 1000 10 0 0 0 0");
    CHECK(!strcmp(r3_command(&c,&p,201,1),"session_or_sequence"));
    CHECK(!c.moving);

    /* Heartbeat logic remains bounded across the71-minute microsecond wrap. */
    ready(&c);c.heartbeat_us=UINT32_MAX-100u;
    r3_tick(&c,10,false,true);CHECK(c.armed);
    r3_tick(&c,(uint32_t)(c.heartbeat_us+R3_HEARTBEAT_US+1u),false,true);
    CHECK(!c.armed&&!c.moving&&!c.pulse_mask&&c.enabled==31);

    /* Timing/current limits: minimumduration,maximumcounts,longestduration. */
    const unsigned durations[]={50,100,4000,30000};
    const unsigned magnitudes[]={25,50,2000,2000};
    for(unsigned sample=0;sample<4;sample++){
        ready(&c);p=parse("MOVE 77 3 1000 1 -1 1 -1 1");
        p.duration_ms=durations[sample];
        for(unsigned a=0;a<5;a++)p.step[a]=(a&1)?-(int32_t)magnitudes[sample]:(int32_t)magnitudes[sample];
        CHECK(!strcmp(r3_command(&c,&p,100,77),"OK"));
        unsigned total=durations[sample]*1000u/R3_TICK_US;
        uint8_t last=0;bool adjacent=false;
        uint32_t last_tick[5]={0};unsigned minimum_gap=UINT_MAX;
        for(unsigned t=1;t<=total+2;t++){
            if(t%1000==0)c.heartbeat_us=100+t*R3_TICK_US;
            r3_tick(&c,100+t*R3_TICK_US,false,true);
            adjacent|=(last&c.pulse_mask)!=0;last=c.pulse_mask;
            for(unsigned a=0;a<5;a++)if(c.pulse_mask&(1u<<a)){
                if(last_tick[a]&&t-last_tick[a]<minimum_gap)minimum_gap=t-last_tick[a];
                last_tick[a]=t;
            }
        }
        CHECK(!adjacent&&minimum_gap>=32u);
        CHECK(!c.moving&&c.armed&&!c.pulse_mask);
        for(unsigned a=0;a<5;a++)CHECK(c.commanded[a]==p.step[a]);
    }
    printf("PASS %u portable checks; no GPIO/USB/UART hardware exercised\n",checks);
    return 0;
}
