# sonoff-transparent-switch

The [Sonoff][] is a (cheap, very cheap) WiFi-activated relay based on ESP8266.

This is an alternative firwmare made exclusively for the regular Sonoff (with
only one switch and no ability to report power) that uses the [Homie-ESP8266][]
platform.

Like most alternative firmwares, this uses MQTT to send and receive commands
to turn the relay on or off. Unless the other alternative firmwares, this one
allows you to plug a physical switch (pushbuttons aren't supported yet) between
GND and GPIO#14. You can then use that switch to turn the light on or off,
in addition to MQTT.

This has obvious advantages:

 - It can be installed using the switch already present for that light.

 - The installation is transparent; the user can keep using the light through
   the switch without even noticing there's a relay involved.

 - Even when WiFi, Internet or the MQTT broker are down, you can still use the
   light through the switch.

 - The MQTT broker is kept updated whenever you use the switch.

This has been working reliably at many homes and I see it ready to release.

## Usage

Detailed building instructions coming someday to the blog. If you want to use
the existing wallswitch (or chain of wallswitches), you should make sure those
have **no AC current whatsoever** and are only connected to GND and #14. Make
sure connections are well done.

Then, flash this firmware (prebuilt images can be found on the [Releases][]
page) onto your Sonoff, and it will boot into configuration mode. Use [this
website](http://setup.homie-esp8266.marvinroger.fr) or
[this Android app](https://build.phonegap.com/apps/1906578/share) to
configure it; you'll be asked the WiFi network details, device ID and the MQTT
broker details.

Once configured, the Sonoff will reboot and connect to your MQTT broker.
At that point, you can start issuing commands to control the relay.
An example [Home Assistant][] configuration:

~~~ yaml
light:
  - platform: mqtt
    name: "Dining room"
    state_topic: "homie/<id>/switch/on"
    command_topic: "homie/<id>/switch/on/set"
    payload_on: "true"
    payload_off: "false"
~~~



[Homie-ESP8266]: https://github.com/marvinroger/homie-esp8266
[Sonoff]: https://itead.cc/sonoff-wifi-wireless-switch.html
[Releases]: https://github.com/jmendeth/sonoff-transparent-switch/releases
[Home Assistant]: https://home-assistant.io
