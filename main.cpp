/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial API and implementation and/or initial documentation
 *******************************************************************************/

/**
 This is a sample program to illustrate the use of the MQTT Client library
 on the mbed platform.  The Client class requires two classes which mediate
 access to system interfaces for networking and timing.  As long as these two
 classes provide the required public programming interfaces, it does not matter
 what facilities they use underneath. In this program, they use the mbed
 system libraries.

*/


#include "MQTTESP8266.h"
#include "MQTTClient.h"

int arrivedcount = 0;

// callback for subscribe topic
void subscribeCallback(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    printf("Message received: qos %d, retained %d, dup %d, packetid %d\n", message.qos, message.retained, message.dup, message.id);
    printf("Payload %.*s\n", message.payloadlen, (char*)message.payload);
    ++arrivedcount;
}

// main function that sets up the subscription and makes a couple publishes
int main(int argc, char* argv[])
{
    printf("Starting\n");
    MQTTESP8266 ipstack(PA_2, PA_3, D10, "nurulfarhain@unifi","farhainnadzmy"); // change to match your wifi access point
    float version = 0.47;
    char* topic = "abcTest";

    printf("Version is %f\n", version);

    MQTT::Client<MQTTESP8266, Countdown> client = MQTT::Client<MQTTESP8266, Countdown>(ipstack);

    char* hostname = "192.168.0.3"; // test.mosquitto.org
    int port = 1883;
    int rc = ipstack.connect(hostname, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
   // data.clientID.cstring = "mbed-clientID";
   // data.username.cstring = "testuser";
   // data.password.cstring = "testpassword";
    if ((rc = client.connect(data)) != 0)
        printf("rc from MQTT connect is %d\n", rc);

    if ((rc = client.subscribe(topic, MQTT::QOS1, subscribeCallback)) != 0)
        printf("Recv'd from MQTT subscribe is %d\n", rc);

    MQTT::Message message;

    // QoS 0
    char buf[100];
    sprintf(buf, "Hello World!  QoS 0 message from app version %f\n", version);
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)buf;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 1)
        client.yield(100);

    // QoS 1
    sprintf(buf, "Hello World!  QoS 1 message from app version %f\n", version);
    message.qos = MQTT::QOS1;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 2)
        client.yield(100);

    // QoS 2
    sprintf(buf, "Hello World!  QoS 2 message from app version %f\n", version);
    message.qos = MQTT::QOS2;
    message.payloadlen = strlen(buf)+1;
    rc = client.publish(topic, message);
    while (arrivedcount < 3)
        client.yield(100);

    // n * QoS 2
    for (int i = 1; i <= 10; ++i) {
        sprintf(buf, "Hello World!  QoS 2 message number %d from app version %f\n", i, version);
        message.qos = MQTT::QOS2;
        message.payloadlen = strlen(buf)+1;
        rc = client.publish(topic, message);
        while (arrivedcount < i + 3)
            client.yield(100);
    }

    if ((rc = client.unsubscribe(topic)) != 0)
        printf("rc from unsubscribe was %d\n", rc);

    if ((rc = client.disconnect()) != 0)
        printf("rc from disconnect was %d\n", rc);

    ipstack.disconnect();
    printf("Finishing with %d messages received\n", arrivedcount);

    return 0;
}
