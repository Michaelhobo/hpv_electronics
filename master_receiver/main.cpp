#include "mbed.h"
#include "xbee.h"
#include "TextLCD.h"
#include <string>
#include <sstream>
struct node;
using namespace std;
AnalogIn BrakeSensor(p15);
AnalogIn CadenceSensor(p16);
AnalogIn SpeedSensor(p17);
AnalogIn GearSensor(p18);
Timer t;
xbee xbee1(p9,p10,p11); //Initalise xbee_lib
Serial pc(USBTX, USBRX); //Initalise PC serial comms for debugging
TextLCD lcd(p26, p25, p24, p23, p22, p21); // rs, e, d4-d7


float current_time; //updated every iteration of the main loop

//------------------LINKED LIST IMPLEMENTATION-------------------\\

struct node {
    float item;
    node* next;
    node* prev;
    int len;
    };

void appendToList(node* sentinel,float time){
    sentinel->len += 1;
    node* new_node = new node;
    new_node->item = time;
    sentinel->prev->next = new_node;
    new_node->prev = sentinel->prev;
    sentinel->prev = new_node;
    new_node->next = sentinel;
}

float front(node* sentinel){
    return sentinel->next->item;
}

void deleteFront(node* sentinel){
    sentinel->len -= 1;
    sentinel->next = sentinel->next->next;
    if (sentinel->len <= 1){
        sentinel->prev = sentinel->next;
    }
}
//---------------END LINKED LIST IMPLEMENTATION-------------------\\




float brakeCutoff = 0.5;
bool brake(float brakeValue){
    return (brakeValue > brakeCutoff);
    //return (brakeValue > 0.02);
}

float stepSize = 0.1;
int gear(float gearValue){
    int gearReading = (int) (gearValue / stepSize);
    return gearReading;
}

int max_num_zeros = 1;
int window_time = 2.5;
int s_current_num_zeros = 0;
bool s_in_magnet_pass = false;
float WHEEL_RADIUS = -1; //fix
float speed(float speedValue,node* speed_sentinel){
   if (!s_in_magnet_pass){
        if (speedValue <= 0.002){
            s_current_num_zeros++;
            if (s_current_num_zeros > max_num_zeros){
                appendToList(speed_sentinel,current_time);
                s_in_magnet_pass = true;
                s_current_num_zeros = 0;
            }
        } else {
            s_current_num_zeros = 0;
        }
    } else if (speedValue != 0) {
        s_in_magnet_pass = false;
    }
    while (speed_sentinel->len > 0 && current_time - front(speed_sentinel) > window_time){
        deleteFront(speed_sentinel);
    }
    return WHEEL_RADIUS*6.28*speed_sentinel->len / window_time;
}
bool c_in_magnet_pass = false;
int c_current_num_zeros = 0;
float cadence(float cadValue,node* cad_sentinel){
    if (!c_in_magnet_pass){
        if (cadValue <= 0.002){
            c_current_num_zeros++;
            if (c_current_num_zeros > max_num_zeros){
                if (current_time == 0){
                    pc.printf("adding a 0");
                }
                appendToList(cad_sentinel,current_time);
                c_in_magnet_pass = true;
                c_current_num_zeros = 0;
            }
        } else {
            c_current_num_zeros = 0;
        }
    } else if (cadValue > 0.005) {
        c_in_magnet_pass = false;
    }
    while (cad_sentinel->len > 0 && current_time - front(cad_sentinel) > window_time){
        pc.printf("updating: now %f ",current_time);
        pc.printf(" deleting %f\n",front(cad_sentinel));
        deleteFront(cad_sentinel);
    }
    return 60*cad_sentinel->len / window_time;
}
int main() {
    t.start();
    set_time(0);
    node* speed_sentinel = new node;
    speed_sentinel->next = speed_sentinel;
    speed_sentinel->prev = speed_sentinel;
    speed_sentinel->len = 0;
    node* cad_sentinel = new node;
    cad_sentinel->next = cad_sentinel;
    cad_sentinel->prev = cad_sentinel;
    cad_sentinel->len = 0;
    char send_data[202];
    bool braking;
    float raw_gear; //int?
    int gear_val;
    float last_time = t.read();
    current_time = t.read();
    float cadence_val;
    float speed_val;
    float distance = 0;
    float distance_const = -1;
    while(1) {
        current_time = t.read();
        braking = brake(BrakeSensor.read());
        cadence_val = cadence(CadenceSensor.read(),cad_sentinel);
        speed_val = speed(SpeedSensor.read(),speed_sentinel);
        raw_gear = GearSensor.read();
        gear_val = gear(raw_gear);
        distance += speed_val * (current_time-last_time) * distance_const;
        
        //XBee Transmission
        sprintf(send_data,"c%f\n", cadence_val);
        xbee1.SendData(send_data);
        sprintf(send_data,"s%f\n", speed_val);
        xbee1.SendData(send_data);
        sprintf(send_data,"g%i\n",gear_val);
        xbee1.SendData(send_data);
        sprintf(send_data,"b%d\n",braking);
        xbee1.SendData(send_data);
        
        //write to LCD screen
        //if (t.read() > last_time + min_time_update){
            //format of the screen is
            //cadence          dist
            //______________________
            //|GEAR         CADENCE|
            //| XX            XXX  |
            //|        TIME        |
            //|       XXX:XX       |
            //|---------------------
            //gear              time
            lcd.cls();
            std::ostringstream format_data;
            format_data << "GEAR         CADENCE";
            //line 2
            format_data << " ";
            format_data.width(2);
            format_data << gear_val;
            format_data << "            ";
            format_data.width(3);
            format_data << cadence_val;
            format_data << "  ";
            //third line
            format_data << "        TIME        ";
            //fourth line
            format_data << "       ";
            int minutes = (int)last_time/60;
            format_data.width(3);
            format_data << minutes;
            format_data << ":";
            int seconds = ((int)last_time) % 60;
            format_data.width(2);
            format_data << seconds;
            format_data << "       ";
            lcd.printf(format_data.str().c_str());
        //}
        last_time = current_time;
    }
}
