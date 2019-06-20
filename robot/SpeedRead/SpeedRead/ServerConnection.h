#ifndef _SERVERCONNECTION_H
#define _SERVERCONNECTION_H

void wifi_init();
void send_data(String);
void send_update(double px, double py, double v, double th);

#endif
