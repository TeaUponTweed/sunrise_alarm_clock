type alarm_state_t =
    | NoTimeSelected
    | HourSelected(int)
    | HourAndMinuteSelected(int, int);


type repeat_t = {
    monday: bool,
    tuesday: bool,
    wednesday: bool,
    thursday: bool,
    friday: bool,
    saturday: bool,
    sunday: bool,
};

type alarm_time_t = {
    hour: int,
    minute: int,
    isAM: bool,
    alarmID: int,
    enabled: bool,
    repeat: repeat_t,
};

type alarm_list_response_t = {
    alarms: list(alarm_time_t),
    time_till_most_recent_alarm: option(string)
};

type light_status = {
    connected: bool,
};

type position_t = {
    x: int,
    y: int
};
