open Types

let soi = string_of_int;

let foi = float_of_int;

let iof = int_of_float;

let px = m => soi(m) ++ "px";

let scale = (n, max, range) => iof(foi(n) /. foi(max) *. foi(range));

let clickNorm = (center, xy, xoffset) => {
    let dx = foi(xy.x - center.x + xoffset);
    let dy = foi(xy.y - center.y);
    let norm = sqrt(dx*.dx +. dy*.dy);
    let nx = dx/.norm;
    let ny = dy/.norm;
    (norm, nx, ny)
}

let degreesFromClick = (clock_loc, clock_size, click) => {
    let (_, nx, ny) = clickNorm(clock_loc, click, clock_size);
    let deg = (mod_float(atan2(ny, nx)+.(2.*.3.1415926), 2.*.3.1415926)*.360./.(2.*.3.1415926)) +. 90.;
    if (deg > 360.) {
        deg -. 360.
    } else {
        deg
    }
}

let hourFromClick =  (clock_loc, clock_size, click) => {
    let deg = degreesFromClick(clock_loc, clock_size, click)
    let hour = iof(deg*.12./.360.+.0.5)
    if (hour == 0) {
        12
    }
    else {
        hour
    }
}


let minuteFromClick =  (clock_loc, clock_size, click) => {
    let deg = degreesFromClick(clock_loc, clock_size, click)
    let minute = iof(deg*.60./.360.+.0.5)
    if (minute == 60) {
        0
    }
    else {
        minute
    }
}

let pads = (s, n, ch) => {
    let npad = n - String.length(s)
    if (npad > 0) {
        String.make(npad, ch) ++ s
    }
    else
    {
        s
    }
}


let getDisplayTime = (alarmState) => {
    let (hour, min) = switch (alarmState) {
        | NoTimeSelected => (12, 0)
        | HourSelected(hour) => (hour, 0)
        | HourAndMinuteSelected(hour,min) => (hour, min)
    };
    (hour, min)
};

// https://github.com/reasonml-community/reason-maze/blob/master/src/UI/ColorSlider.re
// external refToCanvas : Dom.element => Canvas.canvasElement = "%identity";
external convertToDomElement : Js.t({..}) => Webapi.Dom.Element.t = "%identity";

let getElementPos = (element) => {
  let element = ReactDOMRe.domElementToObj(element);
  let box = element##getBoundingClientRect();
  let top: int = box##top;
  let left: int = box##left;
  let width: int = box##width;
  let height: int = box##height;
  (top, left, width, height);
};

