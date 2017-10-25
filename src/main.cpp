/*
 *   Tested with "Sonoff - WiFi Wireless Smart Switch ESP8266 MQTT"
 *
 *   The Relay can also be toggeled with the physical pushbutton.
 *
 *   Additionally, a switch may be attached between GND and the
 *   spare pin, and its state will be exported through the `contact`
 *   property. An algorithm will discard spurious changes in the
 *   input (changes that stay for less than SWITCH_ATTACK_TIME in ms).
 *
 *   Toggling the switch will also toggle the Relay, unless the
 *   the `detach` setting is set to `true`.
 */

#include <Homie.h>

const int PIN_RELAY = 12;
const int PIN_LED = 13;
const int PIN_SWITCH = 14;
const int PIN_BUTTON = 0;

unsigned long buttonDownTime = 0;
byte buttonPressHandled = 0;
byte lastButtonState = 1;

const int SWITCH_ATTACK_TIME = 7;
unsigned long switchTime = 0;
byte lastSwitchState;
byte activeSwitchState;

HomieSetting<bool> detachedSetting("detached", "Don't control the relay from the attached switch");
HomieNode switchNode("switch", "switch");

bool switchOnHandler(HomieRange range, String value) {
  if (value != "true" && value != "false") return false;

  bool on = (value == "true");
  digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  switchNode.setProperty("on").send(value);
  Homie.getLogger() << "Switch is " << (on ? "on" : "off") << endl;

  return true;
}

void toggleRelay() {
  bool on = digitalRead(PIN_RELAY) == HIGH;
  digitalWrite(PIN_RELAY, on ? LOW : HIGH);
  switchNode.setProperty("on").send(on ? "false" : "true");
  Homie.getLogger() << "Switch is " << (on ? "off" : "on") << endl;
}

void loopHandler() {
  byte buttonState = digitalRead(PIN_BUTTON);
  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      buttonDownTime     = millis();
      buttonPressHandled = 0;
    }
    else {
      unsigned long dt = millis() - buttonDownTime;
      if ( dt >= 90 && dt <= 900 && buttonPressHandled == 0 ) {
        toggleRelay();
        buttonPressHandled = 1;
      }
    }
    lastButtonState = buttonState;
  }

  byte switchState = digitalRead(PIN_SWITCH);
  if (switchState != lastSwitchState) {
    lastSwitchState = switchState;
    switchTime = millis();
  } else if (switchState != activeSwitchState && (unsigned long)(millis() - switchTime) >= SWITCH_ATTACK_TIME) {
    activeSwitchState = switchState;
    switchNode.setProperty("contact").send(switchState ? "false" : "true"); // Active low!
    if (!detachedSetting.get()) toggleRelay();
  }
}

void setup() {
  Serial.begin(115200);
  Serial << endl << endl;

  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUTTON, INPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  lastSwitchState = activeSwitchState = digitalRead(PIN_SWITCH);
  digitalWrite(PIN_RELAY, LOW);

  Homie_setFirmware("sonoff-transparent-switch", "1.2.0");
  Homie.setLedPin(PIN_LED, LOW).setResetTrigger(PIN_BUTTON, LOW, 5000);

  detachedSetting.setDefaultValue(false);

  switchNode.advertise("on").settable(switchOnHandler);
  switchNode.advertise("contact");

  Homie.setup();
}

void loop() {
  loopHandler();
  Homie.loop();
}
