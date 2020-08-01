#include "mbed.h"
#include "platform/mbed_thread.h"


// Blinking rate in milliseconds
#define BLINKING_RATE_MS    250

AnalogIn vert(p19);
AnalogIn horz(p20);

DigitalOut col1(p28);
DigitalOut col2(p30);
DigitalOut col3(p26);
DigitalIn row1(p29);
DigitalIn row2(p24);
DigitalIn row3(p25);
DigitalIn row4(p27);

#define NUM_SAMPLE_INIT     20

char compute_val(double val, double center);

char read_keypad () {
    const char matrix[12] = {'1', '4', '7', '*', '2', '5', '8', '0', '3', '6', '9', '#'};

    for (int i = 0; i < 3; i++) {
        // reset columns
        col1 = 0;
        col2 = 0;
        col3 = 0;

        // set active column
        col1 = i == 0 ? 1 : 0;
        col2 = i == 1 ? 1 : 0;
        col3 = i == 2 ? 1 : 0;

        // wait
        thread_sleep_for(10);

        // read row
        int row = row1 ? 0: row2 ? 1 : row3 ? 2 :row4 ? 3 : 0xff;

        if (row < 0xff) {
            printf("%d, %d\n\r", row, i);
            return matrix[i*4 + row];
        }
    }
    return ' ';
}

int main()
{
    DigitalOut led(LED1);

    double vert_center = 0.0;
    double horz_center = 0.0;
    for (int i = 0; i < NUM_SAMPLE_INIT; i++) {
        vert_center = vert_center + vert.read();
        horz_center = horz_center + horz.read();
        thread_sleep_for(100);
    }
    vert_center = vert_center / NUM_SAMPLE_INIT;
    horz_center = horz_center / NUM_SAMPLE_INIT;

    while (true) {
        char vert_read = compute_val(vert.read(), vert_center);
        char horz_read = compute_val(horz.read(), horz_center);
        
        printf("vert %d%%, horz %d%%\n\r", vert_read, horz_read);

        char c = read_keypad();
        if (c != ' ')
            printf("Keypad %c\n\r", c);

        led = !led;
        thread_sleep_for(BLINKING_RATE_MS);
    }
}

char compute_val(double val, double center) {
    double range;

    range  = (val > center) ? (1.0 - center) : center;
    val = val - center;

    return (char)(floor((val / range) * 100));
}
