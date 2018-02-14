#!/usr/bin/python3
import sqlite3
import sys
import time
import pygal

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

def draw_svg(x, y0, y1, y2):
    chart = pygal.Line(width=1024,x_label_rotation=90, \
        stroke=False, \
        legend_at_bottom=True, legend_at_bottom_columns=3)
    chart.title = "Node5 Graphs"
    chart.x_labels = x
    chart.add('Node5 Light1', y0 )
    chart.add('Node5 Temp1', y1 )
    chart.add('Node5 Light2', y2 )
    chart.render_to_file('node5.svg')


def main():
    db_name = "node5.sqlite"
    table_name = "mcp3208"

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

    csv_file = open("eddy2power.csv", 'w')
    for line in sql_return:
        for entry in line:
            csv_file.write("%s " % str(entry))
        csv_file.write("\n")

        #print(line)
    csv_file.close()

    x = []
    y0 = []
    y1 = []
    y2 = []
    for i in range(len(sql_return)):
    #for i in range(30):
            x.append(sql_return[i][0])
            y0.append(sql_return[i][1])
            y1.append(sql_return[i][2])
            y2.append(sql_return[i][3])

    draw_svg(x, y0, y1, y2)

if __name__ == "__main__":
    main()


