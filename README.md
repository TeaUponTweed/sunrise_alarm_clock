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
sudo pip3 install fire
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

Create service files:
```sh
sudo bash -c 'echo "\
[Unit]
Description=Monitor Alarms
After=network.target

[Service]
ExecStart=/usr/bin/python3 monitor_alarms.py
WorkingDirectory=/home/pi/sunrise_alarm_clock
StandardOutput=syslog
StandardError=syslog
Restart=always
User=pi

[Install]
WantedBy=multi-user.target
" >  /etc/systemd/system/monitor_alarms.service'



sudo bash -c 'echo "\
[Unit]
Description=Monitor Alarms
After=network.target

[Service]
ExecStart=/usr/bin/python3 test_server.py
WorkingDirectory=/home/pi/sunrise_alarm_clock
StandardOutput=syslog
StandardError=syslog
Restart=always
User=pi

[Install]
WantedBy=multi-user.target
" >  /etc/systemd/system/sunrise_alarm.service'
```

Start and enable new services:
```sh
systemctl start monitor_alarms
systemctl enable monitor_alarms
systemctl start sunrise_alarm
systemctl enable sunrise_alarm
```


Add health check for server every five minutes
```
$ sudo crontab -e
# Add:
*/5 * * * * /home/pi/sunrise_alarm_clock/health.sh
```


To sync local files:
```
rsync -arzv --exclude .git --exclude node_modules ../sunrise_alarm_clock pi@XXX.XXX.X.XX:
```
