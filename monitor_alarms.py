from datetime import datetime
import json
import sys
import os
import math
import time

# import logging

# LOG=logging.getLogger()

# logging.basicConfig(level=logging.INFO)


from sunrise import main as sunrise

DIRNAME = os.path.dirname(os.path.realpath(__file__))

def load_alarms():
	apath = os.path.join(DIRNAME, 'alarms.json')
	if os.path.exists(apath):
		with open(apath) as f:
			alarms = json.load(f)
	else:
		alarms = {}

	return alarms

# def convert_alarm_to_dt(alarm):
def map_repeat_key_to_int(repeat):
	if repeat == 'monday':
		return 0
	elif repeat == 'tuesday':
		return 1
	elif repeat == 'wednesday':
		return 2
	elif repeat == 'thursday':
		return 3
	elif repeat == 'friday':
		return 4
	elif repeat == 'saturday':
		return 5
	elif repeat == 'sunday':
		return 6
	else:
		raise ValueError(f'Invalid DOW {repeat}')

def get_alarm_hour_min(alarm):
	hour = alarm['hour']
	minute = alarm['minute']
	if hour == 12:
		if alarm['isAM']:
			hour = 0
		else:
			hour = 12
	else:
		if not alarm['isAM']:
			hour += 12

	return hour, minute

def main():
	while True:
		# determine check times
		now_utc_s = time.time()
		next_check_time_utc_s = now_utc_s//60*60+60
		# compare datetimes to now
		now_dt = datetime.now()
		for alarm in load_alarms().values():
			if not alarm['enabled']:
				continue
			# if not all(alarm['repeat'].values()):
			if not any((map_repeat_key_to_int(dow) == now_dt.weekday()) and repeat for dow, repeat in alarm['repeat'].items()):
				continue
			# LOG.info('ere')
			alarm_hour, alarm_min = get_alarm_hour_min(alarm)
			alarm_time = alarm_min+alarm_hour*60
			now_min = now_dt.minute+now_dt.hour*60

			# LOG.info(now_min)
			# LOG.info(alarm_time)
			# LOG.info(now_dt.hour)
			# LOG.info(alarm_hour)
			# LOG.info(abs(now_dt.hour - alarm_hour+24))
			# delta_hour = min(abs(now_dt.hour - alarm_hour), abs(now_dt.hour - alarm_hour+24))
			# delta_min = alarm_time - now_min
			delta_min = 720 - abs(abs(alarm_time - now_min) - 720); 
			# LOG.info(delta_min)
			# delta_min = now_dt.minute - alarm_min
			# LOG.info(delta_hour, delta_min)
			if abs(delta_min) < 2:
				sunrise(num_lights=2)
		try:
			time.sleep(next_check_time_utc_s - time.time())
		except ValueError:
			pass


if __name__ == '__main__':
	main()
