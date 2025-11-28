#include "system.h"

System::System() :
    sensors(SPI2_HOST)
    // estimator(systemstatus)
    // nandflash(vspi, PinMap::IC_Cs, PinMap::IC_WP, PinMap::IC_Hold),
    // filesystem(nandflash)

{};

void System::systemSetup() {

    setupSPI();
    setupPins();

    // estimator.setup();
    sensors.setup();

};

void System::systemUpdate(){

    sensors.update();
    // estimator.update(sensors.getData());
    // telemetry.sendData(sensors.getData());
};

void System::setupSPI()
{
    spi_bus_config_t hspi_bus = {
        .mosi_io_num = PinMap::SNSR_MOSI,
        .miso_io_num = PinMap::SNSR_MISO,
        .sclk_io_num = PinMap::SNSR_SCLK,
        .max_transfer_sz = 4096
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &hspi_bus, SPI_DMA_CH_AUTO));
    ESP_LOGI(TAG, "SPI2 bus initialized (SNSR_* pins)");


    // vspi.begin(PinMap::IC_SCLK,PinMap::IC_MISO,PinMap::IC_MOSI);
    // vspi.setFrequency(8000000);
    // vspi.setBitOrder(MSBFIRST);
    // vspi.setDataMode(SPI_MODE0); // Set the SPI data mode
}

void System::setupPins()
{

    const gpio_num_t outs[] = {
        (gpio_num_t)PinMap::Accel_Gyro_Cs,
        (gpio_num_t)PinMap::Accel_Cs,
        (gpio_num_t)PinMap::Baro_Cs,
        (gpio_num_t)PinMap::Mag_Cs,
        (gpio_num_t)PinMap::IC_Cs,
        (gpio_num_t)PinMap::IC_WP,
        (gpio_num_t)PinMap::LED_RED,
        (gpio_num_t)PinMap::LED_BLUE,
        // (gpio_num_t)PinMap::LED_GREEN,
    };

    // Build a bit mask for gpio_config()
    uint64_t mask = 0;
    for (size_t i = 0; i < sizeof(outs)/sizeof(outs[0]); ++i) {
        mask |= (1ULL << outs[i]);
    }

    gpio_config_t io = {};
    io.intr_type = GPIO_INTR_DISABLE;
    io.mode = GPIO_MODE_OUTPUT;           // push-pull outputs
    io.pull_up_en = GPIO_PULLUP_DISABLE;
    io.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io.pin_bit_mask = mask;
    gpio_config(&io);

    // Set initial levels HIGH (CS inactive, LEDs off if active-high)
    for (size_t i = 0; i < sizeof(outs)/sizeof(outs[0]); ++i) {
        gpio_set_level(outs[i], 1);
    }
}

