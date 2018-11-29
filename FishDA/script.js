var cpf;    //Appbuilder must override this

var HIGH = 1;
var LOW = 0;

//Pins
var LOW_PIN = 2;
var MID_PIN = 3;
var HIGH_PIN = 4;
var ATTRACTOR_ENABLE = 8;

var MOTOR_POS = 9;
var MOTOR_NEG = 10;



cpfSetup();
loop();
function digitalWrite(pin, state) {
    if (cpf) {
        cpf.request('["digitalWrite", ' + pin + ', ' + state + ']');
    }
}

function digitalRead(pin) {
    if (cpf) {
        return cpf.get("d" + pin);
        }
    return LOW;
}

function cpfSetup() {
    if(cpf){
        var setup = '["resetPin"]'
        setup +=  ', ["setPinMode", "digital", ' + LOW_PIN + ', "INPUT"]';
        setup +=  ', ["setPinMode", "digital", ' + MID_PIN + ', "INPUT"]';
        setup +=  ', ["setPinMode", "digital", ' + HIGH_PIN + ', "INPUT"]';
        alert(setup);
        alert(setup == '["resetPin"], ["setPinMode", "digital", 2, "INPUT"], ["setPinMode", "digital", 3, "INPUT"], ["setPinMode", "digital", 4, "INPUT"]');
        setup +=  ', ["setPinMode", "digital",' + ATTRACTOR_ENABLE + ', "OUTPUT"]';

        cpf.setPinMode(setup);
        alert("Setup done.");
    }
}

function loop() {
    Detecc();
    setTimeout(loop, 1000);
}

function button(buttonText) {
    var fish1 = false;
    var fish2 = false;
    var fish3 = false;

    if (buttonText == "LOW") {
        fish1 = true
        buttonText = "MID";
    } else if (buttonText == "MID") {
        fish1 = true;
        fish2 = true;
        buttonText = "HIGH";
    } else if (buttonText == "HIGH") {
        fish1 = true;
        fish2 = true;
        fish3 = true;
        buttonText = "NONE";
    } else {
        buttonText = "LOW";
    }
    document.getElementById("but").textContent = buttonText;
    fishglow("fish1", fish1); fishglow("fish2", fish2); fishglow("fish3", fish3);
}
function fishglow(fish, state) {
    if (state) {
        document.getElementById(fish).src = "assets/mainpage_fishglow.png";
    } else {
        document.getElementById(fish).src = "assets/mainpage_fish.png";
    }
}

function Done() {
    document.getElementById("state").src = "assets/none.gif";
    document.getElementById("detect").disabled = false;
    document.getElementById("attract").disabled = false;
    document.getElementById("return").disabled = false;
    document.getElementById("detect").src = "assets/mainpage_detect-button.png";
    document.getElementById("attract").src = "assets/mainpage_attract-button.png";
    document.getElementById("return").src = "assets/mainpage_return-button.png";

    motor("OFF");
}

function reflectActionState(action) {
    document.getElementById("detect").disabled = true;
    document.getElementById("attract").disabled = true;
    document.getElementById("return").disabled = true;
    document.getElementById("detect").src = "assets/mainpage_detect-button_disabled.png";
    document.getElementById("attract").src = "assets/mainpage_attract-button_disabled.png";
    document.getElementById("return").src = "assets/mainpage_return-button_disabled.png";
    document.getElementById(action).src = "assets/mainpage_" + action + "-button.png";
    document.getElementById("state").src = "assets/mainpage_notif-" + action + ".png";
}
function Detecc() {
    var buttonText = "LOW";
    //alert("8:" + cpf.get("d8") + " 9:" + cpf.get("d9") + " 10:" + cpf.get("d10"));

    if (cpf) {
        if (digitalRead(LOW_PIN)) {
            buttonText = "LOW";
        } else if (digitalRead(MID_PIN)) {
            buttonText = "MID";
        } else if (digitalRead(HIGH_PIN)) {
            buttonText = "HIGH";
        }
        
        button(buttonText);
        //alert(buttonText);
    } else {
        alert("Cpf not found.");
    }

    setTimeout(Done, 1000);
}

function Attracc() {
    //alert("Atraraccting");
    reflectActionState("attract");
    
    if (cpf) {
        digitalWrite(ATTRACTOR_ENABLE, HIGH);
    } else {
        alert("Cpf not found.");
    }
    
    alert(digitalRead(ATTRACTOR_ENABLE));
    setTimeout(Done, 1000);
}

function Retracc() {
    //alert("Retraccting");
    reflectActionState("return");
    motor("REV");
    digitalWrite(ATTRACTOR_ENABLE, LOW);

    setTimeout(Done, 1000);
}

function motor(state) {
    pos = LOW;
    neg = LOW;
    if (state == "ON") {
        pos = HIGH;
    } else if (state == "REV") {
        neg = HIGH;
    }

    digitalWrite(MOTOR_POS, pos);
    digitalWrite(MOTOR_NEG, neg);
    //setTimeout(Done, 5000);
}