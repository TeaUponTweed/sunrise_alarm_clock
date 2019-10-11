open Types
open Model
open Utils
open Server
open Update

let viewNumbers = (model) => {
    let viewNumbersImpl = (i, number, r, len_number, clockX, clockY) => {
        let rad = (foi(i)/.foi(len_number)*.3.14159*.2.)-.3.1415926/.2.;
        let x = iof(r*.cos(rad)) + clockX-18;
        let y = iof(r*.sin(rad)) + clockY+10;
        let myStyle = ReactDOMRe.Style.make(~color="#444444", ~font="bold 35px sans-serif", ());

        <text x={soi(x)} y={soi(y)} fill="white" style=myStyle key={soi(i)++soi(number)}>{React.string(pads(soi(number), 2, '0'))}</text>
    };
    let doViewNumbers = (arr) => {
        Array.mapi((i, n) => {
            viewNumbersImpl(i+1, n, foi(model.clockSize)*.0.8, Array.length(arr), model.clockLoc.x, model.clockLoc.y)
        }, arr)
    };
    switch (model.alarmSetState) {
        | NoTimeSelected | HourSelected(_) => doViewNumbers([|1,2,3,4,5,6,7,8,9,10,11,12|])
        | HourAndMinuteSelected(_,_) => doViewNumbers([|5,10,15,20,25,30,35,40,45,50,55,0|])
    };
};

let viewSelectingCircle = (model, rad) => {
    let dx = iof(0.8*.foi(model.clockSize)*.cos(rad));
    let dy = iof(0.8*.foi(model.clockSize)*.sin(rad));
    [|
        <circle cx={soi(model.clockLoc.x+dx)} cy={soi(model.clockLoc.y+dy)} r={soi(model.clockSize/6)} fill="blue" key="outer_circle"/>,
        <circle cx={soi(model.clockLoc.x)} cy={soi(model.clockLoc.y)} r={soi(model.clockSize/10)} fill="blue" key="inner_circle"/>
    |]
}

let viewCircleAndLines = (model, hourCircRatio, minCircRatio, selectingHour) => {
    let minRad = (minCircRatio-.0.25)*.2.*.3.1415926;
    let minDx = iof(0.8*.foi(model.clockSize)*.cos(minRad));
    let minDy = iof(0.8*.foi(model.clockSize)*.sin(minRad));


    let hourRad = (hourCircRatio-.0.25)*.2.*.3.1415926;
    let hourDx = iof(0.6*.foi(model.clockSize)*.cos(hourRad));
    let hourDy = iof(0.6*.foi(model.clockSize)*.sin(hourRad));

    let lineStyle = ReactDOMRe.Style.make(~stroke="blue", ~strokeWidth="10px", ());
    let lines = [|
        <line x1={soi(model.clockLoc.x)} y1={soi(model.clockLoc.y)} x2={soi(model.clockLoc.x+hourDx)} y2={soi(model.clockLoc.y+hourDy)} key="hourClockLine" style=lineStyle/>,
        <line x1={soi(model.clockLoc.x)} y1={soi(model.clockLoc.y)} x2={soi(model.clockLoc.x+minDx)} y2={soi(model.clockLoc.y+minDy)} key="minClockLine" style=lineStyle/>,
    |]

    let circles = if (selectingHour) {
        viewSelectingCircle(model, hourRad)
    } else {
        viewSelectingCircle(model, minRad)
    };

    Array.append(lines, circles)
};


let viewCurrentSelectionCircle = (model) => {
    let (hour, min) = getDisplayTime(model.alarmSetState);
    switch (model.alarmSetState) {
        | NoTimeSelected | HourSelected(_) => viewCircleAndLines(model, foi(hour)/.12., 0., true)
        | HourAndMinuteSelected(_,_) => viewCircleAndLines(model, foi(hour)/.12., foi(min)/.60., false)
    };
};


let viewTimeText = (model) => {
    let (hour, min) = getDisplayTime(model.alarmSetState);
    let colonStyle = ReactDOMRe.Style.make(~color="blue", ~font="90px sans-serif", ());

    let (hourStyle, minStyle) = switch (model.alarmSetState) {
        | NoTimeSelected | HourSelected(_) => {
            let hourStyle = ReactDOMRe.Style.make(~color="blue", ~font="bold 90px sans-serif", ());
            let minStyle  = ReactDOMRe.Style.make(~color="blue", ~font="90px sans-serif", ());
            (hourStyle, minStyle)
        }
        | HourAndMinuteSelected(_, _) => {
            let hourStyle = ReactDOMRe.Style.make(~color="blue", ~font="90px sans-serif", ());
            let minStyle  = ReactDOMRe.Style.make(~color="blue", ~font="bold 90px sans-serif", ());
            (hourStyle, minStyle)
        }
    };
    let am_pm = if (model.alarmSetAM) {
        "AM"
    }
    else {
        "PM"
    };
    [|
        <text x="30" y="100" fill="blue" style=hourStyle key="hour">{React.string(pads(soi(hour), 2, '0'))}</text>,
        <text x="130" y="100" fill="blue" style=colonStyle key="colon">{React.string(":")}</text>,
        <text x="150" y="100" fill="blue" style=minStyle key="min">{React.string(pads(soi(min), 2, '0'))}</text>,
        <text x="250" y="100" fill="blue" style=colonStyle key="ampm">{React.string(am_pm)}</text>,
    |]
};

let confirmTextHelper = (model) => {
    switch (model.alarmSetState) {
        | NoTimeSelected | HourSelected(_ ) => "Next"
        | HourAndMinuteSelected(_,_) => "Set Alarm"
    }
};

// let viewRepeatImpl = (alarm, dispatcher) => {

// };

let viewRepeat = (alarm, dispatcher) => {
    // [|0,1,2,3,4,5,6,7|]
    let updateRepeat = (i) => {
        let repeat = switch (i) {
            | 0 => {...alarm.repeat, monday: !alarm.repeat.monday}
            | 1 => {...alarm.repeat, tuesday: !alarm.repeat.tuesday}
            | 2 => {...alarm.repeat, wednesday: !alarm.repeat.wednesday}
            | 3 => {...alarm.repeat, thursday: !alarm.repeat.thursday}
            | 4 => {...alarm.repeat, friday: !alarm.repeat.friday}
            | 5 => {...alarm.repeat, saturday: !alarm.repeat.saturday}
            | 6 => {...alarm.repeat, sunday: !alarm.repeat.sunday}
            | _ => {Js.log("Unhandled reapeat value"); alarm.repeat}
        };
        let newAlarm = {
            ...alarm,
            repeat: repeat
        };
        send_updated_alarm_state(newAlarm, dispatcher)
    };
    let getClass = (isReapeating) => {
        if (isReapeating && alarm.enabled) {
            "btn btn-primary"
        } else {
            "btn btn-secondary"
        }
    };

    <div className="btn-group btn-group-toggle">
        <button className={getClass(alarm.repeat.monday)} onClick={_=>updateRepeat(0)} >
            {React.string("M")}
        </button>
        <button className={getClass(alarm.repeat.tuesday)} onClick={_=>updateRepeat(1)} >
            {React.string("Tu")}
        </button>
        <button className={getClass(alarm.repeat.wednesday)} onClick={_=>updateRepeat(2)} >
            {React.string("W")}
        </button>
        <button className={getClass(alarm.repeat.thursday)} onClick={_=>updateRepeat(3)} >
            {React.string("Th")}
        </button>
        <button className={getClass(alarm.repeat.friday)} onClick={_=>updateRepeat(4)} >
            {React.string("F")}
        </button>
        <button className={getClass(alarm.repeat.saturday)} onClick={_=>updateRepeat(5)} >
            {React.string("Sa")}
        </button>
        <button className={getClass(alarm.repeat.sunday)} onClick={_=>updateRepeat(6)} >
            {React.string("Su")}
        </button>
    </div>
};

let viewEnabled = (alarm, dispatcher) => {
    let className = if (alarm.enabled) {
            "btn btn-primary"
        } else {
            "btn btn-secondary"
        };

    <button className={className} onClick={_ => {send_updated_alarm_state({...alarm, enabled: !alarm.enabled}, dispatcher)}}>
        {
            React.string(if (alarm.enabled) {
                "Enabled"
            } else {
                "Disabled"
            })
        }
    </button>
};

let viewSetAlarm = (i, alarm, dispatcher) => {

    let alarm_string = pads(soi(alarm.hour),2,'0') ++ ":" ++ pads(soi(alarm.minute),2,'0');
    let alarm_string = if (alarm.isAM) {
        alarm_string ++ " AM"
    } else {
        alarm_string ++ " PM"
    };

    <tr key={"table" ++ soi(i)}>
        // <th scope="row">{React.string(soi(i+1))}</th>
        <td>{React.string(alarm_string)}</td>
        <td>
            {viewEnabled(alarm, dispatcher)}
        </td>
        <td>
            {viewRepeat(alarm, dispatcher)}
        </td>

        <td>
            <button onClick={_ => send_cancel_alarm(alarm, dispatcher)} className="btn">
                {React.string("X")}
            </button>
        </td>
    </tr>
};

// TODO should make its own component?
let viewSetAlarms = (set_alarms, dispatcher) => {
    let alarm_list_viz = switch (set_alarms) {
        | Some(alarms) => List.mapi((i, alarm) => viewSetAlarm(i, alarm, dispatcher), alarms)
        | None => []
    };
    React.array(Array.of_list(alarm_list_viz))
};

let viewModel = (model, dispatcher) => {
    <div>

        <div>
        <center> // TODO centering screws up mouse math. Can proable assume that clock  cenbter is at span center...
            <div>
                <svg width={soi(2*model.clockSize)}>
                    {React.array(viewTimeText(model))}
                </svg>
            </div>
            <div onClick={event => dispatcher(translate_mouse_click(event))}>

                    <svg width={soi(2*model.clockSize)} height={soi(2*model.clockSize+20)}>
                        <circle cx={soi(model.clockLoc.x)} cy={soi(model.clockLoc.y)} r={soi(model.clockSize)} fill="black" />
                        {React.array(viewCurrentSelectionCircle(model))}
                        {React.array(viewNumbers(model))}
                    </svg>
            </div>
        </center>
        </div>
        <div className="row">
            <div className="col">
                <button onClick={_ => {maybe_set_new_alarm(model, dispatcher); dispatcher(Confirm)}} className="btn btn-primary btn-block">
                    {React.string(confirmTextHelper(model))}
                </button>
            </div>
            <div className="col">
                <button onClick={_ => dispatcher(SetAM)} className="btn btn-primary btn-block" disabled={model.alarmSetAM}>
                    {React.string("AM")}
                </button>
            </div>
        </div>
        <div className="row">
            <div className="col">
                <button onClick={_ => dispatcher(Clear  )} className="btn btn-primary btn-block" disabled={model.alarmSetState == NoTimeSelected}>
                    {React.string("Reset")}
                </button>
            </div>
            <div className="col">
                <button onClick={_ => dispatcher(SetPM)} className="btn btn-primary btn-block" disabled={!model.alarmSetAM}>
                    {React.string("PM")}
                </button>
            </div>
        </div>
        <div>
            <table className="table table-striped">
              // <thead>
              //   <tr>
              //     // <th scope="col">{React.string("#")}</th>
              //     <th scope="col">{React.string("Time")}</th>
              //     <th scope="col">{React.string("Enabled")}</th>
              //     <th scope="col">{React.string("Repeat")}</th>
              //     <th scope="col">{React.string("Cancel")}</th>
              //   </tr>
              // </thead>
              <tbody>
                {viewSetAlarms(model.setAlarms, dispatcher)}
              </tbody>
            </table>
        </div>
        <div>
            <button onClick={_ => do_turn_off_light()} className="btn btn-primary btn-warning">
                {React.string("Turn off light")}
            </button>
        </div>
    </div>
}
