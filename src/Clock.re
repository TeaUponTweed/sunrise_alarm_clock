open Model
open Update
open View
open Server

[@react.component]
let make = (~x, ~y, ~r) => {
    // initialize model
    let init_state : model_t = {
        alarmSetState: HourSelected(12),
        clockLoc: {
            x: x,
            y: y
        },
        clockSize: r,
        alarmSetAM: true,
        setAlarms: None
    };

    // setup update reduced
    let (model: model_t, dispatcher) = React.useReducer(update, init_state);

    // Get intial set alarms from server
    let _ = switch (model.setAlarms) {
        | Some(_) => ()
        | None => get_alarms(dispatcher);
    };

    // TODO periodically check light connection

    // view
    viewModel(model, dispatcher)
}
