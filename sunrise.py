#!/usr/bin/env python

import sys
import time


import lifxlan


def do_sunrise_step(light, step, num_steps, start_color, final_color, count=1):
    dx = step/num_steps
    hue1,saturation1,brightness1,temp1 = start_color
    hue2,saturation2,brightness2,temp2 = final_color
    hue = hue1+dx*(hue2-hue1)
    saturation = saturation1+dx*(saturation2-saturation1)
    temp = temp1+dx*(temp2-temp1)
    brightness = brightness1+dx*(brightness2-brightness1)
    try:
        light.set_color((hue, saturation, brightness, temp))
    except: #lifxlan.errors.WorkflowException:
        # TODO do I need to rediscover?
        print('Failed to set color. Waiting abit and trying again')
        time.sleep(1)
        if count <= 5:
            do_sunrise_step(light, step, num_steps, start_color, final_color, count=count+1)
        else:
            raise

colors = [ # hue staturation brighness kelvin
    (768, 65535, 0, 2500),
    (4500, 65535, 100, 2500),
    (7680, 65535, 300, 2500),
    (7680, 0, 2000, 2500),
    (7680, 0, 20000, 2500),
    (65024, 0, 40000, 2500),
    (65024, 0, 60000, 3000),
]


def main():
    num_lights = 1

    # instantiate LifxLAN client, num_lights may be None (unknown).
    # In fact, you don't need to provide LifxLAN with the number of bulbs at all.
    # lifx = LifxLAN() works just as well. Knowing the number of bulbs in advance 
    # simply makes initial bulb discovery faster.
    print("Discovering lights...")
    lifx = lifxlan.LifxLAN(num_lights)

    # get devices
    devices = lifx.get_lights()
    if not len(devices):
        print('Discoverd no lights')
        return

    bulb = devices[0]

    update_interval = 2
    total_time = 60*30
    total_steps = total_time/update_interval
    num_steps = round(total_steps/(len(colors) -1))

    # turn on and set brightness to zero
    bulb.set_color([768, 65535, 0, 3200])
    bulb.set_power(65535)
    # step through all colors
    for j in range(len(colors)-1):
        color, next_color = colors[j], colors[j+1]
        for i in range(num_steps):
            print(i, j)
            start_time = time.time()
            do_sunrise_step(bulb, i, num_steps, color, next_color)
            end_time = time.time()
            deltat = end_time - start_time
            if deltat < update_interval:
                time.sleep(update_interval-deltat)
            else:
                print('cant keep up! what?')
    time.sleep(20*60)
    for _ in range(5):
        try:
            bulb.set_power("off")
        except: # lifxlan.errors.WorkflowException:
            print('failed to power off bulb, trying again in a bit')
            time.sleep(5)
        else:
            break
    else:
        print('failed to power off bulb!')


if __name__=="__main__":
    main()
