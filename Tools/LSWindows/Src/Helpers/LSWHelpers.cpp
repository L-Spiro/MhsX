#include "LSWHelpers.h"
#include "../Tab/LSWTab.h"
#include "../Tab/LSWTabPageBase.h"
#include "../Widget/LSWWidget.h"

#ifdef LSW_USB_INPUTS
#pragma comment( lib, "Hid.lib" )
#endif	// #ifdef LSW_USB_INPUTS

namespace lsw {

	// == Members.
#ifdef LSW_USB_INPUTS
	/** GUID for all USB serial host PnP drivers. */
	const GUID LSW_HDEVNOTIFY::s_gUsbPnPDevices = { 0x25DBCE51, 0x6C8F, 0x4A72,
		0x8A,0x6D,0xB5,0x4C,0x2B,0x4F,0xC8,0x35 };
#endif	// #ifdef LSW_USB_INPUTS

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
			if ( hHandle.bOpened ) {
				wchar_t wcName[128] = { 0 };
#ifdef LSW_USB_INPUTS
				BOOLEAN bResult = ::HidD_GetProductString( hHandle.hHandle, wcName, sizeof( wchar_t ) * 126 );
#else
				BOOLEAN bResult = FALSE;
#endif	// #ifdef LSW_USB_INPUTS
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
		UINT uiSize = 0;
		if ( ::GetRawInputData( _hRawInput, RID_INPUT, NULL, &uiSize, sizeof( RAWINPUTHEADER ) ) ) {
			return vRaw;
		}
		vRaw.resize( uiSize );
		if ( UINT( -1 ) == ::GetRawInputData( _hRawInput, RID_INPUT, vRaw.data(), &uiSize, sizeof( RAWINPUTHEADER ) ) ) {
			return std::vector<uint8_t>();
		}
		return vRaw;
	}

	/**
	 * Takes an array of CTabPageBase's and fits them into a CTab control, additionally resizing the parent window by the same amount as the tab control is resized.  The tabs are named according to the return value of each CTabPageBase's GetName() method.
	 * 
	 * \param _pptpbPages The pages to insert into the tab control.
	 * \param _stTotal The number of pages to which to _pptpbPages points.
	 * \param _pwTab The tab control into which to insert the pages.
	 * \param _pwParent The parent to resize along with the tab control.
	 * \param _bFitWindow If TRUE, the window is resized to fit the tab control snuggly, otherwise it is reized by the same amount as the tab control, which leaves any extra space around the tab control as-is.
	 **/
	void CHelpers::FillTabAndFitWindow( CTabPageBase ** _pptpbPages, size_t _stTotal, CWidget * _pwTab, CWidget * _pwParent, bool _bFitWindow ) {
		if ( !_stTotal || !_pwTab || !_pptpbPages ) { return; }

		auto ptTab = reinterpret_cast<CTab *>(_pwTab);
		TCITEMW tciItem = { 0 };
		tciItem.mask = TCIF_TEXT;
		LSW_RECT rRect;
		rRect.PrepareConsume();

		for ( size_t I = 0; I < _stTotal; ++I ) {
			auto rThis = _pptpbPages[I]->WindowRect();			// Get the full rectangle.
			rThis.MoveBy( -rThis.left, -rThis.top );			// Move to [0,0].
			rRect.Consume( rThis );								// Get the max width/height of all the items being put into the tab.

			auto wsText = _pptpbPages[I]->GetName();
			tciItem.pszText = const_cast<LPWSTR>(wsText.c_str());
			/*int iIdx = */ptTab->InsertItem( int( I ), &tciItem, _pptpbPages[I] );
		}

		// rRect is the smallest box that will contain all items in the tab control.
		LSW_RECT rNewTabRect = rRect;
		ptTab->AdjustRect( TRUE, &rNewTabRect );
		LSW_RECT rTabWindow = ptTab->WindowRect();
		LSW_RECT rTabClient = rTabWindow.ScreenToClient( ptTab->Wnd() );

		// Move the CTab.
		::MoveWindow( ptTab->Wnd(), rTabClient.left, rTabClient.top, rNewTabRect.Width(), rNewTabRect.Height(), FALSE );
		rRect = ptTab->WindowRect().ScreenToClient( ptTab->Wnd() );
		rTabClient = rRect;
		ptTab->AdjustRect( FALSE, &rRect );

		// Resize the pages.
		for ( size_t I = 0; I < _stTotal; ++I ) {
			::MoveWindow( _pptpbPages[I]->Wnd(), rRect.left, rRect.top, rRect.Width(), rRect.Height(), FALSE );
		}

		// Resize the parent.
		if ( _pwParent ) {
			LSW_RECT rWindowRect = _pwParent->WindowRect();
			if ( _bFitWindow ) {
				LSW_RECT rClientRect;
				rClientRect.PrepareConsume();
				rClientRect.Consume( rTabClient );
				::AdjustWindowRectEx( &rClientRect, _pwParent->GetStyle(), _pwParent->HasMenu(), _pwParent->GetStyleEx() );
				::MoveWindow( _pwParent->Wnd(), rWindowRect.left, rWindowRect.top, rClientRect.Width(), rClientRect.Height(), FALSE );
			}
			else {
				rWindowRect.right += rTabClient.Width() - rTabWindow.Width();
				rWindowRect.bottom += rTabClient.Height() - rTabWindow.Height();
				::MoveWindow( _pwParent->Wnd(), rWindowRect.left, rWindowRect.top, rWindowRect.Width(), rWindowRect.Height(), FALSE );
			}
		}
	}

}	// namespace lsw
