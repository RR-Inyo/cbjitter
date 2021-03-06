 /*
 * cbjitter_1.c
 * Measurement o pigpio library callback jitter: gpioSetAlertFunc() version
 * (c) 2021 @RR_Inyo
 * Released under the MIT license
 * https://opensource.org/licenses/mit-license.php
 */

#include <stdio.h>
#include <stdlib.h>
#include <pigpio.h>

/* Define pins */
#define RED_LED     17              /* GPIO connected to red LED on IoT Learning HAT */
#define GREEN_LED   27              /* GPIO connected to green LED on IoT learning HAT */
#define NTIMES      100             /* Number of times of single tests */
#define SLEEP_US    2000            /* [microsec] to sleep between single tests */
#define PATH        "results.csv"   /* File name to store results

/* Define global variables */
int k = 0;          /* Number of times the callback function is called */
FILE *f;            /* File pointer to write results */
uint32_t tick_old;  /* Tick when GPIO connected to green LED changed */

/* Call back function */
void cbf(int gpio, int level, uint32_t tick)
{
    /* Return if falling edge */
    if (level == 0) return;

    /* Turn on the red LED */
    gpioWrite(RED_LED, 1);

    /* Calculate tick difference between GPIO change and callback called */
    uint32_t tick_diff;
    tick_diff = gpioTick() - tick_old;

    fprintf(f, "%d\t%u\n", k, tick_diff);

    /* Turn off the red LED */
    gpioWrite(RED_LED, 0);
}

int main(int argc, char *argv[])
{
    /* Message */
    printf("cbjitter_1 - Measurement of pigpio library callback jitter: gpioSetAlertFunc() version\n");
    printf("(c) 2021 RR_Inyo\n");

    /* Set number of times of single tests from arguments */
    int ntimes;
    if (argc < 2) {
        ntimes = NTIMES;        /* Default number */
    } else {
        ntimes = atoi(argv[1]); /* Set from argument */
    }

    /* Open a file to write results */
    printf("cbjitter_1 >>> Opening a file to store results...\n");
    f = fopen(PATH, "w");
    if (!f) {
        printf("Can't create file.\n");
        exit(1);
    }
    fprintf(f, "k\tTick difference\n");

    /* Initialize and set GPIO */
    printf("cbjitter_1 >>> Initializing pigpio and setting GPIO...\n");
    if(gpioInitialise() < 0) {
        printf("pigpio initialization failed.\n");
        exit(1);
    } else {
        printf("pigpio initialization OK.\n");
    }
    gpioWrite(RED_LED, 0);
    gpioWrite(GREEN_LED, 0);
    gpioSetMode(RED_LED, PI_OUTPUT);
    gpioSetMode(GREEN_LED, PI_OUTPUT);

    /* Assign callback function */
    printf("cbjitter_1 >>> Assigning callback function by gpioSetAlertFunc()...\n");
    gpioSetAlertFunc(GREEN_LED, cbf);

    /* Execute test */
    printf("cbjitter_1 >>> Executing tests, %d times...\n", ntimes);
    for (k = 0; k < ntimes; k++) {
        tick_old = gpioTick();
        gpioWrite(GREEN_LED, 1);
        gpioSleep(PI_TIME_RELATIVE, 0, SLEEP_US);
        gpioWrite(GREEN_LED, 0);
        gpioSleep(PI_TIME_RELATIVE, 0, SLEEP_US);
    }

    /* Set GPIO to finish */
    printf("cbjitter_1 >>> Setting GPIO to finish...\n");
    gpioSetAlertFunc(GREEN_LED, NULL);
    gpioTerminate();

    /* Close file */
    printf("cbjitter_1 >>> Closing the file to store results...\n");
    if(fclose(f) == EOF) {
        printf("Can't close file.\n");
        exit(1);
    }

    /* Exit program successfully */
    printf("cbjitter_1 >>> Finished!\n");
    exit(0);
}
