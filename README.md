# Sunrise Alarm Clock

A simple sunrise alarm clock designed for use with LIFX bulbs and Raspberry Pi Zero W

## System Setup

Flash SD card with [raspian](https://www.raspberrypi.org/downloads/raspbian/) using [etcher](https://www.balena.io/etcher/)

In `/boot/` create file called `ssh`

In `/boot/` create file called `wpa_supplicant.conf`
Populate `wpa_supplicant.conf` with:
```
country=US
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1

network={
ssid="WIFI_SSID"
scan_ssid=1
psk="WIFI_PASSWORD"
key_mgmt=WPA-PSK
}

```
Access with:
`ssh pi@raspberrypi.local`

Update:
```sh
sudo apt-get update && sudo apt-get upgrade -y
```

Install system dependencies:
```sh
sudo apt-get install python-pip3
sudo apt-get install git
```

Install python dependencies:
```sh
sudo pip3 install lifxlan
sudo pip3 install bottle
```

Clone repo:
```sh
git clone https://github.com/TeaUponTweed/sunrise_alarm_clock.git
```

## Run Project

```sh
# in another tab
python3 test_server.py
# in another tab
python3 monitor_alarms.py
```
## WebApp Developement

*Requires Node, ReasonML and ReasonReact*

With node installed, run:
```sh
npm install -g bs-platform
```

```sh
npm install
npm start
# in another tab
npm run webpack
```

### Build for Production

```sh
npm run clean
npm run build
npm run webpack:production
```

## Daemonize
To use `supervisor` to launch scripts on startup and monitor them
```sh
sudo apt-get install supervisor
sudo service supervisor start
```

Create config files:
```sh
sudo echo "\
[program:monitor_alrams]
command=python3 /home/pi/sunrise_alarm_clock/monitor_alarms.py
" >  /etc/supervisor/conf.d/alarm_monitor.conf

sudo echo "\
[program:sunrise_alram_server]
command=python3 /home/pi/sunrise_alarm_clock/test_server.py
" >  /etc/supervisor/conf.d/sunrise_alarm.conf
```

Re-read and launch:
```sh
supervisorctl reread
supervisorctl update
```
