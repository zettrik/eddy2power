#!/usr/bin/env python3
"""
connect to mqtt broker, read a channel and write it to database...


Copyright 2017 Henning Rieger <age@systemausfall.org>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
der GNU General Public License, wie von der Free Software Foundation,
Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
veröffentlichten Version, weiterverbreiten und/oder modifizieren.

Dieses Programm wird in der Hoffnung, dass es nützlich sein wird, aber
OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
Siehe die GNU General Public License für weitere Details.

Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
"""

import os, re, sys, urllib, time
import paho.mqtt.client as mqtt
import sqlite3

## mqtt config
mqtt_broker = "192.168.1.111"
mqtt_channel = "node/6/mcp3208"

## sqlite config
db_name = "nodemcu_mqtt.sqlite"
table_name = "mcp3208"


def on_connect(mqttc, userdata, flags, result_code):
    #print("connected to broker with result code = " + str(result_code))
    mqttc.subscribe(topic=mqtt_channel, qos=0)

def on_disconnect(mqttc, userdata, result_code):
    #print("disconnected from broker with result_code = " + str(result_code))
    return()

def on_message(mqttc, userdata, msg):
    #print('message received...')
    #print('topic: ' + msg.topic + ': ' + str(msg.payload))
    mcp_value = msg.payload.decode('UTF-8')
    #print(mcp_value)
    ad = mcp_value.split(",")
    unixtime = int(time.time())

    # write to sqlite db
    sql_queue = """INSERT INTO %s VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?)""" % table_name
    sql_data = (unixtime, ad[0], ad[1], ad[2], ad[3], ad[4], ad[5], ad[6], ad[7])
    print(sql_data)
    lines = sql_run(db_name, sql_queue, sql_data)
    for line in lines:
        print(line)


def on_subscribe(mqttc, userdata, mid, granted_qos):
    #print('subscribed to channel (qos=' + str(granted_qos) + ')')
    return()

def on_unsubscribe(mqttc, userdata, mid, granted_qos):
    #print('unsubscribed from channel (qos=' + str(granted_qos) + ')')
    return()

## sqlite
def create_table(db_name, table_name):
    try:
        db = sqlite3.connect(db_name)
    except Exception as e:
        print("Couldn't use database %s!" % db_name)
        print(e)
        sys.exit(1)
    cur = db.cursor()
    try:
        cur.execute("CREATE TABLE IF NOT EXISTS " + table_name + " ( \
            unixtime INT NOT NULL, \
            A0 INT, \
            A1 INT, \
            A2 INT, \
            A3 INT, \
            A4 INT, \
            A5 INT, \
            A6 INT, \
            A7 INT) \
        ")
        print("Created table %s in database %s." % (table_name, db_name))
    except Exception as e:
        print("Couldn't create table %s!" % table_name)
        print(e)
        sys.exit(1)
    db.commit()
    db.close()


def delete_table(db_name, table_name):
    try:
        db = sqlite3.connect(db_name)
    except Exception as e:
        print("Couldn't use database %s!" % db_name)
        print(e)
        sys.exit(1)
    cur = db.cursor()
    try:
        cur.execute("DROP TABLE IF EXISTS " + table_name)
    except Exception as e:
        print("Couldn't delete table %s!" % (table_name))
        print(e)
        sys.exit(1)
    db.commit()
    db.close()


def sql_run(db_name, sql_queue, sql_data=""):
    try:
        db = sqlite3.connect(db_name)
    except Exception as e:
        print("Couldn't use database %s!" % db_name)
        print(e)
        sys.exit(1)
    cur = db.cursor()
    try:
        ##sql_queue = """INSERT INTO test VALUES ( ?, ?)"""
        ##sql_data = (unixtime, str(foo))
        cur.execute(sql_queue, sql_data)
        lines = cur.fetchall()
    except Exception as e:
        print("Couldn't run sql queue! (%s, %s)" % (sql_queue, sql_data))
        print(e)
        if db:
            db.rollback()
        sys.exit(1)
    db.commit()
    db.close()

    return lines


if __name__ == '__main__':
    #delete_table(db_name, table_name)
    create_table(db_name, table_name)
    sql_queue = """SELECT COUNT() FROM %s""" % table_name
    sql_return = sql_run(db_name, sql_queue)
    print("There are already %s rows in %s@%s." % \
            (str(sql_return[0][0]), table_name, db_name))
  
    mqttc = mqtt.Client()
    mqttc.on_connect = on_connect
    mqttc.on_disconnect = on_disconnect
    mqttc.on_message = on_message
    mqttc.on_subscribe = on_subscribe
    mqttc.on_unsubscribe = on_unsubscribe
    # connect to mqtt broker
    try:
        mqttc.connect(host=mqtt_broker, port=1883)
    except ConnectionRefusedError:
        sys.exit(1)

    # start mqtt listening process
    mqttc.loop_forever()
