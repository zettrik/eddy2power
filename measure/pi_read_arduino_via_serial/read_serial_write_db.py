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
            intVCC INT) \
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
    db_name = "eddy2power.sqlite"
    table_name = "test"
    field = "testfield"
    field_type = 'INTEGER'

    #delete_table(db_name, table_name)
    #create_table(db_name, table_name)
    sql_queue = """SELECT COUNT() FROM %s""" % table_name
    sql_return = sql_run(db_name, sql_queue)
    print("There are already %s rows in table." % sql_return[0])

    ser = serial.Serial('/dev/ttyACM0', 115200) 
    while True:
        #print("using serial: " + ser.portstr)
        #current_time = datetime.datetime.now()
        unixtime = int(time.time())
        ## read a line and convert it from b'xxx\r\r\n' to xxx
        arduino_output = str(ser.readline().decode('utf-8')[:-2])
        print(unixtime, arduino_output)
        data = arduino_output.split(", ")
        sql_queue = """INSERT INTO test VALUES ( ?, ?, ?, ?, ?, ?, ?, ?)"""
        # skip on first run because of incomplete dataset
        if len(data) == 9:
            sql_data = (unixtime, data[1], data[2], data[3], data[4], data[5], data[6], data[7]) 
            lines = sql_run(db_name, sql_queue, sql_data)
            for line in lines:
                print(line)

            # write laste values in tmpfile for munin
            tmp_file = open("/tmp/eddy_last_values", 'w')
            tmp_file.write(str(unixtime))
            for i in range(1,7):
                tmp_file.write(", " + str(int(data[i])))
            tmp_file.close()
        else:
            print("incomplete row: %s" % data)
    ser.close()


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
