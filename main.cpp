#include "stm32f103xb.h"
#include "hwlib.hpp"
#include "am2302.hpp"
#include "st7789_spi_dc_cs_rst_bw.cpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"


int main() {
    namespace target = hwlib::target;

    auto sensor = hwlib::target::pin_in_out(1, 6); // The data pin of the am2302

    auto spi = target::spi_bus_hardware(); // Setup the SPI bus with hardware support
    // SCLK: A5
    // MOSI: A7

    auto dc = hwlib::target::pin_out{hwlib::target::pins::a2};
    auto &cs = hwlib::pin_out_dummy;
    auto rst = hwlib::target::pin_out{hwlib::target::pins::a3};
    rst.write(false);

    auto display = st7789_spi_dc_cs_rst_bw(spi, dc, cs, rst);

    auto terminal = hwlib::terminal_from(display, hwlib::font_default_16x16());

    while (true) {
        auto measurment = oscarIO::am2302::measure(sensor);
        char sign = measurment.tempSign ? '-' : '+';
        terminal << "\f\n\nTemp Sensor\nBy Carlos\n\n";
        terminal << "T:  " << sign << measurment.temp / 10 << "." << measurment.temp % 10 << "C\n";
        terminal << "RH: " << measurment.rh / 10 << "." << measurment.rh % 10 << "%\n\n";
        if (measurment.tempSign || measurment.temp < 100) {
            terminal << "T too low\nfor paint LV!\n\n";
        } else if (measurment.temp > 280) {
            terminal << "T too high\nfor paint LV!\n\n";
        } else {
            terminal << "T is oke\nfor paint LV!\n\n";
        }

        if (measurment.rh < 500) {
            terminal << "RH too low\nfor paint LV!\n";
        } else if (measurment.rh > 750) {
            terminal << "RH too high\nfor paint LV!\n";
        } else {
            terminal << "RH is oke\nfor paint LV!\n";
        }
        terminal << hwlib::flush;
        hwlib::wait_ms(1000*10); //10 seconds refresh rate
    }
}

#pragma clang diagnostic pop
