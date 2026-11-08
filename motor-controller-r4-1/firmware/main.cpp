#include "core.h"
#include <cstdio>
#include "pico/stdlib.h"
#include "pico/stdio_usb.h"
#include "pico/rand.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/critical_section.h"

#ifndef SEIZO_R4_COMMISSIONING_UNLOCKED
#define SEIZO_R4_COMMISSIONING_UNLOCKED 0
#endif

namespace {
constexpr unsigned step_pin[5]={6,7,8,9,10};
constexpr unsigned dir_pin[5]={11,12,13,14,15};
constexpr unsigned enn_pin[5]={2,3,26,27,28};
constexpr unsigned hold_pin=29;
constexpr uint32_t step_gpio_mask=(31u<<6);
constexpr uint32_t dir_gpio_mask=(31u<<11);
constexpr unsigned initial_rms_ma[5]={707,707,354,354,354};
constexpr uint32_t gconf=(1u<<2)|(1u<<6)|(1u<<7)|(1u<<8);
// 16 real microsteps; interpolation OFF; SpreadCycle; low Vsense=.180V.
constexpr uint32_t chopconf=4u|(4u<<4)|(1u<<7)|(2u<<15)|(1u<<17)|(4u<<24);
constexpr uint32_t critical_drv_status=0x3fu; // otpw,ot,s2ga/b,s2vsa/b.
R3Core core;
critical_section_t core_lock;
volatile bool hold_irq_seen=false;
volatile bool usb_present=false;
uint32_t reported_status[5]{};
unsigned actual_rms_ma[5]{};
bool uart_read_ready=false;

void write_outputs() {
    gpio_put_masked(step_gpio_mask,(uint32_t)(core.pulse_mask&SEIZO_R41_POPULATED_AXIS_MASK)<<6);
    gpio_put_masked(dir_gpio_mask,(uint32_t)(core.dir_mask&SEIZO_R41_POPULATED_AXIS_MASK)<<11);
    for(unsigned a=0;a<5;a++)gpio_put(enn_pin[a],!(r41_axis_populated(a)&&(core.enabled&(1u<<a))));
}
void hold_irq(unsigned gpio,uint32_t events) {
    if(gpio==hold_pin&&(events&GPIO_IRQ_EDGE_RISE)){
        gpio_clr_mask(step_gpio_mask);hold_irq_seen=true;
    }
}
bool tick(repeating_timer_t *) {
    critical_section_enter_blocking(&core_lock);
    if(hold_irq_seen){r3_stop(&core);hold_irq_seen=false;}
    r3_tick(&core,time_us_32(),gpio_get(hold_pin),usb_present);
    write_outputs();
    critical_section_exit(&core_lock);
    return true;
}
uart_inst_t *bus(unsigned axis){return axis<4?uart0:uart1;}
uint8_t address(unsigned axis){return axis<4?(uint8_t)axis:0;}
void drain(uart_inst_t *u){while(uart_is_readable(u))(void)uart_getc(u);}
bool read_register(unsigned axis,uint8_t reg,uint32_t &value) {
    if(!r41_axis_populated(axis)||!uart_read_ready)return false;
    auto *u=bus(axis);drain(u);
    uint8_t request[4]={5,address(axis),(uint8_t)(reg&0x7f),0};request[3]=r3_crc(request,3);
    uart_write_blocking(u,request,4);uart_tx_wait_blocking(u);
    uint8_t rx[32];size_t count=0;uint32_t deadline=time_us_32()+4000;
    while((int32_t)(time_us_32()-deadline)<0){
        if(uart_is_readable(u)){
            uint8_t b=(uint8_t)uart_getc(u);
            if(count==sizeof rx){for(size_t i=1;i<count;i++)rx[i-1]=rx[i];count--;}
            rx[count++]=b;
            if(count>=8){
                auto *p=rx+count-8;
                if(p[0]==5&&p[1]==0xff&&p[2]==(reg&0x7f)&&r3_crc(p,7)==p[7]){
                    value=((uint32_t)p[3]<<24)|((uint32_t)p[4]<<16)|((uint32_t)p[5]<<8)|p[6];
                    // The addressed driver drives UART for four bit-times
                    // after the final reply bit; do not overlap the next TX.
                    sleep_us(R4_UART_IDLE_US);return true;
                }
            }
        }else tight_loop_contents();
    }
    // Finish a malformed/partial reply with a fresh recovery-idle interval.
    sleep_us(R4_UART_IDLE_US);drain(u);return false;
}
void write_register(unsigned axis,uint8_t reg,uint32_t value) {
    if(!r41_axis_populated(axis))return;
    auto *u=bus(axis);drain(u);
    uint8_t data[8]={5,address(axis),(uint8_t)(reg|0x80),(uint8_t)(value>>24),
                    (uint8_t)(value>>16),(uint8_t)(value>>8),(uint8_t)value,0};
    data[7]=r3_crc(data,7);uart_write_blocking(u,data,8);uart_tx_wait_blocking(u);sleep_us(R4_UART_IDLE_US);
}
void prepare_uart_reads() {
    // Every read batch starts with writes to every node. This also covers
    // ARM/MOVE checks before CHECK and a node whose power reset restored
    // SENDDELAY0 since the preceding batch; GSTAT reset flags remain intact.
    uart_read_ready=false;
    for(unsigned a=0;a<5;a++){
        if(!r41_axis_populated(a))continue;
        watchdog_update();
        write_register(a,R4_TMC_SENDDELAY_REGISTER,R4_TMC_SENDDELAY_VALUE);
    }
    uart_read_ready=true;
}
bool configure_axis(unsigned a) {
    if(!r41_axis_populated(a))return false;
    uint32_t ioin, before, after, check, stat, gst;
    unsigned cs=0;
    if(!r3_current_scale(initial_rms_ma[a],&cs,&actual_rms_ma[a]))return false;
    if(!read_register(a,6,ioin)||(ioin>>24)!=0x21||!(ioin&1u))return false;
    if(!read_register(a,2,before))return false;
    uint32_t current=cs|(cs<<8)|(6u<<16); // IHOLD equals IRUN: no automatic torque reduction.
    // Register0x03 is write-only. Include this second SENDDELAY write in
    // the measured IFCNT delta instead of inventing a readable register.
    write_register(a,R4_TMC_SENDDELAY_REGISTER,R4_TMC_SENDDELAY_VALUE);
    write_register(a,0,gconf);
    write_register(a,0x10,current);
    write_register(a,0x11,255);
    write_register(a,0x6c,chopconf);
    write_register(a,0x22,0); // VACTUAL=0, STEP/DIR is the only pulse source.
    write_register(a,1,7);   // Explicit operator CHECK only; clear startup/history flags.
    if(!read_register(a,2,after)||!r4_ifcnt_increment_matches(before,after,R4_TMC_VERIFIED_CONFIG_WRITES))return false;
    if(!read_register(a,0,check)||(check&0x1ffu)!=gconf)return false;
    if(!read_register(a,0x10,check)||(check&0xf1f1fu)!=current)return false;
    if(!read_register(a,0x6c,check)||check!=chopconf)return false;
    if(!read_register(a,0x22,check)||check!=0)return false;
    if(!read_register(a,0x6f,stat)||(stat&critical_drv_status))return false;
    if(!read_register(a,1,gst)||(gst&7u))return false;
    reported_status[a]=stat;return true;
}
void configure_all() {
    // Configure every node before the first read on either shared bus.
    // A reset module otherwise defaults to SENDDELAY0, which the datasheet
    // explicitly excludes from multi-node operation. Writes need no reply.
    prepare_uart_reads();
    uint8_t good=0;
    for(unsigned a=0;a<5;a++){
        if(!r41_axis_populated(a))continue;
        watchdog_update();
        if(configure_axis(a))good|=(uint8_t)(1u<<a);
    }
    critical_section_enter_blocking(&core_lock);
    core.verified=good;
    if(good==SEIZO_R41_POPULATED_AXIS_MASK){core.fault_latched=false;core.fault_axes=0;}
    else r3_fault(&core,(uint8_t)(SEIZO_R41_POPULATED_AXIS_MASK^good));
    write_outputs();critical_section_exit(&core_lock);
}
void poll_drivers(bool check_configuration=false) {
    prepare_uart_reads();
    uint8_t bad=0;
    for(unsigned a=0;a<5;a++){
        if(!r41_axis_populated(a))continue;
        uint32_t status,gstat;
        bool ok=read_register(a,0x6f,status)&&read_register(a,1,gstat)&&!(status&critical_drv_status)&&!(gstat&7u);
        if(ok&&check_configuration){
            uint32_t gc,cc,ih;unsigned cs,actual;
            ok=r3_current_scale(initial_rms_ma[a],&cs,&actual)&&read_register(a,0,gc)&&gc==gconf
               &&read_register(a,0x6c,cc)&&cc==chopconf&&read_register(a,0x10,ih)
               &&(ih&0xf1f1fu)==(cs|(cs<<8)|(6u<<16));
        }
        if(!ok)bad|=(uint8_t)(1u<<a);else reported_status[a]=status;
    }
    if(bad){
        critical_section_enter_blocking(&core_lock);r3_fault(&core,bad);write_outputs();critical_section_exit(&core_lock);
    }
}
void status(const char *result) {
    R3Core snapshot;
    critical_section_enter_blocking(&core_lock);snapshot=core;critical_section_exit(&core_lock);
    printf("{\"result\":\"%s\",\"revision\":\"R4.1 three-driver bench\",\"populated_mask\":%u,\"locked\":%s,\"session\":%lu,\"seq\":%lu,\"armed\":%s,\"moving\":%s,\"hold_open\":%s,\"verified_mask\":%u,\"enabled_mask\":%u,\"fault_mask\":%u,\"fault_latched\":%s,\"position_known\":false,\"commanded_pulses\":[%lld,%lld,%lld,%lld,%lld],\"rms_mA\":[%u,%u,%u,%u,%u]}\n",
      result,(unsigned)SEIZO_R41_POPULATED_AXIS_MASK,snapshot.locked?"true":"false",(unsigned long)snapshot.session,(unsigned long)snapshot.sequence,
      snapshot.armed?"true":"false",snapshot.moving?"true":"false",snapshot.hold_open?"true":"false",
      snapshot.verified,snapshot.enabled,snapshot.fault_axes,snapshot.fault_latched?"true":"false",
      snapshot.commanded[0],snapshot.commanded[1],snapshot.commanded[2],snapshot.commanded[3],snapshot.commanded[4],
      actual_rms_ma[0],actual_rms_ma[1],actual_rms_ma[2],actual_rms_ma[3],actual_rms_ma[4]);
}
void command(const char *line,size_t size) {
    R3Command parsed{};
    if(!r3_parse(line,size,&parsed)){status("invalid_command");return;}
    if(parsed.kind==R3_ARM||parsed.kind==R3_MOVE)poll_drivers(parsed.kind==R3_ARM);
    uint32_t next=1;
    if(parsed.kind==R3_HELLO){
        critical_section_enter_blocking(&core_lock);
        uint32_t previous=core.session;
        critical_section_exit(&core_lock);
        next=r3_session_nonce(get_rand_32(),previous);
    }
    critical_section_enter_blocking(&core_lock);
    core.hold_open=gpio_get(hold_pin);
    const char *result=r3_command(&core,&parsed,time_us_32(),next);
    write_outputs();critical_section_exit(&core_lock);
    if(!strcmp(result,"CHECK")){
        configure_all();
        critical_section_enter_blocking(&core_lock);
        bool good=core.verified==SEIZO_R41_POPULATED_AXIS_MASK&&!core.fault_latched;
        critical_section_exit(&core_lock);
        result=good?"check_complete":"check_failed";
    }
    status(result);
}
}
int main() {
    critical_section_init(&core_lock);
    r3_init(&core,!SEIZO_R4_COMMISSIONING_UNLOCKED);
    for(unsigned pin:enn_pin){gpio_init(pin);gpio_put(pin,1);gpio_set_dir(pin,GPIO_OUT);}
    for(unsigned pin:step_pin){gpio_init(pin);gpio_put(pin,0);gpio_set_dir(pin,GPIO_OUT);}
    for(unsigned pin:dir_pin){gpio_init(pin);gpio_put(pin,0);gpio_set_dir(pin,GPIO_OUT);}
    gpio_init(hold_pin);gpio_set_dir(hold_pin,GPIO_IN);gpio_pull_up(hold_pin);
    gpio_set_irq_enabled_with_callback(hold_pin,GPIO_IRQ_EDGE_RISE,true,hold_irq);
    uart_init(uart0,R4_UART_BAUD);uart_init(uart1,R4_UART_BAUD);
    constexpr unsigned uart_pins[4]={0,1,4,5};
    for(unsigned pin:uart_pins)gpio_set_function(pin,GPIO_FUNC_UART);
    uart_set_hw_flow(uart0,false,false);uart_set_hw_flow(uart1,false,false);
    uart_set_format(uart0,8,1,UART_PARITY_NONE);uart_set_format(uart1,8,1,UART_PARITY_NONE);
    stdio_init_all();
    repeating_timer_t timer;
    if(!add_repeating_timer_us(-(int32_t)R3_TICK_US,tick,nullptr,&timer))while(true)tight_loop_contents();
    watchdog_enable(1000,true);
    char line[R3_MAX_LINE+1];size_t length=0;bool overflow=false,was_connected=false;uint32_t poll_at=0;
    while(true){
        watchdog_update();usb_present=stdio_usb_connected();
        if(was_connected&&!usb_present){length=0;overflow=false;}
        was_connected=usb_present;
        int ch=usb_present?getchar_timeout_us(0):-1;
        if(ch>=0){
            if(ch=='\n'){
                if(overflow)status("line_too_long");else command(line,length);
                length=0;overflow=false;
            }else if(length<R3_MAX_LINE&&!overflow)line[length++]=(char)ch;
            else {overflow=true;length=0;}
        }
        uint32_t now=time_us_32();
        if((uint32_t)(now-poll_at)>=50000){
            poll_at=now;
            bool verified;
            critical_section_enter_blocking(&core_lock);verified=core.verified!=0;critical_section_exit(&core_lock);
            if(verified)poll_drivers();
        }
        tight_loop_contents();
    }
}
