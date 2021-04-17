#include "MXLayoutManager.h"
#include "../Converter/MXConverterWindow.h"
#include "../ExpEval/MXExpEvalWindow.h"
#include "../FloatingPointStudio/MXFloatingPointStudioWindow.h"
#include "../FoundAddresses/MXFoundAddressesListView.h"
#include "../FoundAddresses/MXFoundAddressesWindow.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../OpenProcess/MXOpenProcessWindow.h"
#include "../Options/MXOptionsPageHotkeys.h"
#include "../Options/MXOptionsPageGeneral.h"
#include "../Options/MXOptionsPageGenSearch.h"
#include "../Options/MXOptionsPageOpenProc.h"
#include "../Options/MXOptionsPageSearchEx.h"
#include "../Options/MXOptionsWindow.h"
#include "../PE/MXPeWorksWindow.h"
#include "../Search/MXNewDataTypeSearchWindow.h"
#include "../Search/MXNewExpressionSearchWindow.h"
#include "../Search/MXNewPointerSearchWindow.h"
#include "../Search/MXNewStringSearchWindow.h"
#include "../Search/MXSearchProgressWindow.h"
#include "../Search/MXStandardSubsearchWindow.h"
#include "../StringTheory/MXStringTheoryWindow.h"


namespace mx {

	// == Functions.
	// Creates a class based on its type.
	CWidget * CLayoutManager::CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu, uint64_t _ui64Data ) {
		switch ( _wlLayout.ltType ) {
			case MX_MAIN_WINDOW : {
				return new CMhsMainWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPEN_PROCESS_WINDOW : {
				return new COpenProcessWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_WINDOW : {
				return new COptionsWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_GENERAL : {
				return new COptionsPageGeneral( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_GENSEARCH : {
				return new COptionsPageGenSearch( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_SEARCHEX : {
				return new COptionsPageSearchEx( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_OPENPROC : {
				return new COptionsPageOpenProc( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_OPTIONS_HOTKEYS : {
				return new COptionsPageHotkeys( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_FOUND_ADDRESSES : {
				return new CFoundAddressesWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_FOUND_ADDRESSES_LISTVIEW : {
				return new CFoundAddressesListView( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_EXP_EVAL : {
				return new CExpEvalWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_CONVERTER : {
				return new CConverterWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_NEW_DATA_TYPE_SEARCH : {
				return new CNewDataTypeSearchWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_NEW_POINTER_SEARCH : {
				return new CNewPointerSearchWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_NEW_STRING_SEARCH : {
				return new CNewStringSearchWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_NEW_EXPRESSION_TYPE_SEARCH : {
				return new CNewExpressionSearchWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_STANDARD_SUBSEARCH : {
				return new CStandardSubsearchWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_SEARCH_PROGRESS : {
				return new CSearchProgressWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_PE_WORKS : {
				return new CPeWorksWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_STRING_THEORY : {
				return new CStringTheoryWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
			case MX_FLOATING_POINT_STUDIO : {
				return new CFloatingPointStudioWindow( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
			}
		}
		return lsw::CLayoutManager::CreateWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu, _ui64Data );
	}

	// Creates a pop-up menu.
	bool CLayoutManager::CreatePopupMenuEx( CWidget * _pwParent, const LSW_MENU_LAYOUT * _pmlLayout, size_t _sTotal,
		INT _iX, INT _iY ) {
		std::vector<LSW_MENU_LAYOUT> vMenus;
		std::vector<std::vector<LSW_MENU_ITEM> *> vMenuItems;
		std::vector<std::vector<CSecureWString> *> vMenuStrings;
		CLayoutManager::UnencryptMenu( _pmlLayout, _sTotal,
			vMenus,
			vMenuItems,
			vMenuStrings );

		HMENU hMenu = CreatePopupMenu( &vMenus[0], vMenus.size() );
		if ( !hMenu ) { return false; }

		if ( _iX == -1 && _iY == -1 ) {
			POINT pPos;
			::GetCursorPos( &pPos );
			_iX = pPos.x;
			_iY = pPos.y;
		}
		::TrackPopupMenu( hMenu,
			TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			_iX, _iY, 0, _pwParent ? _pwParent->Wnd() : NULL, NULL );

		CLayoutManager::CleanEncryptedMenu( vMenuItems, vMenuStrings );

		return true;
	}

	// Creates a copy of the given array of LSW_WIDGET_LAYOUT objects with LSW_WIDGET_LAYOUT::pwcText and the expressions.
	void CLayoutManager::UnencryptLayouts( const LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal,
		std::vector<LSW_WIDGET_LAYOUT> &_vArray,
		std::vector<CSecureWString> &_vWStrings,
		std::vector<CSecureString> &_vStrings ) {
		_vStrings.resize( _sTotal * 6 );	// Each control can have up to 6 normal strings.
		_vWStrings.resize( _sTotal * 1 );
		size_t sStringIndex = 0;
		size_t sStringWIndex = 0;
		for ( size_t I = 0; I < _sTotal; ++I ) {
			LSW_WIDGET_LAYOUT wlTemp = _pwlLayouts[I];
			if ( wlTemp.pwcText && wlTemp.sTextLen ) {
				_vWStrings[sStringWIndex] = mx::CStringDecoder::DecodeToWString( reinterpret_cast<const char *>(wlTemp.pwcText), wlTemp.sTextLen );
				wlTemp.pwcText = _vWStrings[sStringWIndex++].c_str();
			}

			if ( wlTemp.pcLeftSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sLeftSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcLeftSizeExp, wlTemp.sLeftSizeExpLen ) : wlTemp.pcLeftSizeExp;
				wlTemp.pcLeftSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcRightSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sRightSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcRightSizeExp, wlTemp.sRightSizeExpLen ) : wlTemp.pcRightSizeExp;
				wlTemp.pcRightSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcTopSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sTopSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcTopSizeExp, wlTemp.sTopSizeExpLen ) : wlTemp.pcTopSizeExp;
				wlTemp.pcTopSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcBottomSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sBottomSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcBottomSizeExp, wlTemp.sBottomSizeExpLen ) : wlTemp.pcBottomSizeExp;
				wlTemp.pcBottomSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcWidthSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sWidthSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcWidthSizeExp, wlTemp.sWidthSizeExpLen ) : wlTemp.pcWidthSizeExp;
				wlTemp.pcWidthSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcHeightSizeExp ) {
				_vStrings[sStringIndex] = wlTemp.sHeightSizeExpLen ? mx::CStringDecoder::DecodeToString( wlTemp.pcHeightSizeExp, wlTemp.sHeightSizeExpLen ) : wlTemp.pcHeightSizeExp;
				wlTemp.pcHeightSizeExp = _vStrings[sStringIndex++].c_str();
			}
			_vArray.push_back( wlTemp );
		}
	}

	// Removes decrypted strings from an array of LSW_WIDGET_LAYOUT objects.
	void CLayoutManager::CleanEncryptedStrings( std::vector<CSecureWString> &_vWStrings,
		std::vector<CSecureString> &_vStrings ) {
		CleanEncryptedStrings( _vWStrings );
		CleanEncryptedStrings( _vStrings );
	}

	// Removes decrypted strings.
	void CLayoutManager::CleanEncryptedStrings( std::vector<CSecureWString> &_vWStrings ) {
#if 0	// Don't need to do this now that CSecureWString is used.
		for ( size_t I = 0; I < _vWStrings.size(); ++I ) {
			for ( size_t J = 0; J < _vWStrings[I].size(); ++J ) {
				_vWStrings[I][J] = L'\0';
			}
		}
#endif
		_vWStrings.clear();
	}

	// Removes decrypted strings.
	void CLayoutManager::CleanEncryptedStrings( std::vector<CSecureString> &_vStrings ) {
#if 0	// Don't need to do this now that CSecureString is used.
		for ( size_t I = 0; I < _vStrings.size(); ++I ) {
			for ( size_t J = 0; J < _vStrings[I].size(); ++J ) {
				_vStrings[I][J] = '\0';
			}
		}
#endif
		_vStrings.clear();
	}

	// Creates a copy of a given array of LSW_MENU_ITEM objects with converted strings.
	void CLayoutManager::UnencryptMenuItems( const LSW_MENU_ITEM * _pmiItems, size_t _sTotal,
		std::vector<LSW_MENU_ITEM> &_vArray,
		std::vector<CSecureWString> &_vWStrings ) {

		_vWStrings.resize( _sTotal * 1 );
		size_t sStrIdx = 0;
		for ( size_t I = 0; I < _sTotal; ++I ) {
			LSW_MENU_ITEM miItem = _pmiItems[I];

			if ( miItem.lpwcText && miItem.stTextLen ) {
				_vWStrings[sStrIdx] = mx::CStringDecoder::DecodeToWString( reinterpret_cast<const char *>(miItem.lpwcText), miItem.stTextLen );
				miItem.lpwcText = _vWStrings[sStrIdx++].c_str();
			}

			_vArray.push_back( miItem );
		}
	}

	// Removes decrypted strings from an array of LSW_MENU_LAYOUT objects.
	void CLayoutManager::UnencryptMenu( const LSW_MENU_LAYOUT * _pmlLayout, size_t _sTotal,
		std::vector<LSW_MENU_LAYOUT> &_vArray,
		std::vector<std::vector<LSW_MENU_ITEM> *> &_vMenuItems,
		std::vector<std::vector<CSecureWString> *> &_vWStrings ) {
		for ( size_t I = 0; I < _sTotal; ++I ) {
			LSW_MENU_LAYOUT mlCopy = _pmlLayout[I];
			std::vector<LSW_MENU_ITEM> * pvMenuItems = new( std::nothrow ) std::vector<LSW_MENU_ITEM>();
			if ( pvMenuItems ) {
				_vMenuItems.push_back( pvMenuItems );

				std::vector<CSecureWString> * pvMenuTexts = new( std::nothrow ) std::vector<CSecureWString>();
				if ( pvMenuTexts ) {
					_vWStrings.push_back( pvMenuTexts );
					UnencryptMenuItems( mlCopy.pmiItems, mlCopy.stTotalMenuItems,
						(*pvMenuItems),
						(*pvMenuTexts) );
					if ( (*pvMenuItems).size() ) {
						mlCopy.pmiItems = &(*pvMenuItems)[0];
					}
					else {
						mlCopy.pmiItems = nullptr;
					}


					_vArray.push_back( mlCopy );
				}
			}
		}
	}

	// Cleans up a decrypted menu.
	void CLayoutManager::CleanEncryptedMenu( std::vector<std::vector<LSW_MENU_ITEM> *> &_vMenuItems,
		std::vector<std::vector<CSecureWString> *> &_vWStrings ) {
		for ( size_t I = _vWStrings.size(); I--; ) {
			CleanEncryptedStrings( (*_vWStrings[I]) );
			delete _vWStrings[I];
			_vWStrings[I] = nullptr;
		}

		for ( size_t I = _vMenuItems.size(); I--; ) {
			delete _vMenuItems[I];
			_vMenuItems[I] = nullptr;
		}
	}

}	// namespace mx
