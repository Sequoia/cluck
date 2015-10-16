# cluck

This is:

1. a sketch for the Sparkfun Thing (ESP8266 board) that posts the state of a door (when it changes) to <data.sparkfun.com>
2. a webpage that reads the data from <data.sparkfun.com> 1x/second and updates the page text/color when it's changed.

## Arduino sketch

1. Wake up from sleep
2. Read last state from EEPROM
3. Read new state from door sensor
4. If they are different:
  1. flash onboard LED 1x
  2. connect WIFI
  3. post new state to data.sparkfun.com
  4. flash onboard LED 2x
  5. disconnect WIFI
5. go to sleep for specified # of seconds
6. `GOTO` `1`

## Webpage

1. read new status from data.sparkfun.com 1x/second
2. If it's different, update page

## Don't forget!

* connect `DTR` to `XPD` to utilize `deepSleep`. If you do not, your thing will not wake up from sleep 'til it's manually reset
* on wakeup, sketch starts from the top so state can only be preserved across sleep by **writing to EEPROM** before sleep & reading from it after
