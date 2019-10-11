# import bottle
import json
import os

from bottle import static_file, run, route, post, request, get

from lifxlan import LifxLAN

DIRNAME = os.path.dirname(os.path.realpath(__file__))

@route('/<filepath:path>')
def server_static(filepath):
    return static_file(filepath, root=os.path.join(DIRNAME, 'build'))

@route('/')
def server_static(filepath="index.html"):
    return static_file(filepath, root=os.path.join(DIRNAME, 'build'))

def _transform_alarms(alarms):
    return {
        'alarms': list(alarms.values())
    }

ALARM_FILE = 'alarms.json'
def load_alarms(f):
    if os.path.exists('alarms.json'):
        with open('alarms.json') as f:
            alarms = json.load(f)
    else:
        alarms = {}
    return alarms

@post('/api/set_alarm')
def set_alarm():
    body = request.body
    data = json.loads(body.getvalue().decode('ascii'))
    try:
        alarm_id = int(data['alarmID'])
        data['hour'] = int(data['hour'])
        data['minute'] = int(data['minute'])
    except KeyError:
        print('Invalid request')
        print(data)
    else:

        alarms = load_alarms(ALARM_FILE)
        if alarm_id == -1: # allocate new alarm id
            if len(alarms) == 0:
                alarm_id = 1
            else:
                alarm_id = max(map(int, alarms.keys())) + 1

            assert str(alarm_id) not in alarms
        else:
            assert str(alarm_id) in alarms

        data['alarmID'] = int(alarm_id)
        alarms[str(alarm_id)] = data
        print('saving...')
        print(alarms)
        with open(ALARM_FILE, 'w') as out:
            json.dump(alarms, out)

        return json.dumps(_transform_alarms(alarms))


@post('/api/cancel_alarm')
def cancel_alarm():
    body = request.body
    data = json.loads(body.getvalue().decode('ascii'))
    try:
        alarm_id = data['alarmID']
    except KeyError:
        print('Invalid request')
        print(data)
    else:
        alarms = load_alarms(ALARM_FILE)
        alarms.pop(str(alarm_id), None)
        with open('alarms.json', 'w') as out:
            json.dump(alarms, out)

        return json.dumps(_transform_alarms(alarms))


@get('/api/alarms')
def alarms():
    alarms = load_alarms(ALARM_FILE)
    return json.dumps(_transform_alarms(alarms))

@get('/api/turn_off_light')
def turn_off_light():
    num_lights = 1

    # instantiate LifxLAN client, num_lights may be None (unknown).
    # In fact, you don't need to provide LifxLAN with the number of bulbs at all.
    # lifx = LifxLAN() works just as well. Knowing the number of bulbs in advance 
    # simply makes initial bulb discovery faster.
    print("Discovering lights...")
    lifx = LifxLAN(num_lights)

    # get devices
    devices = lifx.get_lights()
    if not len(devices):
        print('Discoverd no lights')
        return json.dumps({'light_connected': False})

    bulb = devices[0]
    bulb.set_power("off")
    return json.dumps({'light_connected': True})


run(host='localhost', port=8080, debug=True)
