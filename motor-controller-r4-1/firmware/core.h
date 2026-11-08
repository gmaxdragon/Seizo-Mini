#ifndef SEIZO_R3_CORE_H
#define SEIZO_R3_CORE_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

/* Portable, bounded bench-motion core. No GPIO, heap or USB side effects. */
#define R3_AXES 5
/* Physical sockets remain five. This candidate populates U1/U2/U3 only. */
#ifndef SEIZO_R41_POPULATED_AXIS_MASK
#define SEIZO_R41_POPULATED_AXIS_MASK 7u /* 0b00111 */
#endif
#if SEIZO_R41_POPULATED_AXIS_MASK < 1 || SEIZO_R41_POPULATED_AXIS_MASK > 31
#error "Populated axis mask must select at least one of the five physical slots"
#endif
static inline bool r41_axis_populated(unsigned axis) {
    return axis<R3_AXES && (SEIZO_R41_POPULATED_AXIS_MASK&(1u<<axis))!=0;
}
static inline bool r41_mask_populated(uint32_t mask) {
    return mask!=0 && (mask&~(uint32_t)SEIZO_R41_POPULATED_AXIS_MASK)==0;
}
#define R3_TICK_US 50u
#define R3_HEARTBEAT_US 500000u
#define R3_MAX_LINE 160u
#define R3_MAX_STEP 2000
#define R3_MAX_RATE 500u

/* TMC2209 multi-node UART requirements: SENDDELAY>=2 and at least four
 * bit-times after a reply before the slave releases its transmitter. The
 * 150us quiet interval also exceeds the12-bit error-recovery idle time. */
#define R4_UART_BAUD 115200u
#define R4_UART_IDLE_US 150u
#define R4_TMC_SENDDELAY_REGISTER 0x03u
#define R4_TMC_SENDDELAY_VALUE (2u<<8)
#define R4_TMC_VERIFIED_CONFIG_WRITES 7u

static inline bool r4_ifcnt_increment_matches(uint32_t before,uint32_t after,unsigned writes){
    return writes<256u&&((after-before)&255u)==writes;
}

typedef enum { R3_STATUS, R3_HELLO, R3_PING, R3_CHECK, R3_ARM,
               R3_MOVE, R3_HOLD, R3_DISARM } R3Kind;
typedef struct { R3Kind kind; uint32_t session, seq, mask, duration_ms; int32_t step[5]; } R3Command;
typedef struct {
    bool locked, armed, moving, hold_open, fault_latched, heartbeat_seen;
    uint8_t verified, enabled, fault_axes, dir_mask, pulse_mask;
    uint32_t session, sequence, heartbeat_us, elapsed, total, ramp;
    uint32_t count[5], emitted[5];
    int64_t commanded[5];
} R3Core;

static inline uint8_t r3_crc(const uint8_t *p, size_t size) {
    uint8_t crc=0;
    for(size_t i=0;i<size;i++) {
        uint8_t v=p[i];
        for(unsigned bit=0;bit<8;bit++) {
            crc=(uint8_t)((crc<<1)^(((crc>>7)^(v&1))?7:0)); v>>=1;
        }
    }
    return crc;
}

static inline bool r3_number(const char *p,size_t n,int32_t *out) {
    if(!n)return false;
    bool neg=false; size_t i=0; uint64_t v=0;
    if(p[0]=='-'||p[0]=='+'){neg=p[0]=='-';if(++i==n)return false;}
    for(;i<n;i++){
        if(p[i]<'0'||p[i]>'9')return false;
        v=v*10+(unsigned)(p[i]-'0');
        if(v>((uint64_t)INT32_MAX+(neg?1u:0u)))return false;
    }
    *out=neg?(int32_t)(-(int64_t)v):(int32_t)v;return true;
}

static inline bool r3_parse(const char *s,size_t n,R3Command *out) {
    if(!s||!n||n>R3_MAX_LINE)return false;
    const char *t[10];size_t l[10],k=0,i=0;
    while(i<n){
        if(s[i]==' '||s[i]=='\t'){i++;continue;}
        if(s[i]=='\r'&&i+1==n){i++;continue;}
        if(k==10)return false;
        size_t start=i;
        while(i<n&&s[i]!=' '&&s[i]!='\t'&&s[i]!='\r'){
            if((unsigned char)s[i]<33||(unsigned char)s[i]>126)return false;
            i++;
        }
        if(start==i)return false;
        t[k]=s+start;l[k++]=i-start;
        if(i<n&&s[i]=='\r'&&i+1!=n)return false;
    }
    if(!k)return false;
    R3Command c;memset(&c,0,sizeof c);
#define R3_WORD(w) (l[0]==sizeof(w)-1&&!memcmp(t[0],w,sizeof(w)-1))
    if(k==1){
        if(R3_WORD("STATUS"))c.kind=R3_STATUS;
        else if(R3_WORD("HELLO"))c.kind=R3_HELLO;
        else if(R3_WORD("HOLD"))c.kind=R3_HOLD;
        else return false;
    } else {
        if(k<3)return false;
        int32_t session,seq;
        if(!r3_number(t[1],l[1],&session)||!r3_number(t[2],l[2],&seq)||session<1||seq<1)return false;
        c.session=(uint32_t)session;c.seq=(uint32_t)seq;
        if(k==3){
            if(R3_WORD("PING"))c.kind=R3_PING;
            else if(R3_WORD("CHECK"))c.kind=R3_CHECK;
            else if(R3_WORD("DISARM"))c.kind=R3_DISARM;
            else return false;
        } else if(k==4&&R3_WORD("ARM")){
            int32_t mask;if(!r3_number(t[3],l[3],&mask)||mask<1||mask>31)return false;
            c.kind=R3_ARM;c.mask=(uint32_t)mask;
        } else if(k==9&&R3_WORD("MOVE")){
            int32_t ms;if(!r3_number(t[3],l[3],&ms)||ms<50||ms>30000)return false;
            c.kind=R3_MOVE;c.duration_ms=(uint32_t)ms;
            bool any=false;
            for(unsigned a=0;a<5;a++){
                if(!r3_number(t[4+a],l[4+a],&c.step[a])||c.step[a]<-R3_MAX_STEP||c.step[a]>R3_MAX_STEP)return false;
                uint32_t mag=(uint32_t)(c.step[a]<0?-c.step[a]:c.step[a]);
                if((uint64_t)mag*1000u>(uint64_t)c.duration_ms*R3_MAX_RATE)return false;
                any|=mag!=0;
            }
            if(!any)return false;
        } else return false;
    }
#undef R3_WORD
    *out=c;return true;
}

static inline void r3_init(R3Core *c,bool locked){memset(c,0,sizeof *c);c->locked=locked;}
/* Preserve the positive31-bit wire format while rejecting zero and immediate
 * reuse. The caller supplies SDK entropy; this is not network authentication. */
static inline uint32_t r3_session_nonce(uint32_t entropy,uint32_t previous){
    uint32_t next=entropy&0x7fffffffu;
    if(!next||next==previous)next=previous>=0x7fffffffu?1u:previous+1u;
    return next;
}
static inline void r3_stop(R3Core *c){c->moving=false;c->armed=false;c->pulse_mask=0;}
static inline void r3_session_lost(R3Core *c){
    r3_stop(c);c->session=0;c->sequence=0;c->heartbeat_seen=false;
    /* Keep healthy holding-current requests; fresh HELLO/PING/ARM is required. */
}
static inline void r3_fault(R3Core *c,uint8_t axes){r3_stop(c);c->fault_latched=true;c->fault_axes|=axes;c->enabled&=(uint8_t)~axes;c->verified&=(uint8_t)~axes;}
static inline bool r3_fresh(const R3Core *c,uint32_t now){return c->heartbeat_seen&&(uint32_t)(now-c->heartbeat_us)<=R3_HEARTBEAT_US;}

/* CHECK performs I/O outside this core, only while every output is disabled. */
static inline const char *r3_command(R3Core *c,const R3Command *p,uint32_t now,uint32_t new_session){
    if(p->kind==R3_STATUS)return "OK";
    if(p->kind==R3_HOLD){r3_stop(c);return "OK";}
    if(p->kind==R3_HELLO){
        r3_stop(c);c->session=new_session?new_session:1;c->sequence=0;c->heartbeat_seen=false;return "OK";
    }
    if(p->session!=c->session||p->seq<=c->sequence)return "session_or_sequence";
    c->sequence=p->seq;
    if(p->kind==R3_PING){c->heartbeat_seen=true;c->heartbeat_us=now;return "OK";}
    if(p->kind==R3_DISARM){r3_stop(c);c->enabled=0;return "OK";}
    if(p->kind==R3_CHECK){
        if(c->enabled||c->moving)return "disable_before_check";
        c->verified=0;return "CHECK";
    }
    if(p->kind==R3_ARM&&!r41_mask_populated(p->mask))return "axis_unpopulated";
    if(p->kind==R3_MOVE)for(unsigned a=0;a<R3_AXES;a++)
        if(p->step[a]&&!r41_axis_populated(a))return "axis_unpopulated";
    if(c->locked)return "commissioning_locked";
    if(c->hold_open)return "hold_open";
    if(c->fault_latched)return "fault_latched";
    if(!r3_fresh(c,now))return "heartbeat_required";
    if(c->verified!=SEIZO_R41_POPULATED_AXIS_MASK)return "driver_check_required";
    if(p->kind==R3_ARM){
        if(c->moving||c->pulse_mask)return "busy";
        c->enabled=(uint8_t)p->mask;c->armed=true;return "OK";
    }
    if(p->kind==R3_MOVE){
        if(!c->armed||c->moving||c->pulse_mask)return "not_armed_or_busy";
        for(unsigned a=0;a<5;a++)if(p->step[a]&&!(c->enabled&(1u<<a)))return "axis_not_enabled";
        c->elapsed=0;c->total=p->duration_ms*1000u/R3_TICK_US;c->ramp=c->total/5;c->dir_mask=0;c->pulse_mask=0;
        for(unsigned a=0;a<5;a++){
            c->count[a]=(uint32_t)(p->step[a]<0?-p->step[a]:p->step[a]);c->emitted[a]=0;
            if(p->step[a]>=0)c->dir_mask|=(uint8_t)(1u<<a);
        }
        c->moving=true;return "OK";
    }
    return "invalid";
}

/* 50us pulses with a zero-output tick between pulses. The 500steps/s
 * average cap also bounds the trapezoid peak to625steps/s. */
static inline void r3_tick(R3Core *c,uint32_t now,bool hold,bool usb){
    c->hold_open=hold;
    if(!usb||((c->armed||c->moving)&&!r3_fresh(c,now))){r3_session_lost(c);return;}
    if(hold){r3_stop(c);return;}
    bool prior_pulse=c->pulse_mask!=0;c->pulse_mask=0;
    if(!c->moving)return;
    if(c->elapsed<c->total)c->elapsed++;
    uint64_t t=c->elapsed,a=c->ramp,n=c->total,den=2*a*(n-a),num;
    if(t<a)num=t*t;
    else if(t<=n-a)num=2*a*t-a*a;
    else num=den-(n-t)*(n-t);
    for(unsigned axis=0;axis<5;axis++){
        if(!r41_axis_populated(axis))continue;
        uint32_t target=(uint32_t)((uint64_t)c->count[axis]*num/den);
        if(target>c->emitted[axis]&&!prior_pulse){
            c->pulse_mask|=(uint8_t)(1u<<axis);c->emitted[axis]++;
            c->commanded[axis]+=(c->dir_mask&(1u<<axis))?1:-1;
        }
    }
    bool done=c->elapsed==c->total;
    for(unsigned axis=0;axis<5;axis++)done&=c->emitted[axis]==c->count[axis];
    if(done)c->moving=false;
}

/* Returns a current scale rounded DOWN. rsense includes chip20milliohm term.
 * Safe low-current range uses Vsense=.180V and never silently clamps upward. */
static inline bool r3_current_scale(unsigned rms_ma,unsigned *cs,unsigned *actual_ma){
    if(rms_ma<31||rms_ma>979)return false;
    unsigned scale=(unsigned)((uint64_t)rms_ma*32u*130u*1414u/(180000u*1000u));
    if(scale<1||scale>32)return false;
    *cs=scale-1;*actual_ma=(unsigned)((uint64_t)scale*180000u*1000u/(32u*130u*1414u));return true;
}
#endif
