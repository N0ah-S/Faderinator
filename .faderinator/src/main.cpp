#include <uart_rp2040.h>
#include "Application.h"

int main() {
    uart_rp2040 uart(0, 0, 1, 115200/2, Application::MODE);
    Application application(uart);

    while (true) {
        application.loop();
    }
}
