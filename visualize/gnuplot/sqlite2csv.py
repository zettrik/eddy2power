#!/usr/bin/python3
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
        ##sql_queue = """INSERT INTO test VALUES ( ?, ?)"""
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
    db_name = "eddy2power.sqlite"
    table_name = "test"

    #sql_queue = """DELETE FROM %s WHERE A0 > 1023 OR A1 > 1023 OR A2 > 1023 OR A3 > 1023 OR A4 > 1023 OR A5 > 1023""" % table_name
    #sql_return = sql_run(db_name, table_name, sql_queue)

    #seconds_backwards = 3600  ## one hour
    #seconds_backwards = 86400  ## one day
    seconds_backwards = 604800  ## one week
    start_date = time.time() - (2 * seconds_backwards)
    sql_queue = """SELECT * FROM %s
                    WHERE unixtime > %i \
                    AND A0 < 1024 \
                    AND A1 < 1024 \
                    AND A2 < 1024 \
                    AND A3 < 1024 \
                    AND A4 < 1024 \
                    AND A5 < 1024 \
                """ % (table_name, start_date)
    sql_return = sql_run(db_name, table_name, sql_queue)
    print("There were %i rows selected in table." % len(sql_return))

    csv_file = open("eddy2power.csv", 'w')
    for line in sql_return:
        for entry in line:
            csv_file.write("%s " % str(entry))
        csv_file.write("\n")

        #print(line)
    csv_file.close()

if __name__ == "__main__":
    main()


