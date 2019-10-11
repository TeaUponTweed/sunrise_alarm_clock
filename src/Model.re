open Types

// model

type model_t = {
    alarmSetState: alarm_state_t,
    clockLoc: position_t,
    clockSize: int,
    alarmSetAM: bool,
    setAlarms: option(list(alarm_time_t))
};

type msg_t =
    | MouseClicked(position_t)
    | Clear
    | Confirm
    | SetAM
    | SetPM
    | UpdateSetAlarms(alarm_list_response_t)
