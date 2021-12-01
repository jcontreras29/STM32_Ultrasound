
#ifndef INC_HARDWARE_H_
#define INC_HARDWARE_H_

void config_GPIO_pins(void);
void setup_timer2(void);
void setup_timer5(void);
void stop_timer2(void);
void stop_timer5(void);
void reset_timer2(void);
void reset_timer5(void);
int get_time2(void);
int get_time5(void);
void update_dutycycle5(int x);
uint32_t calc_time(void);
int calc_distance(int time);
void wait10(void);
void POST(void);
int sendReceive(void);

#endif /* INC_HARDWARE_H_ */
