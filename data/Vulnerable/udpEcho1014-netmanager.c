/**
 * File:   netmanager.c
 * Author: Ericson Joseph
 *
 * Created on September 27, 2018,  9:22 PM
 */

#include "netmanager.h"
#include "sys_time.h"
#include <string.h>

#define NUM_ROUTES 10
#define NUM_DEVICES 10
#define MAX_TIME_REMOVE 60000 // 60 deg

static device_t devicesList[NUM_DEVICES];
static rtp_route_t routeTable[NUM_ROUTES];

void NE_Init() {
    for (int i = 0; i < NUM_DEVICES; i++) {
        strcpy(devicesList[i].key, "");
        memset(&devicesList[i], 0, sizeof(struct sockaddr_in));
        devicesList[i].isFree = true;
        devicesList[i].time   = 0;
    }
}

bool NE_exist(char *key) {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (strcmp(devicesList[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

void NE_put(char *key, struct sockaddr_in addr) {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (devicesList[i].isFree) {
            strcpy(devicesList[i].key, key);
            devicesList[i].addr   = addr;
            devicesList[i].isFree = false;
            devicesList[i].time   = millis();
            return;
        }
    }
}

device_t *NE_get(char *key) {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (strcmp(devicesList[i].key, key) == 0) {
            return &devicesList[i];
        }
    }
    return NULL;
}

void NE_touch(char *key) {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (strcmp(devicesList[i].key, key) == 0) {
            devicesList[i].time = millis();
        }
    }
}

void NE_removeOlds() {
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (!devicesList[i].isFree) {
            long long int max_time = millis() - devicesList[i].time;
            if (max_time > MAX_TIME_REMOVE) {
                NE_remove(devicesList[i].key);
            }
        }
    }
}

void NE_remove(char *key) {
    char tmp_key[50];
    strcpy(tmp_key, key);
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (strcmp(devicesList[i].key, tmp_key) == 0) {
            strcpy(devicesList[i].key, "");
            memset(&devicesList[i], 0, sizeof(struct sockaddr_in));
            devicesList[i].isFree = true;
            devicesList[i].time   = 0;
            return;
        }
    }
}

void NE_initTableRoute() {

    for (int i = 0; i < NUM_ROUTES; ++i) {
        strcpy(routeTable[0].ne_a, "");
        strcpy(routeTable[0].ne_b, "");
    }

    strcpy(routeTable[0].ne_a, "4568@ericsonj.net");
    strcpy(routeTable[0].ne_b, "1001@ericsonj.net");
}

rtp_route_t *NE_getRoute(char *KEY_ne_a) {
    for (int i = 0; i < NUM_ROUTES; ++i) {
        if(strcmp(routeTable[i].ne_a, KEY_ne_a) == 0){
            return &routeTable[i];
        }
    }
    return NULL;
}

device_t *NE_getDscNe(struct sockaddr_in* addr){
    for (int i = 0; i < NUM_DEVICES; i++) {
        if (devicesList[i].addr.sin_addr.s_addr == addr->sin_addr.s_addr) {
            rtp_route_t *route = NE_getRoute(devicesList[i].key);
            if(route != NULL){
                device_t *dscNE = NE_get(route->ne_b);
                if(dscNE != NULL){
                    return dscNE;
                }
            }
        }
    }
    return NULL;

}


//						↓↓↓VULNERABLE LINES↓↓↓

// 21,8;21,14

// 40,12;40,18

// 79,4;79,10

// 82,12;82,18

// 94,8;94,14

// 95,8;95,14

// 98,4;98,10

// 99,4;99,10

