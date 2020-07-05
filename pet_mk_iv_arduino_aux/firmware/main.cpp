#include <Arduino.h>
#include <stdint.h>

#include "ros.h"
#include <ros/time.h>
#include <ros/duration.h>

#include "timer.h"
#include "chatter.h"

static const ros::Duration kChatterInterval(1, 0);    // sec, nsec

pet::ros::NodeHandle nh;
Timer<1> timer(nh);

void setup() 
{
    nh.initNode();

    while (!nh.connected())
    {
        nh.spinOnce();
    }

    nh.loginfo("Arduino Aux starting...");

    chatterSetup();

    // We need to renegotiate topics after setup-calls since new publishers/subscribers is registered.
    nh.negotiateTopics();

    // Ensure time is synced before continuing.
    uint32_t last_sync_time = nh.get_last_sync_receive_time();
    nh.requestSyncTime();
    while (last_sync_time == nh.get_last_sync_receive_time())
    {
        nh.spinOnce();
    }

    timer.register_callback(chatterUpdate, kChatterInterval);

    nh.loginfo("Arduino Aux setup done!");
}

void loop() 
{ 
    nh.spinOnce();
    timer.spin_once();
}
