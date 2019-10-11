open Model
open Utils
open Types

let translate_mouse_click = (e) => {
    let dom = ReactEvent.Mouse.currentTarget(e);

    let (top, left, width, _) = getElementPos(convertToDomElement(dom));
    let click = {
        x: ReactEvent.Mouse.clientX(e) - left - width/2,
        y: ReactEvent.Mouse.clientY(e) - top
    };
    MouseClicked(click)
};


let handleMouseClicked = (alarmState, clock_loc, clock_size, click) => {
    let (norm, _, _) = clickNorm(clock_loc, click, clock_size);
    if (iof(norm) > 10000000) {
        None
    } else {
        switch (alarmState) {
            | NoTimeSelected => {
                let hour = hourFromClick(clock_loc, clock_size, click);
                Some(HourSelected(hour))
            }
            | HourSelected(_) => {
                let hour = hourFromClick(clock_loc, clock_size, click);
                Some(HourAndMinuteSelected(hour, 0))
            }
            | HourAndMinuteSelected(hour, _) => {
                let minute = minuteFromClick(clock_loc, clock_size, click);
                Some(HourAndMinuteSelected(hour, minute))
            }
        }
    }
}

let update = (model: model_t, msg : msg_t) => {
    switch msg {
        | MouseClicked(clickLoc) => {
            let updatedAlarmState = handleMouseClicked(model.alarmSetState, model.clockLoc, model.clockSize, clickLoc);
            switch (updatedAlarmState) {
                | Some(alarmState) => {
                    ...model,
                    alarmSetState: alarmState
                }
                | None => model
            }
        }
        | Clear => {
            {
                ...model,
                alarmSetState: HourSelected(12)
            }
        }
        | Confirm => {
            // TODO send message to server
            switch (model.alarmSetState) {
                | NoTimeSelected => model
                | HourSelected(hour) => {
                    {
                        {
                            ...model,
                            alarmSetState: HourAndMinuteSelected(hour, 0)
                        }
                    }
                }
                | HourAndMinuteSelected(_, _) => {
                    {
                        ...model,
                        alarmSetState: HourSelected(12)
                    }
                }
            }
        }
        | SetAM => {
            {
                ...model,
                alarmSetAM: true
            }
        }
        | SetPM => {
            {
                ...model,
                alarmSetAM: false
            }
        }
        | UpdateSetAlarms(alarms) => {
            {
                ...model,
                setAlarms: Some(alarms.alarms)
            }
        }
    };
};
