#pragma once
/*
To facilitate the discovery of Bluetooth devices and services,
Windows maps the Bluetooth Service Discovery Protocol(SDP) onto
the Windows Sockets namespace interfaces.The primary functions
used for this mapping are the WSASetService, WSALookupServiceBegin,
WSALookupServiceNext, and WSALookupServiceEnd functions.
The WSAQUERYSET structure is also used in conjunction with these functions.


CONVERTING UUIDS
	128_bit_value = 16_bit_value * 2^96 + Bluetooth_Base_UUID
	128_bit_value = 32_bit_value  2^96 + Bluetooth_Base_UUID
	A 16-bit UUID may be converted to 32-bit UUID format by zero-extending the
	16-bit value to 32-bits. An eq

	(128_bit_value - Bluetooth_Base_UUID) >> 96  = 16_bit_value

*/
#include <winsock2.h>
#include "Windows.h"
#include "Ws2bth.h"
#include "BluetoothAPIs.h"

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <array>

namespace bluetooth
{ 
	template<typename T>
	using deleted_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

	using uuid16 = unsigned short int;
	using uuid32 = unsigned int;

	static const GUID BLUETOOTH_BASE_UUID  = 
	{
		0x00000000,
		0x0000,
		0x1000,
		{
			0x80,
			0x00,
			0x00,
			0x80,
			0x5F,
			0x9B,
			0x34,
			0xFB
		}
	};

	static const GUID HeadSetProfile = 
	{
		0x00001108,
		0x0000,
		0x1000,
		{
			0x80,
			0x00,
			0x00,
			0x80,
			0x5F,
			0x9B,
			0x34,
			0xFB
		}
	};

	static enum class Services
	{
		Headset,
		Handsfree
	};

	static const  std::map<Services,uuid16 > UUID16_SERVICES
	{
		{ Services::Headset, 0x1108},
		{ Services::Handsfree , 0x111E}
	};

	static const uuid16 GetUUID16(GUID& guid) 
	{
		return static_cast<uuid16> (guid.Data1);
	}

	static const uuid32 GetUUID32(GUID& guid)
	{
		return static_cast<uuid32> (guid.Data1);
	}

	std::vector<deleted_unique_ptr<HANDLE>> FindRadios();

	bool IsRadioConnectable(const HANDLE radioHandle);

	bool IsRadioDiscoverable(const HANDLE radioHandle);

	BLUETOOTH_RADIO_INFO GetRadioInfo(const HANDLE radioHandle);

	void PrintRadioInfo(const BLUETOOTH_RADIO_INFO& info);

	bool EnableDiscovery(const HANDLE radioHandle, bool enabled = true);

	// Precondition: Bluetooth must be discoverable
	bool EnableIncomingConnections(const HANDLE radioHandle, bool enabled);

	//A value that indicates the time out for the inquiry, expressed in increments of 1.28 seconds.
	//For example, an inquiry of 12.8 seconds has a timeOutMultiplier value of 10. The maximum value 
	//for this member is 48.
	std::vector<BLUETOOTH_DEVICE_INFO> ScanForNearbyDevices(const HANDLE radioHandle, unsigned char timeOutMultiplier);

	std::vector<BLUETOOTH_DEVICE_INFO> GetConnectedDevices(const HANDLE radioHandle);

	std::vector<BLUETOOTH_DEVICE_INFO> GetAuthenticatedDevices(const HANDLE radioHandle);
	
	std::vector<BLUETOOTH_DEVICE_INFO> GetRememberedDevices(const HANDLE radioHandle);

	std::vector<BLUETOOTH_DEVICE_INFO> GetUnknownDevices(const HANDLE radioHandle);

	std::vector<GUID> ServiceDiscovery(const HANDLE radioHandle, const BLUETOOTH_DEVICE_INFO& deviceInfo);
	
	bool Connect(const BLUETOOTH_DEVICE_INFO& deviceInfo, GUID service); 
	bool RegisterServiceAndListen();
	void SetLocalService(const HANDLE radioHandle);
	std::vector<unsigned int> GetServicePort(const BLUETOOTH_DEVICE_INFO& deviceInfo,const GUID service);
};