#!/usr/bin/python3
"""
convert sqlite db in csv file 
"""

import sqlite3
import sys
import time


def sql_run(db_name, table_name, sql_queue, sql_data=""):
    try:
        db = sqlite3.connect(db_name)
    except Exception as e:
        print("Couldn't use database! %s" % e)
        sys.exit(1)
    cur = db.cursor()
    try:
        ##sql_queue = """INSERT INTO testtable VALUES ( ?, ?)"""
        ##sql_data = (unixtime, str(foo))
        cur.execute(sql_queue, sql_data)
        lines = cur.fetchall()
    except Exception as e:
        print("Couldn't run sql queue! %s" % e)
        if db:
            db.rollback()
        sys.exit(1)
    db.commit()
    db.close()
    return lines


def main():
    db_name = "node6.sqlite"
    table_name = "mcp3208"
    csv_file = "eddy2power.csv"

    #sql_queue = """DELETE FROM %s WHERE A0 > 1023 OR A1 > 1023 OR A2 > 1023 OR A3 > 1023 OR A4 > 1023 OR A5 > 1023""" % table_name
    #sql_return = sql_run(db_name, table_name, sql_queue)

    #seconds_backwards = 3600  ## one hour
    seconds_backwards = 86400  ## one day
    #seconds_backwards = 604800  ## one week
    start_date = time.time() - (2 * seconds_backwards)
    sql_queue = """SELECT * FROM %s \
                    WHERE unixtime > %i \
                    AND A0 < 4096 \
                    AND A1 < 4096 \
                    AND A2 < 4096 \
                    AND A3 < 4096 \
                    AND A4 < 4096 \
                    AND A5 < 4096 \
                """ % (table_name, start_date)
    sql_return = sql_run(db_name, table_name, sql_queue)
    print("Selected %i rows from table: %s in db: %s." % \
            (len(sql_return), table_name, db_name))

    csv_file = open(csv_file, 'w')
    for line in sql_return:
        for entry in line:
            csv_file.write("%s " % str(entry))
        csv_file.write("\n")

        #print(line)
    csv_file.close()

if __name__ == "__main__":
    main()


