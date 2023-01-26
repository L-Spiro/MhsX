#include "LSWHelpers.h"

#pragma comment(lib, "Hid.lib")

namespace lsw {

	// == Members.
	#include "LSWMessages.h"		// m_mMessages[].

	/**
	 * Converts a message to a string.
	 *
	 * \param _wMessage The Windows message to turn into a string.
	 * \param _sRet Returned by the function after being filled with a text string containing the given Windows message.
	 * \param _bOnlyFirst Returns only the first identifier when multiple exist.
	 * \return The returned string.
	 */
	std::string & CHelpers::WindowsMessageToString( WORD _wMessage, std::string &_sRet, bool _bOnlyFirst ) {
		static_cast<void>(_wMessage);
		static_cast<void>(_bOnlyFirst);
#ifdef _DEBUG
		bool bFound = false;
		for ( size_t I = 0; I < sizeof( m_mMessages ) / sizeof( m_mMessages[0] ); ++I ) {
			if ( m_mMessages[I].uiId == _wMessage ) {
				if ( bFound ) {
					_sRet += ", ";
				}
				bFound =  true;
				_sRet += m_mMessages[I].pcName;
				if ( !_bOnlyFirst ) { break; }
			}
		}
		if ( !bFound ) {
			char szBuffer[32];
			std::sprintf( szBuffer, "%.4X", _wMessage );
			_sRet += szBuffer;
		}
#endif	// #ifdef _DEBUG
		return _sRet;
	}

	/**
	 * Gets a list of input devices.
	 *
	 * \return Returns a vector of input devices.
	 */
	std::vector<RAWINPUTDEVICELIST> CHelpers::GetRawInputDeviceList() {
		UINT uiTotal = 1;
		UINT uiRet = ::GetRawInputDeviceList( NULL, &uiTotal, sizeof( RAWINPUTDEVICELIST ) );
		if ( UINT( -1 ) == uiRet ) {
			CBase::PrintError( L"Error Reading Input Devices." );
			return std::vector<RAWINPUTDEVICELIST>();
		}
		std::vector<RAWINPUTDEVICELIST> vList;
		vList.resize( uiTotal );
		uiRet = ::GetRawInputDeviceList( vList.data(), &uiTotal, sizeof( RAWINPUTDEVICELIST ) );
		if ( UINT( -1 ) == uiRet ) {
			CBase::PrintError( L"Error Gathering Input Devices." );
			return std::vector<RAWINPUTDEVICELIST>();
		}
		return vList;
	}

	/**
	 * Gets an input device's name.
	 *
	 * \param _hHandle The handle to the device.
	 * \param _wsIdentString Contains the identifier string for the device.
	 * \return Returns a wstring containing the name of the device.
	 */
	std::wstring CHelpers::GetRawInputDeviceName( HANDLE _hHandle, std::wstring &_wsIdentString ) {
		UINT uiSize = 0;
		UINT uiThis = ::GetRawInputDeviceInfoW( _hHandle,
			RIDI_DEVICENAME, NULL, &uiSize );
		if ( UINT( -1 ) == uiThis ) {
			CBase::PrintError( L"Error Gathering Input Device Name." );
			return std::wstring();
		}
		if ( uiSize ) {
			std::wstring wsRet;
			_wsIdentString.resize( uiSize + 1 );
			::GetRawInputDeviceInfoW( _hHandle,
				RIDI_DEVICENAME, _wsIdentString.data(), &uiSize );

			LSW_HID_HANLE hHandle( _wsIdentString.c_str() );
			if ( hHandle.m_bOpened ) {
				wchar_t wcName[128] = { 0 };
				BOOLEAN bResult = ::HidD_GetProductString( hHandle.hHandle, wcName, sizeof( wchar_t ) * 126 );
				if ( bResult ) {
					wsRet = wcName;
				}
			}
			return wsRet;
		}
		return std::wstring();
	}

	/**
	 * Gets an input device's information.
	 *
	 * \param _hHandle The handle to the device.
	 * \return Returns the device's information.
	 */
	RID_DEVICE_INFO CHelpers::GetRawInputDeviceInformation( HANDLE _hHandle ) {
		UINT uiSize = sizeof( RID_DEVICE_INFO );
		RID_DEVICE_INFO diInfo = { .cbSize = uiSize };
		UINT uiThis = ::GetRawInputDeviceInfoW( _hHandle,
			RIDI_DEVICEINFO, &diInfo, &uiSize );
		if ( UINT( -1 ) == uiThis ) {
			CBase::PrintError( L"Error Gathering Input Device Information." );
		}
		return diInfo;
	}

	/**
	 * Gets an input device's preparsed data.
	 *
	 * \param _hHandle The handle to the device.
	 * \return Returns the device's preparsed data.
	 */
	std::vector<uint8_t> CHelpers::GetRawInputDevicePreparsedData( HANDLE _hHandle ) {
		std::vector<uint8_t> vRet;
		UINT uiSize;
		//PHIDP_PREPARSED_DATA pdData = nullptr;
		UINT uiThis = ::GetRawInputDeviceInfoW( _hHandle,
			RIDI_PREPARSEDDATA, NULL, &uiSize );
		if ( UINT( -1 ) == uiThis ) {
			CBase::PrintError( L"Error Gathering Input Device Preparsed Data." );
			return vRet;
		}
		vRet.resize( uiSize );
		::GetRawInputDeviceInfoW( _hHandle,
			RIDI_PREPARSEDDATA, vRet.data(), &uiSize );
		
		return vRet;
	}

	/**
	 * Gathers all of the raw nput devices of a given type into an array.
	 *
	 * \param _dwType The type of device to gather.
	 * \return Returns a vector containing the raw input devices of the given type.
	 */
	std::vector<LSW_RAW_INPUT_DEVICE_LIST> CHelpers::GatherRawInputDevices( DWORD /*_dwType*/ ) {
		std::vector<RAWINPUTDEVICELIST> vList = CHelpers::GetRawInputDeviceList();
		std::vector<LSW_RAW_INPUT_DEVICE_LIST> vRet;
		for ( size_t I = 0; I < vList.size(); ++I ) {
			if ( RIM_TYPEHID == vList[I].dwType ) {
				LSW_RAW_INPUT_DEVICE_LIST ridlList;
				ridlList.hDevice = vList[I].hDevice;
				ridlList.wsName = CHelpers::GetRawInputDeviceName( vList[I].hDevice, ridlList.wsIdent );
				if ( ridlList.wsName.size() == 0 ) {
					CBase::PrintError( L"Error Gathering Input Device Name." );
					return vRet;
				}

				ridlList.diInfo = CHelpers::GetRawInputDeviceInformation( vList[I].hDevice );
				ridlList.vPreparsedData = CHelpers::GetRawInputDevicePreparsedData( vList[I].hDevice );
				
				vRet.push_back( ridlList );
			}
		}
		return vRet;
	}

	/**
	 * Registers raw input devices.
	 *
	 * \param _pridDevices An array of devices to register.
	 * \param _uiNumDevices The number of devices to which _uiNumDevices points.
	 * \return Returns TRUE if registration succeeded.
	 */
	bool CHelpers::RegisterRawInputDevices( const RAWINPUTDEVICE * _pridDevices, UINT _uiNumDevices ) {
		if ( ::RegisterRawInputDevices( _pridDevices, _uiNumDevices, sizeof( RAWINPUTDEVICE ) ) == FALSE ) {
			return false;
		}
		return true;
	}

	/**
	 * Gets the raw input data from a WM_INPUT message.
	 *
	 * \param _hRawInput The handle provided by WM_INPUT.
	 * \return Returns an array of the gathered input data.
	 */
	std::vector<uint8_t> CHelpers::GetRawInputData_Input( HRAWINPUT _hRawInput ) {
		std::vector<uint8_t> vRaw;
		UINT uiSize;
		if ( ::GetRawInputData( _hRawInput, RID_INPUT, NULL, &uiSize, sizeof( RAWINPUTHEADER ) ) ) {
			return vRaw;
		}
		vRaw.resize( uiSize );
		if ( UINT( -1 ) == ::GetRawInputData( _hRawInput, RID_INPUT, vRaw.data(), &uiSize, sizeof( RAWINPUTHEADER ) ) ) {
			return std::vector<uint8_t>();
		}
		return vRaw;
	}

}	// namespace lsw
