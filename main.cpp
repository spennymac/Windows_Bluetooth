#include "bt_discovery.h"

#include <iostream>
#include <array>

/*
BOOL callb(LPVOID pvParam, PBLUETOOTH_AUTHENTICATION_CALLBACK_PARAMS pDevice)
{
	std::cout << "device authentication callbac " << std::endl;
	BLUETOOTH_AUTHENTICATE_RESPONSE resp;
	resp.authMethod = pDevice->authenticationMethod;
	resp.bthAddressRemote = pDevice->deviceInfo.Address;
	resp.negativeResponse = FALSE;
	BluetoothSendAuthenticationResponseEx(NULL, &resp);
 return 1;
};
*/


int main()
{

	/*
	{
	// Radio Info
	BLUETOOTH_RADIO_INFO rInfo;
	rInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);
	BLUETOOTH_FIND_RADIO_PARAMS frp;
	frp.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

	HANDLE radio;
	if (BluetoothFindFirstRadio(&frp, &radio) == NULL)
	{
	std::cout << "BluetoothFindFirstRadio() is null" << std::endl;
	std::cout << GetLastError() << std::endl;
	}
	else
	{
	BluetoothGetRadioInfo(radio, &rInfo);
	std::wcout << rInfo.szName << std::endl;
	std::cout << rInfo.ulClassofDevice << std::endl;
	std::cout << rInfo.manufacturer << std::endl;
	}

	// Device Info
	BLUETOOTH_DEVICE_SEARCH_PARAMS pbtsp;
	pbtsp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	pbtsp.fReturnAuthenticated = true;
	pbtsp.fReturnRemembered = true;
	pbtsp.fReturnUnknown = true;
	pbtsp.fReturnConnected = true;
	pbtsp.fIssueInquiry = true;
	pbtsp.cTimeoutMultiplier = 10;
	pbtsp.hRadio = NULL;

	BLUETOOTH_DEVICE_INFO          pbtdi;
	pbtdi.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

	//Register for Authencatio callback
	HBLUETOOTH_AUTHENTICATION_REGISTRATION authHandle;
	PFN_AUTHENTICATION_CALLBACK_EX cb = (PFN_AUTHENTICATION_CALLBACK_EX)&callb;
	BluetoothRegisterForAuthenticationEx(NULL,&authHandle, cb, NULL);

	HBLUETOOTH_DEVICE_FIND hdevice = BluetoothFindFirstDevice(&pbtsp,&pbtdi);
	if (hdevice == NULL)
	{
	std::cout << "device handle null" << std::endl;
	std::cout << GetLastError() << std::endl;
	}
	else
	{
	std::wcout <<pbtdi.szName <<std::endl;
	std::cout << "Connected: " << pbtdi.fConnected <<std::endl;
	std::cout << "CLass: " << pbtdi.ulClassofDevice <<std::endl;
	std::cout << "AuthenticateDevice Error: " << BluetoothAuthenticateDeviceEx(NULL, NULL, &pbtdi, NULL, AUTHENTICATION_REQUIREMENTS::MITMProtectionNotRequired) << std::endl;
	}
	}
	*/


	/*
	for (auto &a : radios)
	{
		auto x = bluetooth::GetRadioInfo(*a);
		//bluetooth::PrintRadioInfo(x);
		//std::cout << bluetooth::EnableDiscovery(*a);
		//std::cout << bluetooth::EnableIncomingConnections(*a, true) << std::endl;
	}

	std::cout << "Scanning for nearby devices\n";
	auto devices = bluetooth::ScanForNearbyDevices(*radios.at(0), 10);
	for (auto &a : devices)
	{
		std::wcout << a.szName << '\n';
	}
	
	std::cout << "\n\nGetting authenticated devices\n";
	auto authDevices = bluetooth::GetAuthenticatedDevices(*radios.at(0));
	for (auto &a : authDevices)
	{
		std::wcout << a.szName << '\n';
		auto services = bluetooth::ServiceDiscovery(*radios.at(0), a);

		for (auto &guid : services)
		{
			std::cout << guid.Data1 << '\n';
			std::cout << guid.Data2 << '\n';
			std::cout << guid.Data3 << '\n';
			std::cout << guid.Data4 << '\n';

			auto y =  bluetooth::GetUUID16(guid) ;
				std::cout <<y<< std::endl;
		}
	}

	std::cout << "\n\nGetting connected devices\n";
	auto connectedDevices = bluetooth::GetConnectedDevices(*radios.at(0));
	for (auto &a : connectedDevices)
	{
		std::wcout << a.szName << '\n';
		auto services = bluetooth::ServiceDiscovery(*radios.at(0), a);
	}

	std::cout << "\n\nGetting remembered devices\n";
	auto remeberedDevices = bluetooth::GetRememberedDevices(*radios.at(0));
	for (auto &a : remeberedDevices)
	{
		std::wcout << a.szName << '\n';
		auto services = bluetooth::ServiceDiscovery(*radios.at(0), a);

		std::cout << "bluetooth connect "<< bluetooth::Connect(remeberedDevices.front());
		for (auto &guid : services)
		{
			std::cout << guid.Data1 << '\n';
			std::cout << guid.Data2 << '\n';
			std::cout << guid.Data3 << '\n';
			std::cout << guid.Data4 << '\n';
		}
	}

	std::cout << "\n\nGetting unknown devices\n";
	auto unknownDevices = bluetooth::GetUnknownDevices(*radios.at(0));
	for (auto &a : unknownDevices)
	{
		std::wcout << a.szName << '\n';
		auto services = bluetooth::ServiceDiscovery(*radios.at(0), a);

		for (auto &guid : services)
		{
			std::cout << guid.Data1 << '\n';
			std::cout << guid.Data2 << '\n';
			std::cout << guid.Data3 << '\n';
			std::cout << guid.Data4 << '\n';
		}
	}

	std::cout << "\n\nGetting Services From Devices\n";
	for (auto &a : unknownDevices)
	{
		auto services = bluetooth::ServiceDiscovery(*radios.at(0), a);

		for (auto &guid : services)
		{
			std::cout << guid.Data1 <<'\n';
			std::cout << guid.Data2 <<'\n';
			std::cout << guid.Data3 <<'\n';
			std::cout << guid.Data4 <<'\n';
		}
	}

	*/
	int option; 
	auto radios = bluetooth::FindRadios();
	auto radio = std::move(radios.at(0));
	std::vector<BLUETOOTH_DEVICE_INFO> devices;

	do 
	{
		std::cout << "\n\n1) Scan Nearby Devices " << std::endl;
		std::cout << "2) List authenticated devices" << std::endl;
		std::cout << "3) List connected devices" << std::endl;
		std::cout << "4) List remembered devices" << std::endl;
		std::cout << "5) List unknown devices" << std::endl;
		std::cout << "6) List services on last device query " << std::endl;
		std::cout << "7) Print Radio Info" << std::endl;
		std::cout << "8) Set Service" << std::endl;
		std::cout << "9) Listen As SErver" << std::endl;
		std::cout << "10) Connect" << std::endl;
		std::cout << "11) Exit Program " << std::endl;
		std::cout << "Please select an option : ";
		std::cin >> option;  
		std::cout << std::endl;

		if (option == 1) 
		{
			devices = bluetooth::ScanForNearbyDevices(*radio, 10);
			for (auto &a : devices)
			{
				std::wcout << a.szName << '\n';
			}
		}
		else if (option == 2) 
		{
			devices = bluetooth::GetAuthenticatedDevices(*radio);
			for (auto &a : devices)
			{
				std::wcout << a.szName << '\n';
			}
		}
		else if (option == 3) 
		{
			devices = bluetooth::GetConnectedDevices(*radio);
			for (auto &a : devices)
			{
				std::wcout << a.szName << '\n';
			}
		}
		else if (option == 4)
		{
			devices = bluetooth::GetRememberedDevices(*radio);
			for (auto &a : devices)
			{
				std::wcout << a.szName << '\n';
			}
		}
		else if (option == 5)
		{
			devices = bluetooth::GetUnknownDevices(*radio);
			for (auto &a : devices)
			{
				std::wcout << a.szName << '\n';
			}
		}
		else if (option == 6)
		{
			for (auto &a : devices)
			{
				auto services = bluetooth::ServiceDiscovery(*radio, a);
				std::wcout << a.szName << '\n';
				for (auto &guid : services)
				{
					std::cout << std::hex << bluetooth::GetUUID16(guid) << '\n';
					bluetooth::GetServicePort(a, guid);
					/* if (bluetooth::GetUUID16(guid) == bluetooth::UUID16_SERVICES.at(bluetooth::Services::Headset))
					{
						std::cout << "Found Headset Profile\n";
						std::cout << "Attempting to connect...";
						std::cout << bluetooth::Connect(a, guid);
						break;
					} */
				}
			}
		}
		else if (option == 7)
		{
			bluetooth::PrintRadioInfo(bluetooth::GetRadioInfo(*radio));
		}
		else if (option == 8)
		{
			bluetooth::SetLocalService(*radio);
		}
		else if (option == 9)
		{
			bluetooth::RegisterServiceAndListen();
		}
		else if (option == 10)
		{
			for (auto &a : devices)
			{
				auto services = bluetooth::ServiceDiscovery(*radio, a);
				std::wcout << a.szName << '\n';
				for (auto &guid : services)
				{
					if (bluetooth::GetUUID16(guid) == bluetooth::UUID16_SERVICES.at(bluetooth::Services::Headset))
					{
						std::cout << "Found Headset Profile\n";
						std::cout << "Attempting to connect...";
						std::cout << bluetooth::Connect(a, guid);
						break;
					} 
				}
			}
		}
	} while (option != 11);  

	return 0;
}
