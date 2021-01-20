//
// Made by a second year student of Wouter van Ooijen
//

using namespace hwlib;
namespace target = hwlib::target;
class st7789_spi_dc_cs_rst_bw : public st7789, public window {
private:

    static auto constexpr wsize = xy(240, 240);

    uint8_t buffer[(wsize.x * wsize.y) / 8];

    void write_implementation(
            xy pos,
            color col
    ) override {
        if (col == white) {
            buffer[(pos.x + wsize.x * pos.y) / 8] |= (0x01 << (pos.x % 8));
        } else {
            buffer[(pos.x + wsize.x * pos.y) / 8] &= ~(0x01 << (pos.x % 8));
        }
    }

    spi_bus &bus;
    pin_out &dc;
    pin_out &cs;
    pin_out &rst;

public:

    void command(
            commands c
    ) {
        dc.write(0);
        dc.flush();
        auto transaction = bus.transaction(cs);
        transaction.write(static_cast< uint8_t >( c ));
        dc.write(1);
        dc.flush();
    }

    void command(
            commands c,
            uint8_t d0
    ) {
        dc.write(0);
        dc.flush();
        auto transaction = bus.transaction(cs);
        transaction.write(static_cast< uint8_t >( c ));
        dc.write(1);
        dc.flush();
        transaction.write(d0);
    }

    void command(
            commands c,
            uint8_t d0,
            uint8_t d1,
            uint8_t d2,
            uint8_t d3
    ) {
        dc.write(0);
        dc.flush();
        auto transaction = bus.transaction(cs);
        transaction.write(static_cast< uint8_t >( c ));
        dc.write(1);
        dc.flush();
        transaction.write(d0);
        transaction.write(d1);
        transaction.write(d2);
        transaction.write(d3);
    }

    st7789_spi_dc_cs_rst_bw(
            spi_bus &bus,
            pin_out &dc,
            pin_out &cs,
            pin_out &rst
    ) :
            window(wsize, white, black),
            bus(bus),
            dc(dc),
            cs(cs),
            rst(rst) {
        rst.write(0);
        rst.flush();
        wait_ms(200);
        rst.write(1);
        rst.flush();
        wait_ms(200);

        command(commands::SWRESET);
        wait_ms(150);

        command(commands::SLPOUT);
        wait_ms(10);

        command(commands::COLMOD, 0x66);
        wait_ms(10);

        command(commands::MADCTL, 0x10);
        command(commands::CASET, 0, 0, wsize.x >> 8, wsize.x & 0xFF);
        command(commands::RASET, 0, 0, wsize.y >> 8, wsize.y & 0xFF);

        command(commands::INVON);
        wait_ms(10);
        command(commands::NORON);
        wait_ms(100);
        command(commands::DISPON);
        wait_ms(100);
    }

    void flush() override {
        dc.write(0);
        dc.flush();
        auto transaction = bus.transaction(cs);
        transaction.write(static_cast< uint8_t >( commands::RAMWR ));
        dc.write(1);
        dc.flush();
        for (int i = 0; i < 240 * 240; ++i) {
            // transaction.write( ( buffer[ i ] << 2 ) & 0xC0 );
            // transaction.write( ( buffer[ i ] << 4 ) & 0xC0 );
            // transaction.write( ( buffer[ i ] << 6 ) & 0xC0 );
            if ((buffer[i / 8] >> (i % 8)) & 0x01) {
                transaction.write(0xFF);
                transaction.write(0xFF);
                transaction.write(0xFF);
            } else {
                transaction.write(0x00);
                transaction.write(0x00);
                transaction.write(0x00);
            }
        }
    }

}; // class st7789_spi_dc_cs_rst