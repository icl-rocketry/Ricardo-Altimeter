#include "commands.h"

#include "system.h"
#include "States/msc.h"

void Commands::TelemetryCommand(System& system, const RnpPacketSerialized& packet) 
{

	SimpleCommandPacket commandpacket(packet);

	TelemetryPacket telemetry;

	auto raw_sensors = system.sensors.getData();
	auto estimator_state = system.estimator.getData();

	telemetry.header.type = 101;
	telemetry.header.source = system.networkmanager.getAddress();
	telemetry.header.source_service = static_cast<uint8_t>(DEFAULT_SERVICES::COMMAND);
	telemetry.header.destination = commandpacket.header.source;
	telemetry.header.destination_service = commandpacket.header.source_service;
	telemetry.header.uid = commandpacket.header.uid; 
	telemetry.system_time = millis();

	telemetry.pn = estimator_state.position(0);
	telemetry.pe = estimator_state.position(1);
	telemetry.pd = estimator_state.position(2);

	telemetry.vn = estimator_state.velocity(0);
	telemetry.ve = estimator_state.velocity(1);
	telemetry.vd = estimator_state.velocity(2);

	telemetry.an = estimator_state.acceleration(0);
	telemetry.ae = estimator_state.acceleration(1);
	telemetry.ad = estimator_state.acceleration(2);

	telemetry.roll = estimator_state.eulerAngles(0);
	telemetry.pitch = estimator_state.eulerAngles(1);
	telemetry.yaw =estimator_state.eulerAngles(2);

	telemetry.q0 = estimator_state.orientation.w();
	telemetry.q1 = estimator_state.orientation.x();
	telemetry.q2 =estimator_state.orientation.y();
	telemetry.q3 =estimator_state.orientation.z();

	telemetry.ax = raw_sensors.accelgyro.ax;
	telemetry.ay = raw_sensors.accelgyro.ay;
	telemetry.az = raw_sensors.accelgyro.az;

	telemetry.h_ax = raw_sensors.accel.ax;
	telemetry.h_ay = raw_sensors.accel.ay;
	telemetry.h_az = raw_sensors.accel.az;

	telemetry.gx = raw_sensors.accelgyro.gx;
	telemetry.gy = raw_sensors.accelgyro.gy;
	telemetry.gz = raw_sensors.accelgyro.gz;

	telemetry.mx = raw_sensors.mag.mx;
	telemetry.my = raw_sensors.mag.my;
	telemetry.mz = raw_sensors.mag.mz;

	telemetry.baro_temp = raw_sensors.baro.temp;
	telemetry.baro_press = raw_sensors.baro.press;
	telemetry.baro_alt = raw_sensors.baro.alt;

	telemetry.batt_voltage = raw_sensors.logicrail.volt;
	telemetry.batt_percent= raw_sensors.logicrail.percent;

	telemetry.system_status = system.systemstatus.getStatus();


	system.networkmanager.sendPacket(telemetry);

}

void Commands::FreeRamCommand(System& sm, const RnpPacketSerialized& packet)
{	

	SimpleCommandPacket commandpacket(packet);

	uint32_t freeram = esp_get_free_heap_size();

	if (commandpacket.arg == 0){
	MessagePacket_Base<0,static_cast<uint8_t>(decltype(System::commandhandler)::PACKET_TYPES::MESSAGE_RESPONSE)> message("FreeRam: " + std::to_string(esp_get_free_heap_size()));
		message.header.source_service = sm.commandhandler.getServiceID(); 
		message.header.destination_service = packet.header.source_service;
		message.header.source = packet.header.destination;
		message.header.destination = packet.header.source;
		message.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(message);
	}
	else if (commandpacket.arg == 1)
	{
		BasicDataPacket<uint32_t,0,105> responsePacket(freeram);
		responsePacket.header.source_service = sm.commandhandler.getServiceID(); 
		responsePacket.header.destination_service = packet.header.source_service;
		responsePacket.header.source = packet.header.destination;
		responsePacket.header.destination = packet.header.source;
		responsePacket.header.uid = packet.header.uid;
		sm.networkmanager.sendPacket(responsePacket);	
	}
	
}

void Commands::EnterMSCCommand(System& system, const RnpPacketSerialized& packet) 
{
	system.statemachine.changeState(std::make_unique<MSC>(system.systemstatus,system.commandhandler));
}