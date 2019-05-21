# Fairy Tale Phone
This file is used to list changes made in each version of the Fairy Tale Phone Arduino project
## v1.1
- Share the same pin for sound and tone by implementing deInit()
- Power sleep after fairy tale finished or error to save battery in case kids forget to hang up
- Power sleep after 10s if wheel hasn't been dialed to save battery (hook off by accident)
