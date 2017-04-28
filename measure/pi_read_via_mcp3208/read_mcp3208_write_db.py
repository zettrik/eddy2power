#!/usr/bin/python3
"""
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

import datetime
import serial
import sqlite3
import sys
import time
from gpiozero import MCP3208


# Read data from the ADC 
def getData(sensor_nr):
    raw_value = 0 
    read_repeat = 20
    # mittelwert
    for i in range(read_repeat):
        sensor = MCP3208(sensor_nr)
        raw_value += sensor.value
        #print(sensor.value)
        sensor.close()
        mcp = (raw_value * 4096 / read_repeat) 
    return('%0.f' % mcp)


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


def main():
    db_name = "eddy2power_mcp3208.sqlite"
    table_name = "test"
    field = "testfield"
    field_type = 'INTEGER'

    #delete_table(db_name, table_name)
    create_table(db_name, table_name)
    sql_queue = """SELECT COUNT() FROM %s""" % table_name
    sql_return = sql_run(db_name, sql_queue)
    print("There are already %s rows in table." % sql_return[0])
    while True:
        #print("using serial: " + ser.portstr)
        #current_time = datetime.datetime.now()
        unixtime = int(time.time())
        #print('%.0f | %.0f | %0.f | %0.f | %0.f | %0.f | %0.f | %0.f ' \
            #% (getData(0), getData(1), getData(2), getData(3), \
                #getData(4), getData(5), getData(6), getData(7)))
        sql_queue = """INSERT INTO test VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?)"""
        sql_data = (unixtime, getData(0), getData(1), getData(2), \
            getData(3), getData(4), getData(5), getData(6), getData(7))
        print(sql_data)
        lines = sql_run(db_name, sql_queue, sql_data)
        for line in lines:
            print(line)

        """
        # write laste values in tmpfile for munin
        tmp_file = open("/tmp/eddy_last_values", 'w')
        #tmp_file.write(str(unixtime))
        for i in range(0,7):
            tmp_file.write(", " + str(int(sql_data[i])))
        tmp_file.close()
        """
        time.sleep(4)


if __name__ == "__main__":
    main()


"""
# Verwendung in einem Modul
import logging
logger = logging.getLogger(__name__)
del(logging)

def foo():
    ...
    logger.info('Sun is shining')


# Beispiel für die Loggingkonfiguration
# Diese Zeilen stehen in der Datei, in der die main() Funktion aufgerufen wird.

import sys
import logging
root_logger=logging.getLogger()
root_logger.setLevel(logging.INFO)
handler=logging.StreamHandler(sys.stdout)
handler.setFormatter(logging.Formatter('%(asctime)s %(name)s:
%(levelname)-8s [%(process)d] %(message)s', '%Y-%m-%d %H:%M:%S'))
root_logger.addHandler(handler)
logger=logging.getLogger(os.path.basename(sys.argv[0]))
del(logging)
"""
