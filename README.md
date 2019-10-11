# Sunrise Alarm Clock
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

Install pip:
```sh
sudo apt-get install python-pip
```

Install python dependencies:
```sh
sudo pip install lifxlan
sudo pip install bottle
```

Clone repo:
```sh
git clone https://github.com/TeaUponTweed/sunrise_alarm_clock.git
```

## Run Project

```sh
# in another tab
python test_server.py
# in another tab
python monitor_alarms.py
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
