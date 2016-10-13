#include "bt_discovery.h"


#include <ws2bth.h>
#include <iostream>
#include <iterator>
#include <array>
#include <algorithm>

DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);
						
std::vector<BLUETOOTH_DEVICE_INFO> QueryRadioForDevices(BLUETOOTH_DEVICE_SEARCH_PARAMS& searchParams);

std::vector<bluetooth::deleted_unique_ptr<HANDLE>> bluetooth::FindRadios()
{
	std::vector<deleted_unique_ptr<HANDLE>> radioContainer{};

	// Setup Neded
	BLUETOOTH_FIND_RADIO_PARAMS findRadioParams;
	findRadioParams.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);
	HANDLE radioHandle = nullptr;

	HBLUETOOTH_RADIO_FIND findHandle = BluetoothFindFirstRadio(&findRadioParams, &radioHandle);
	if (findHandle)
	{
		deleted_unique_ptr<HANDLE> ptr(new HANDLE, [](HANDLE* handle)
		{
			CloseHandle(*handle); 
			delete handle; 
		});
		*ptr = radioHandle;
		radioContainer.emplace_back(std::move(ptr));

		while (BluetoothFindNextRadio(findHandle, &radioHandle))
		{
			deleted_unique_ptr<HANDLE> ptr(new HANDLE, [](HANDLE* handle)
			{ 
				CloseHandle(*handle); 
				delete handle; 
			});
			*ptr = radioHandle;
			radioContainer.emplace_back(std::move(ptr));
		}
		return radioContainer;
	}
	else
	{
		std::cout << "BluetoothFindFirstRadio() is null" << std::endl;
		std::cout << "Windows Error: " << GetLastError() << std::endl;
		return{};
	}
}

void bluetooth::PrintRadioInfo(const BLUETOOTH_RADIO_INFO& info)
{
	std::wcout << "Name: "         << info.szName          << '\n';
	std::cout  << "Class: "		   << info.ulClassofDevice << '\n';
	std::cout  << "Manufacturer: " << info.manufacturer    << std::endl;
	std::cout  << "Address: "      << info.address.ullLong << std::endl;
}

BLUETOOTH_RADIO_INFO bluetooth::GetRadioInfo(const HANDLE radioHandle)
{
	BLUETOOTH_RADIO_INFO radioInfo{ };
	radioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

	if( BluetoothGetRadioInfo(radioHandle, &radioInfo) == ERROR_SUCCESS)
		return radioInfo;
	else
	{
		std::cout << "ERROR";

	}
	return {};
}

bool bluetooth::IsRadioConnectable(const HANDLE radioHandle)
{
	return static_cast<bool>(BluetoothIsConnectable(radioHandle));
}

bool bluetooth::IsRadioDiscoverable(const HANDLE radioHandle)
{
	return  static_cast<bool>(BluetoothIsDiscoverable(radioHandle));
}

bool bluetooth::EnableDiscovery(const HANDLE radioHandle, bool enabled)
{
	return static_cast<bool>(BluetoothEnableDiscovery(radioHandle, enabled));
}

bool bluetooth::EnableIncomingConnections(const HANDLE radioHandle, bool enabled)
{
	return static_cast<bool>(BluetoothEnableIncomingConnections(radioHandle, enabled));
}

std::vector<BLUETOOTH_DEVICE_INFO> bluetooth::ScanForNearbyDevices(const HANDLE radioHandle, unsigned char timeOutMultiplier)
{
	struct bounded
	{
		static const unsigned char get(unsigned char v)
		{
			if (v <= 48 && v >= 0)
			{ 
				return v;
			}
			else
			{
				return 48;
			}
		};
	};

	auto multiplier = bounded::get(timeOutMultiplier);
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams{};
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.fIssueInquiry = true;
	searchParams.cTimeoutMultiplier = multiplier;
	searchParams.hRadio = radioHandle;
	return QueryRadioForDevices(searchParams);
}

std::vector<BLUETOOTH_DEVICE_INFO> bluetooth::GetConnectedDevices(const HANDLE radioHandle)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams{};
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.fReturnConnected = true;
	searchParams.hRadio = radioHandle;
	return QueryRadioForDevices(searchParams);
}

std::vector<BLUETOOTH_DEVICE_INFO> bluetooth::GetAuthenticatedDevices(const HANDLE radioHandle)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams{};
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.fReturnAuthenticated = true;
	searchParams.hRadio = radioHandle;
	return QueryRadioForDevices(searchParams);
}

std::vector<BLUETOOTH_DEVICE_INFO> bluetooth::GetRememberedDevices(const HANDLE radioHandle)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams{};
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.fReturnRemembered = true;
	searchParams.hRadio = radioHandle;
	return QueryRadioForDevices(searchParams);
}

std::vector<BLUETOOTH_DEVICE_INFO> bluetooth::GetUnknownDevices(const HANDLE radioHandle)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams{};
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.fReturnUnknown = true;
	searchParams.hRadio = radioHandle;
	return QueryRadioForDevices(searchParams);
}

std::vector<BLUETOOTH_DEVICE_INFO> QueryRadioForDevices(BLUETOOTH_DEVICE_SEARCH_PARAMS& searchParams)
{
	std::vector<BLUETOOTH_DEVICE_INFO> deviceContainer{};

	BLUETOOTH_DEVICE_INFO deviceInfo;
	deviceInfo.dwSize = sizeof(BLUETOOTH_DEVICE_INFO);

	HBLUETOOTH_DEVICE_FIND findHandle = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
	if (findHandle)
	{
		deviceContainer.emplace_back(deviceInfo);
		while (BluetoothFindNextDevice(findHandle, &deviceInfo))
		{
			deviceContainer.emplace_back(deviceInfo);
		}
		BluetoothFindDeviceClose(findHandle);
		return deviceContainer;
	}
	else
	{
		auto err = GetLastError();
		if (err == ERROR_NO_MORE_ITEMS)
			std::cout << "No devices found\n";
		else
		{
			std::cout << "BluetoothFindFirstDevice() returned error or no devices\n";
			std::cout << "Windows Error: " << GetLastError() << '\n';
		}
		return{};
	}
}

std::vector<GUID> bluetooth::ServiceDiscovery(const HANDLE radioHandle, const BLUETOOTH_DEVICE_INFO& deviceInfo)
{
	static const auto serviceCapacity = 20;
	std::array<GUID, serviceCapacity> servicesArr;
	//On output, represents actual number of services returned
	auto dwordServiceCapacity = static_cast<DWORD>(serviceCapacity);

	auto err = BluetoothEnumerateInstalledServices(radioHandle, &deviceInfo, &dwordServiceCapacity, servicesArr.data());
	if (err == ERROR_SUCCESS || ERROR_MORE_DATA)
	{
		if (err == ERROR_MORE_DATA)
		{
			auto& actualNumGUIDs = dwordServiceCapacity;
			std::cout << "There is more data than capacity .... actual = " << actualNumGUIDs << '\n';
			std::vector<GUID> services(serviceCapacity);
			std::copy_n(servicesArr.begin(), serviceCapacity, services.begin());
			return services;
		}
		auto& actualNumGUIDs = dwordServiceCapacity;
		std::vector<GUID> services(actualNumGUIDs);
		std::copy_n(servicesArr.begin(), actualNumGUIDs, services.begin());
		return services;
	}
	else
	{
		std::cout << "BluetoothFindFirstDevice() returned error or no devices\n" << std::endl;
		std::cout << "Windows Error: " << GetLastError() << std::endl;
		return{};
	}
}

bool bluetooth::Connect(const BLUETOOTH_DEVICE_INFO& deviceInfo, GUID service)
{
	WSAData wsaData;
	auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	auto ports = bluetooth::GetServicePort(deviceInfo, service);
	if (ports.empty())
	{
		return false;
	}

	SOCKADDR_BTH  addr{};
	addr.addressFamily = AF_BTH;
	addr.btAddr = deviceInfo.Address.ullLong;
	addr.serviceClassId = service;
	addr.port = ports.at(0);
	auto sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if ( sock == INVALID_SOCKET) 
	{
		std::cout << "=CRITICAL= | Socket() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
    }

	if (SOCKET_ERROR == connect(sock, (struct sockaddr *) &addr, sizeof(SOCKADDR_BTH))) 
	{
		std::cout << "=CRITICAL= | connect() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false; 
	}
	char arr[100];
	while (true)
	{

		auto len = recv(sock, arr, 100, 0);
		if (len > 0)
		{
			for (auto i = 0; i < len; i++)
			{
				std::cout << arr[i] << '\n';
			}
				std::cout << '\n';
				std::cout << '\n';
		}
		else
		{
			std::cout << "recv returned len 0\n\n";
		}
	}
	char ar[10] = { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4 };
	auto sent = send(sock, ar, 10, 0);
	if (sent == 10)
	{
		std::cout << "Data Sent successfully\n";
	}
	else
	{
		std::cout << "Actual SEnt is: " << sent << std::endl;
	}
	return true;
}

bool bluetooth::RegisterServiceAndListen()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create Socket
	auto sock = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (sock == INVALID_SOCKET) 
	{
		std::cout << "=CRITICAL= | Socket() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
    }

	// Bind Socket
	SOCKADDR_BTH  addr{};
	addr.addressFamily = AF_BTH;
	addr.port = BT_PORT_ANY;
	auto err = bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(SOCKADDR_BTH));
	if (err == SOCKET_ERROR)
	{
		std::cout << "=CRITICAL= | bind() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
	}

	// Get Socket Info
	auto sockAddrBthSize = static_cast<int>(sizeof(SOCKADDR_BTH));
	err = getsockname(sock, reinterpret_cast<struct sockaddr *>(&addr), &sockAddrBthSize);
	if (SOCKET_ERROR == err) 
	{
		std::cout << "=CRITICAL= | getsockname() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Port: " << addr.port << '\n';

	//Set Service in SDP
	CSADDR_INFO cSAddrInfo{};
	cSAddrInfo.LocalAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	cSAddrInfo.LocalAddr.lpSockaddr = reinterpret_cast<struct sockaddr*>(&addr);
	cSAddrInfo.RemoteAddr.iSockaddrLength = sizeof(SOCKADDR_BTH);
	cSAddrInfo.RemoteAddr.lpSockaddr = reinterpret_cast<struct sockaddr*>(&addr);
	cSAddrInfo.iSocketType = SOCK_STREAM;
	cSAddrInfo.iProtocol = BTHPROTO_RFCOMM;

	GUID HeadSetProf =
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

	//wchar_t str[] = L"DERPDPER";
	WSAQUERYSET querySet{}; //https://msdn.microsoft.com/en-us/library/windows/desktop/aa362920(v=vs.85).aspx
	querySet.dwSize = sizeof(WSAQUERYSET);
	querySet.lpszServiceInstanceName = L"Spencer";
	querySet.lpServiceClassId = &HeadSetProf;
	querySet.lpszComment = L"Spencers crap";
	querySet.dwNameSpace = NS_BTH;
	querySet.dwNumberOfCsAddrs = 1;
	querySet.lpcsaBuffer = &cSAddrInfo; 

	err = WSASetService(&querySet, RNRSERVICE_REGISTER, 0);
	if (err) 
	{
		std::cout << "=CRITICAL= | WSASetSErvice() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
	}

	if (SOCKET_ERROR == listen(sock, 4)) 
	{ 
		std::cout << "=CRITICAL= | listen() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
	}

	auto client = accept(sock, NULL, NULL);
	if (INVALID_SOCKET == client) 
	{
		std::cout << "=CRITICAL= | accept() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return false;
	}


	return true;

}

void bluetooth::SetLocalService(const HANDLE radioHandle)
{
	/*
	WSAQUERYSET querySet{}; //https://msdn.microsoft.com/en-us/library/windows/desktop/aa362920(v=vs.85).aspx
	querySet.dwSize = sizeof(WSAQUERYSET);
	querySet.dwSize = sizeof(WSAQUERYSET);
	querySet.lpServiceClassId = &HeadSetProf;
	querySet.dwNameSpace = NS_BTH;
	querySet.dwNumberOfCsAddrs = 1;

.

	*/
	//
	// As long as we use a blocking accept(), we will have a race
	// between advertising the service and actually being ready to
	// accept connections.  If we use non-blocking accept, advertise
	// the service after accept has been called.
	//
	/*
	if (SOCKET_ERROR == WSASetService(&wsaQuerySet, RNRSERVICE_REGISTER, 0))
		
	auto err =	WSASetService(&querySet, RNRSERVICE_REGISTER, 0);

	if (err)
	{
		std::cout << "=CRITICAL= | WSASetSErvice() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
	}
	*/

}

std::vector<unsigned int> bluetooth::GetServicePort(const BLUETOOTH_DEVICE_INFO& deviceInfo, const GUID service)
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKADDR_BTH  addr{};
	addr.addressFamily = AF_BTH;
	addr.btAddr = deviceInfo.Address.ullLong;
	wchar_t buf[1024] = {};
	DWORD buflen  = sizeof(buf);
	auto err = WSAAddressToString(reinterpret_cast<LPSOCKADDR>(&addr), sizeof(addr), NULL, buf, &buflen);
	if (err == SOCKET_ERROR)
	{
		std::cout << "=CRITICAL= | WSAAddressToString() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return{};
	}

	WSAQUERYSET querySet{};
	DWORD qs_len = sizeof(WSAQUERYSET);
	querySet.dwSize = sizeof(WSAQUERYSET);
	auto guid = service;
	querySet.lpServiceClassId = &guid;
	querySet.dwNameSpace = NS_BTH;
	querySet.dwNumberOfCsAddrs = 0;
	querySet.lpszContext = buf;

	HANDLE lookUpHandle;
	if (SOCKET_ERROR == WSALookupServiceBegin(&querySet, LUP_FLUSHCACHE | LUP_RETURN_ADDR, &lookUpHandle)) 
	{
		std::cout << "=CRITICAL= | WSAALookupServiceBegin() call failed. WSAGetLastError= " << WSAGetLastError() << std::endl;
		return{};
	}

	std::unique_ptr<char[]> querySetChar(new char[sizeof(WSAQUERYSET) + 2000]);
	qs_len = sizeof(WSAQUERYSET) + 2000;
	auto qsNext = reinterpret_cast<WSAQUERYSET*>(querySetChar.get());

	std::vector<unsigned int> ports{};
	bool done= false;
	while (!done)
	{
		if (SOCKET_ERROR == WSALookupServiceNext(lookUpHandle,  LUP_FLUSHCACHE | LUP_RETURN_ADDR, &qs_len, qsNext))
		{
			auto err = WSAGetLastError();
			if (err == WSA_E_NO_MORE)
			{
				return ports;
			}
			else
			{
				std::cout << "=CRITICAL= | WSALookupServiceNext() call failed. WSAGetLastError= " << err << std::endl;
				return{};
			}
		}
		else
		{
			auto *sa = reinterpret_cast<SOCKADDR_BTH*>(qsNext->lpcsaBuffer->RemoteAddr.lpSockaddr);
			std::cout << "Port: " << sa->port << '\n';
			ports.emplace_back(sa->port);
		}
	}
	WSALookupServiceEnd(lookUpHandle);
	return ports;
}