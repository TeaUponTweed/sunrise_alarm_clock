open Model
open Types
open Utils

module Decode = {
    let repeat = json =>
        Json.Decode.{
            monday: json |> field("monday",  bool),
            tuesday: json |> field("tuesday",  bool),
            wednesday: json |> field("wednesday",  bool),
            thursday: json |> field("thursday",  bool),
            friday: json |> field("friday",  bool),
            saturday: json |> field("saturday",  bool),
            sunday: json |> field("sunday",  bool),
    };

    let alarm_time = json =>
        Json.Decode.{
            hour: json |> field("hour", int),
            minute: json |> field("minute", int),
            alarmID: json |> field("alarmID", int),
            isAM: json |> field("isAM", bool),
            enabled: json |> field("enabled", bool),
            repeat: json |> field("repeat", repeat)
    };

    let alarm_list_response = json =>
        Json.Decode.{
            alarms: json |> field("alarms", list(alarm_time)),
            time_till_most_recent_alarm : json |> optional(field("time_till_most_recent_alarm", string))
    };

    let light_status = json =>
        Json.Decode.{
                    connected: json |> field("connected", bool)
            };
};


let send_to_api = (payload, api, decoder) => {
    Js.Promise.(
        Fetch.fetchWithInit(
            api,
            Fetch.RequestInit.make(
                ~method_=Post,
                ~body=Fetch.BodyInit.make(Js.Json.stringify(Js.Json.object_(payload))),
                ~headers=Fetch.HeadersInit.make({"Content-Type": "application/json"}),
                ()
            )
        )
        |> then_(Fetch.Response.text)
        |> then_(data => {
                    data |> Json.parseOrRaise
                             |> decoder
                             |> resolve
        })
    )
}

let construct_repeat_payload_with_default = (repeat) => {
    let repeat = switch (repeat) {
        | Some(repeat) => repeat
        | None => {
            monday: true,
            tuesday: true,
            wednesday: true,
            thursday: true,
            friday: true,
            saturday: true,
            sunday: true,
        }
    };
    let repeat_payload = Js.Dict.empty();
    Js.Dict.set(repeat_payload, "monday", Js.Json.boolean(repeat.monday));
    Js.Dict.set(repeat_payload, "tuesday", Js.Json.boolean(repeat.tuesday));
    Js.Dict.set(repeat_payload, "wednesday", Js.Json.boolean(repeat.wednesday));
    Js.Dict.set(repeat_payload, "thursday", Js.Json.boolean(repeat.thursday));
    Js.Dict.set(repeat_payload, "friday", Js.Json.boolean(repeat.friday));
    Js.Dict.set(repeat_payload, "saturday", Js.Json.boolean(repeat.saturday));
    Js.Dict.set(repeat_payload, "sunday", Js.Json.boolean(repeat.sunday));
    repeat_payload
};

let send_set_alarm = (hour, minute, isAM, enabled, repeat, alarmID) => {
    let payload = Js.Dict.empty();
    // TODO use a proper encoder
    Js.Dict.set(payload, "hour", Js.Json.string(soi(hour)));
    Js.Dict.set(payload, "minute", Js.Json.string(soi(minute)));
    Js.Dict.set(payload, "isAM", Js.Json.boolean(isAM));
    Js.Dict.set(payload, "alarmID", Js.Json.string(soi(alarmID)));
    Js.Dict.set(payload, "enabled", Js.Json.boolean(enabled));
    Js.Dict.set(payload, "repeat", Js.Json.object_(construct_repeat_payload_with_default(repeat)));
    send_to_api(payload, "/api/set_alarm", Decode.alarm_list_response)
};


let send_cancel_alarm_impl = (alarmID) => {
    let payload = Js.Dict.empty();
    Js.Dict.set(payload, "alarmID", Js.Json.string(soi(alarmID)));
    send_to_api(payload, "/api/cancel_alarm", Decode.alarm_list_response)
};

let get_from_api = (api, decoder) => {
    Js.Promise.(
        Fetch.fetch(api)
        |> then_(Fetch.Response.text)
        |> then_(data => {
            data |> Json.parseOrRaise
                 |> decoder
                 |> resolve
        })
    )
};


let get_alarms = (dispatcher) => {
    let _ = get_from_api("/api/alarms", Decode.alarm_list_response)
        |> Js.Promise.then_(alarm_list => {
            UpdateSetAlarms(alarm_list)
                |> dispatcher
                |> Js.Promise.resolve
            });
    ()
};

let send_cancel_alarm = (set_alarm, dispatcher) => {
    let _ = send_cancel_alarm_impl(set_alarm.alarmID)
        |> Js.Promise.then_(alarm_list => {
            UpdateSetAlarms(alarm_list)
                 |> dispatcher
                 |> Js.Promise.resolve
        });
    ()
};

let do_turn_off_light = () => {
    let _ = get_from_api("/api/turn_off_light", Decode.light_status)
};

// TODO wrap reducer in a meta function that can handle promises
let maybe_set_new_alarm = (model, dispatcher) => {
    switch (model.alarmSetState) {
        | HourAndMinuteSelected(hour, minute) => {
            let _ = send_set_alarm(hour, minute, model.alarmSetAM, true, None, -1) // -1 signals server to allocate ID
                |> Js.Promise.then_(alarm_list => {
                    UpdateSetAlarms(alarm_list)
                        |> dispatcher
                        |> Js.Promise.resolve
                });
            ()
        }
        | HourSelected(_) | NoTimeSelected => {
            ()
        }
    }
};

let send_updated_alarm_state = (alarm, dispatcher) => {
    let _ = send_set_alarm(alarm.hour, alarm.minute, alarm.isAM, alarm.enabled, Some(alarm.repeat), alarm.alarmID)
        |> Js.Promise.then_(alarm_list => {
            UpdateSetAlarms(alarm_list)
                |> dispatcher
                |> Js.Promise.resolve
        });
}
