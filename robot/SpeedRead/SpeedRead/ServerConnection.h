#ifndef _SERVERCONNECTION_H
#define _SERVERCONNECTION_H

#include "constants.h"
#include <ArduinoJson.h>

void wifi_init();
void send_data(String);
void send_update(double px, double py, double v, double th);
void send_json(StaticJsonDocument<JSON_BYTE_SIZE> &doc, StaticJsonDocument<JSON_BYTE_SIZE> &obs);

#endif
