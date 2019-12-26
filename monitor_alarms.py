from datetime import datetime
import json
import sys
import os
import math
import time

from sunrise import main as sunrise

def load_alarms():
	with open('alarms.json') as f:
		alarms = json.load(f)
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
			# print('ere')
			alarm_hour, alarm_min = get_alarm_hour_min(alarm)
			alarm_time = alarm_min+alarm_hour*60
			now_min = now_dt.minute+now_dt.hour*60

			# print(now_min)
			# print(alarm_time)
			# print(now_dt.hour)
			# print(alarm_hour)
			# print(abs(now_dt.hour - alarm_hour+24))
			# delta_hour = min(abs(now_dt.hour - alarm_hour), abs(now_dt.hour - alarm_hour+24))
			# delta_min = alarm_time - now_min
			delta_min = 720 - abs(abs(alarm_time - now_min) - 720); 
			print(delta_min)
			# delta_min = now_dt.minute - alarm_min
			# print(delta_hour, delta_min)
			if abs(delta_min) < 2:
				sunrise()
		try:
			time.sleep(next_check_time_utc_s - time.time())
		except ValueError:
			pass


if __name__ == '__main__':
	main()