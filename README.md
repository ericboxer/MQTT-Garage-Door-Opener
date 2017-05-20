# MQTT Garage Door Opener
---

## A quick little sketch used to operate my garage door.

### Main Info
---
I use an Adafruit [Huzzah Feather][HuzzahFeather] with the [Non-Latching Mini Relay][RelayWing]. I tie the relay into the pushbutton inside the garage to using the normally open configuration.
I bought a cheap [Magnetic Sensor][MagSensor] from Amazon and set it up to be active when the garage door is open.

This is part of a larger integration to [Home Assistant][HA] that I am working on installing throughout the house. You can find my larger project [here][MYHA].

One of the biggest challanges I had was the relay resetting every time I power cycled. It ultimately ended up being a pin sleection (Pin 4 on the feather). 

### If I were to rebuild (And I might)
---
I would use a differnt wireless chip. The Huzzah is more than capable of doing the job, however the pinout is a bit odd and limited.
I would also add a second sensor for the closed position. This is, of course, an easy addition. I just need to find the time to do it. It also allows for a positive assurance that the door is actually closed.


[HuzzahFeather]: http://bxhd.me/2r3hZvF
[RelayWing]: http://bxhd.me/2rCgH7N
[HA]: http://home-assistant.io
[MYHA]: #
[MagSensor]: http://bxhd.me/2r3jj1u
