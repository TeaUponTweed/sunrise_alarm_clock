#!/usr/bin/env python

import sys
import time

import fire

import lifxlan
from lifxlan import LifxLAN


def do_sunrise_step(light, step, num_steps, start_color, final_color):
    dx = step/num_steps
    hue1,saturation1,brightness1,temp1 = start_color
    hue2,saturation2,brightness2,temp2 = final_color
    hue = hue1+dx*(hue2-hue1)
    saturation = saturation1+dx*(saturation2-saturation1)
    temp = temp1+dx*(temp2-temp1)
    brightness = brightness1+dx*(brightness2-brightness1)
    try:
        light.set_color((hue, saturation, brightness, temp))
    except lifxlan.errors.WorkflowException:
        print("*** workflow exception ***")
        return 1
    else:
        return 0


colors = [ # hue staturation brighness kelvin
    (768, 65535, 0, 2500),
    (4500, 65535, 100, 2500),
    (7680, 65535, 300, 2500),
    (7680, 0, 2000, 2500),
    (7680, 0, 20000, 2500),
    (65024, 0, 40000, 2500),
    (65024, 0, 60000, 3000),
]

def connect(expected_num_lights, ntries=5):
    print("Discovering lights...")
    devices = []
    for i in range(ntries):
        lifx = lifxlan.LifxLAN(expected_num_lights)
        devices = lifx.get_lights()
        if expected_num_lights is None:
            if len(devices) > 0:
                break
            else:
                continue
        elif not len(devices) >= expected_num_lights:
            print(f'Discoverd {len(devices)} lights')
            if i < expected_num_lights:
                print('Waiting and trying again no lights')
                time.sleep(.5)
        else:
            print(f"Discoverd {len(devices)} lights")
            break

    return devices

def main(test=False, num_lights=None):
    # get devices
    devices = connect(num_lights)
    if len(devices) == 0:
        print("Failed to discover any lights")
        return

    # bulb = devices[0]
    if test:
        update_interval = .5
        total_time = 60
    else:
        update_interval = 2
        total_time = 60*30

    total_steps = total_time/update_interval
    num_steps = round(total_steps/(len(colors) -1))
    for bulb in devices:
        # turn on and set brightness to zero
        bulb.set_color([768, 65535, 0, 3200])
        bulb.set_power(65535)
        time.sleep(.1)

    # step through all colors
    err_count = 0
    for j in range(len(colors)-1):
        color, next_color = colors[j], colors[j+1]
        for i in range(num_steps):
            print(i, j, len(devices))
            if len(devices) == 0:
                devices = connect(num_lights)

            start_time = time.time()
            for bulb in devices:
                err = do_sunrise_step(bulb, i, num_steps, color, next_color)
                if err:
                    print('error with one bulb')

                err_count += err
                time.sleep(.1)

            if err_count == 2:
                print("too many errors. attempting to reconnect")
                devices = connect(num_lights)
                err_count = 0

            end_time = time.time()
            deltat = end_time - start_time
            if deltat < update_interval:
                time.sleep(update_interval-deltat)
            else:
                print('cant keep up! what?')

    if not test:
        time.sleep(60*60)

    for bulb in devices:
        bulb.set_power("off")
        time.sleep(.1)


if __name__=="__main__":
    fire.Fire(main)
