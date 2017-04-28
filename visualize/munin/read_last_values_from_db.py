#!/usr/bin/python3
import sqlite3
import sys

def sql_run(db_name, table_name, sql_queue, sql_data=""):
    try:
        db = sqlite3.connect(db_name)
    except Exception as e:
        print("Konnte Datenbank nicht verwenden! %s" % e)
        sys.exit(1)
    cur = db.cursor()
    try:
        ##sql_queue = """INSERT INTO test VALUES ( ?, ?)"""
        ##sql_data = (unixtime, str(foo))
        cur.execute(sql_queue, sql_data)
        lines = cur.fetchall()
    except Exception as e:
        print("Konnte Queue nicht ausführen! %s" % e)
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
    #sql_queue = """SELECT * FROM %s;""" % table_name
    #sql_queue = """SELECT MAX(unixtime), A0, A1, A2, A3, A4, A5 FROM %s;""" % table_name
    sql_queue = ("""SELECT avg(A0), avg(A1), avg(A2), avg(A3), avg(A4), avg(A5) \
                    FROM (
                        SELECT * \
                        FROM %s \
                        ORDER BY unixtime DESC LIMIT 10
                    );""" \
                    % table_name)
    sql_return = sql_run(db_name, table_name, sql_queue)
    print(sql_return)
    tmp_file = open("/tmp/eddy_last_values", 'w')
    tmp_file.write(str(sql_return))
    tmp_file.close()

if __name__ == "__main__":
    main()


