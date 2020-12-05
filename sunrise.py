#!/usr/bin/env python

import sys
import time

import fire

import lifxlan
from lifxlan import LifxLAN


class wrap_lifx_retry_ret_err(object):
    def __init__(self, tries, wait_retry=None):
        self.tries = tries
        self.wait_retry = wait_retry
    def __call__(self, f):
        def g(*args, **kwargs):
            for ntry in range(self.tries):
                try:
                    ret = f(*args, **kwargs)
                except (lifxlan.errors.WorkflowException, OSError) as e:
                    if ntry == self.tries - 1:
                        return True
                    else:
                        if self.wait_retry is not None:
                            time.sleep(self.wait_retry)
                else:
                    assert ret is False
                    return ret
            return True        

        return g


@wrap_lifx_retry_ret_err(3, 0.01)
def do_sunrise_step(light, step, num_steps, start_color, final_color):
    dx = step/num_steps
    hue1,saturation1,brightness1,temp1 = start_color
    hue2,saturation2,brightness2,temp2 = final_color
    hue = hue1+dx*(hue2-hue1)
    saturation = saturation1+dx*(saturation2-saturation1)
    temp = temp1+dx*(temp2-temp1)
    brightness = brightness1+dx*(brightness2-brightness1)
    light.set_color((hue, saturation, brightness, temp))
    return False

@wrap_lifx_retry_ret_err(3, 0.01)
def turn_light_on(bulb):
    bulb.set_color([768, 65535, 0, 3200])
    bulb.set_power(65535)
    return False

@wrap_lifx_retry_ret_err(3, 0.01)
def turn_light_on_dim(bulb):
    bulb.set_color((7680, 0, 32500, 2500))
    bulb.set_power("on")
    return False
@wrap_lifx_retry_ret_err(3, 0.01)
def turn_light_off(bulb):
    bulb.set_power("off")
    return False

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
        try:
            lifx = lifxlan.LifxLAN(expected_num_lights)
            devices = lifx.get_lights()
        except (OSError, lifxlan.errors.WorkflowException):
            time.sleep(0.1)
            continue
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
        turn_light_on(bulb)
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
                    err_count += 1

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
        err = turn_light_off(bulb)
        if err:
            print("Failed to power off bulb")


if __name__=="__main__":
    devices = connect(2)
    for bulb in devices:
        # turn on and set brightness to zero
        turn_light_on_dim(bulb)
        time.sleep(.1)
    # turn_light_on(bulb):
    # bulb.set_color([768, 65535, 0, 3200])
    # bulb.set_power(65535)
    # return False
    # fire.Fire(main)
