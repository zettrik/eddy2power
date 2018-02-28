# gnuplot
* write data to sqlite database
* let gnuplot print some diagrams

# munin
* node sends csv data via mqtt
* muninscript for collection the last mqtt message

# grafana
## dataflow: mqtt -> collector_script -> influx -> grafana

* mqtt node setup

* data collection
  * write a simple collector script
    * mqtt2influx: https://gist.github.com/jasonmhite/c2d9766dc27facf642b2
    * https://influxdb-python.readthedocs.io/en/latest/examples.html
    * https://github.com/influxdata/influxdb-python
  * most people will be happy with telegraf as collector
    * data via mqtt should follow the telegraf input data formats (e.g. json)
    * https://github.com/influxdata/telegraf/tree/master/plugins/inputs/mqtt_consumer
    * https://www.andymelichar.com/2017/02/26/iot-data-management-with-the-tig-stack/

* influxdb
  * installation
  ```
  curl -sL https://repos.influxdata.com/influxdb.key | sudo apt-key add -
  apt update
  apt install influxdb
  ```
  * setting up a database
  ```
  CREATE DATABASE node_data
  SHOW USERS
  CREATE USER "user1" WITH PASSWORD "pw1"
  ```
* grafana 
  * installation:
    * http://docs.grafana.org/installation/debian/
    * http://www.andremiller.net/content/grafana-and-influxdb-quickstart-on-ubuntu
    * http://docs.grafana.org/features/datasources/influxdb/
  * configure influxdb as data source (http settings: proxy)
  * create new dashboard
  * create panel, click on title and add dataset to panel (metrics)
  * hint: "toggle edit mode" to do some math with fields
   * e.g. calculate windspeed from interrupt/looptime: SELECT mean("i1") / mean("t2") FROM "mqtt_consumer" ...

* TODO: add config files
