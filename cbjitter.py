#!/usr/bin/python3
# -*- coding: utf-8 -*-

#
# cbjitter.py
# Measurement of pigpio module callback jitter
# (c) 2021 @RR_Inyo
#

import sys
import time
import pigpio

# Define pins and constants
RED_LED = 17            # GPIO pin connected to red LED on IoT Learning HAT
GREEN_LED = 27          # GPIO pin connected to green LED on IoT Learning HAT
NTIMES = 1000           # Number of times of single tests
TSLEEP = 0.002          # [sec] to sleep between single tests
PATH = 'results.csv'    # File name to store results

# Show message
print('cbjitter.py - Measurement of pigpio module callback jitter in Python')
print('(c) 2021 @RR_Inyo')

# Set number of single tests from argument
if len(sys.argv) < 2:
    ntimes = NTIMES
else:
    ntimes = int(sys.argv[1])

# Open a file to store results
print('cbjitter.py >>> Opening a file to store results...')
f = open(PATH, "w")
f.write('k\tTick difference\n')

# Initialize and set GPIO
print('cbjitter.py >>> Initializing and setting GPIO...')
pi = pigpio.pi()
pi.write(RED_LED, 0)
pi.write(GREEN_LED, 0)
pi.set_mode(RED_LED, pigpio.OUTPUT)
pi.set_mode(GREEN_LED, pigpio.OUTPUT)

# Callback function
def cb_interrupt(gpio, level, tick):

    # Return if falling edge
    if level == 0: return

    # Turn on the red LED
    pi.write(RED_LED, 1)

    # Calculate tick difference between GPIO change and callback called
    tick_diff = pigpio.tickDiff(tick_old, pi.get_current_tick())

    # Write results to file
    f.write(f'{k}\t{tick_diff}\n')

    # Turn off the red LED
    pi.write(RED_LED, 0)

# Assign callback function
print('cbjitter.py >>> Assigning callback function...')
cb = pi.callback(GREEN_LED, pigpio.EITHER_EDGE, cb_interrupt)

# Execute test
print(f'cbjitter.py >>> Executing tests, {ntimes} times...')
for k in range(0, ntimes):
    tick_old = pi.get_current_tick()
    pi.write(GREEN_LED, 1)
    time.sleep(TSLEEP)
    pi.write(GREEN_LED, 0)
    time.sleep(TSLEEP)

# Set GPIO to finish
print('cbjitter.py >>> Setting GPIO to finish...')
cb.cancel()
pi.set_mode(RED_LED, pigpio.INPUT)
pi.set_mode(GREEN_LED, pigpio.INPUT)
pi.stop()

# Close file
print('cbjitter.py >>> Closing the file to store results...')
f.close()

# Exit from program successfully
print('cbjitter.py >>> Finished!')
