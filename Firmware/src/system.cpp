#include "system.h"

System::System():
    RicCoreSystem(Commands::command_map,Commands::defaultEnabledCommands,Serial),
    vspi(0),
    hspi(1),
    sensors(hspi, systemstatus),
    estimator(systemstatus),
    nandflash(vspi, PinMap::IC_Cs, PinMap::IC_WP, PinMap::IC_Hold),
    filesystem(nandflash)

{};

void System::systemSetup() {
    
    Serial.setRxBufferSize(GeneralConfig::SerialRxSize);
    Serial.begin(GeneralConfig::SerialBaud);

    setupSPI();
    setupPins();

    estimator.setup();

    statemachine.initalize(std::make_unique<Idle>(systemstatus,commandhandler));

    delay(3000); //wait forem filesystem; printing purposes

    // Serial.println(filesystem.setup() ? "Filesystem mounted" : "Filesystem mount failed");
    // filesystem.print_disk_space();
     
};
// #include <librnp/rnp_packet.h>
// #include "test.h"
void System::systemUpdate(){

    sensors.update();
    estimator.update(sensors.getData());

    // MessagePacket_Base<0,0> msg("hello");
    // dumpTx(msg);  // prints the serialized header+body bytes

    // RnpPacketSerialized rx(bytes_from_uart);
    // dumpRx(rx); 

};

void System::setupSPI()
{
    hspi.begin(PinMap::SNSR_SCLK,PinMap::SNSR_MISO,PinMap::SNSR_MOSI);
    hspi.setFrequency(8000000);
    hspi.setBitOrder(MSBFIRST);
    hspi.setDataMode(SPI_MODE0);

    vspi.begin(PinMap::IC_SCLK,PinMap::IC_MISO,PinMap::IC_MOSI);
    vspi.setFrequency(1000000);
    vspi.setBitOrder(MSBFIRST);
    vspi.setDataMode(SPI_MODE0); // Set the SPI data mode
}

void System::setupPins()
{
    pinMode(PinMap::Accel_Gyro_Cs, OUTPUT);
    pinMode(PinMap::Accel_Cs, OUTPUT);
    pinMode(PinMap::Baro_Cs, OUTPUT);
    pinMode(PinMap::Mag_Cs, OUTPUT);
    pinMode(PinMap::IC_Cs, OUTPUT);
    pinMode(PinMap::IC_WP, OUTPUT);
    pinMode(PinMap::LED_RED, OUTPUT);
    pinMode(PinMap::LED_BLUE, OUTPUT);
    pinMode(PinMap::LED_GREEN, OUTPUT);
    // initialise cs pins
    digitalWrite(PinMap::Accel_Gyro_Cs, HIGH);
    digitalWrite(PinMap::Accel_Cs, HIGH);
    digitalWrite(PinMap::Baro_Cs, HIGH);
    digitalWrite(PinMap::Mag_Cs, HIGH);
    digitalWrite(PinMap::IC_Cs, HIGH);
    digitalWrite(PinMap::IC_WP, HIGH);
    digitalWrite(PinMap::LED_RED, HIGH);
    digitalWrite(PinMap::LED_BLUE, HIGH);
    digitalWrite(PinMap::LED_GREEN, HIGH);
}



