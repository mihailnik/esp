
#include <AsyncMqttClient.h>
#ifdef ESP8266
#include <Ticker.h>
#else
#include <ESP32Ticker.h>
#endif
#include "Base64.h"

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

String mqtt_host;
int mqtt_port;
String mqtt_user;
String mqtt_pass;
String mqtt_id;

bool mqtt_connected;
String mqtt_tpc[20];
String mqtt_pld[20];
bool mqtt_sub[20];
int qos[20];

void connectToMqtt();
void onMqttConnect(bool sessionPresent);
void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
void mqtt_setup();

void onWifiConnect()
{
    DEBG(F("Connected to Wi-Fi."));
    connectToMqtt();
}

void onWifiDisconnect()
{
    DEBG(F("Disconnected from Wi-Fi."));
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
}

void connectToMqtt()
{
    static bool mqtt_stp;
    if(!mqtt_stp)
    {
        mqtt_setup();
        mqtt_stp = true;
    }
        
    DEBG(F("Connecting to MQTT..."));
    mqttClient.connect();
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    mqtt_connected = false;
    for (int i = 0; i < 20; i++)
        mqtt_sub[i] == false;

    DEBG(F("Disconnected from MQTT."));
    if (WiFi.isConnected())
    {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
    DEBG(String(F("Subscribe packetId: ")) + String(packetId) + String(F("  qos: ")) + String(qos));
}

void onMqttUnsubscribe(uint16_t packetId)
{
    DEBG(String(F("Unsubscribe.  packetId: ")) + String(packetId));
}

void onMqttPublish(uint16_t packetId)
{
    DEBG(String(F("Publish.   packetId: ")) + String(packetId));
}

void mqtt_setup()
{
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onSubscribe(onMqttSubscribe);
    mqttClient.onUnsubscribe(onMqttUnsubscribe);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    mqttClient.setCleanSession(true);
    mqttClient.setClientId(mqtt_id.c_str());
    mqttClient.setCredentials(mqtt_user.c_str(), mqtt_pass.c_str());
    mqttClient.setServer(mqtt_host.c_str(), mqtt_port);
}

void onMqttConnect(bool sessionPresent)
{
    mqtt_connected = true;
    DEBG(F("Connected to MQTT."));
    DEBG("Session present: " + String{sessionPresent});
    
    //mqttClient.publish("test", 0, true, "test_mes");
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    String msg = String(payload).substring(0, len);
    //String tpc = topic;
    //tpc.replace(param(F("mqtt_prefix")), "");

    DEBG("topic: " + String(topic));
    DEBG("payload: " + msg);

    for (int i = 0; i < 20; i++)
    {
        if (mqtt_tpc[i] == String(topic))
        {
            mqtt_pld[i] = msg;
            break;
        }  
    }
}

void mqtt_pub()
{

}

String sub_mqtt(String topic, int QoS)
{
    if(mqtt_host == "")
        return "";
    static bool mqtt_conn = false;
    int cnt;
    bool search = false;
    if(!get_ip)
        mqtt_conn = false;
    for (int i = 0; i < 20; i++)
    {
        if (mqtt_tpc[i] == topic)
        {
            cnt = i;
            search = true;
            break;
        }
    }
    if(!search)
    {
        for (int i = 0; i < 20; i++)
        {
            if (mqtt_tpc[i] == "")
            {
                mqtt_tpc[i] = topic;
                cnt = i;
                break;
            }
        }
    }
    DEBG("CNT: " + String(cnt) + " TPC: " + mqtt_tpc[cnt] + " PLD: " + mqtt_pld[cnt]);
    if (!mqtt_conn && get_ip)
    {
        onWifiConnect();
        mqtt_conn = true;
    }
    if (!mqtt_sub[cnt] && mqtt_connected)
    {
        mqttClient.subscribe(mqtt_tpc[cnt].c_str(), QoS);
        mqtt_sub[cnt] = true;
    }
    return mqtt_pld[cnt];
}

void mqtt_connect(String id, String host, int port, String user, String password)
{
    mqtt_id = id;
    mqtt_host = host;
    mqtt_port = port;
    mqtt_user = user;
    mqtt_pass = password;
}