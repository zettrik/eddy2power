#TODO
## dataflow: mqtt -> influx -> grafana
* influxdb installation
```
curl -sL https://repos.influxdata.com/influxdb.key | sudo apt-key add -
apt update
apt install influxdb
```
* mqtt2influx: https://gist.github.com/jasonmhite/c2d9766dc27facf642b2
  * https://influxdb-python.readthedocs.io/en/latest/examples.html
  * https://github.com/influxdata/influxdb-python
* grafana install:
  * http://www.andremiller.net/content/grafana-and-influxdb-quickstart-on-ubuntu
