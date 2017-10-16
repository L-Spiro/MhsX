#include "MXLayoutManager.h"
#include "../Layouts/MXLayoutMacros.h"
#include "../MainWindow/MXMhsMainWindow.h"
#include "../OpenProcess/MXOpenProcessWindow.h"
#include "../Options/MXOptionsPageGeneral.h"
#include "../Options/MXOptionsWindow.h"

namespace mx {

	// == Functions.
	// Creates a class based on its type.
	CWidget * CLayoutManager::CreateWidget( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget, HMENU _hMenu ) {
		switch ( _wlLayout.ltType ) {
			case MX_MAIN_WINDOW : {
				return new CMhsMainWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu );
			}
			case MX_OPEN_PROCESS_WINDOW : {
				return new COpenProcessWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu );
			}
			case MX_OPTIONS_WINDOW : {
				return new COptionsWindow( _wlLayout, _pwParent,  _bCreateWidget, _hMenu );
			}
			case MX_OPTIONS_GENERAL : {
				return new COptionsPageGeneral( _wlLayout, _pwParent,  _bCreateWidget, _hMenu );
			}
		}
		return lsw::CLayoutManager::CreateWidget( _wlLayout, _pwParent, _bCreateWidget, _hMenu );
	}

	// Creates a copy of the given array of LSW_WIDGET_LAYOUT objects with LSW_WIDGET_LAYOUT::pwcText and the expressions.
	void CLayoutManager::UnencryptLayouts( const LSW_WIDGET_LAYOUT * _pwlLayouts, size_t _sTotal,
		std::vector<LSW_WIDGET_LAYOUT> &_vArray,
		std::vector<std::wstring> &_vWStrings,
		std::vector<std::string> &_vStrings ) {
		_vStrings.resize( _sTotal * 6 );	// Each control can have up to 6 normal strings.
		_vWStrings.resize( _sTotal * 1 );
		size_t sStringIndex = 0;
		size_t sStringWIndex = 0;
		for ( size_t I = 0; I < _sTotal; ++I ) {
			LSW_WIDGET_LAYOUT wlTemp = _pwlLayouts[I];
			if ( wlTemp.pwcText ) {
				std::string sTemp;
				for ( size_t J = 0; J < wlTemp.sTextLen; ++J ) {
					sTemp.push_back( static_cast<char>(wlTemp.pwcText[J]) );
				}
				_vWStrings[sStringWIndex] = mx::CStringDecoder::DecodeToWString( sTemp.c_str(), wlTemp.sTextLen );
				wlTemp.pwcText = _vWStrings[sStringWIndex++].c_str();
			}

			if ( wlTemp.pcLeftSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcLeftSizeExp, wlTemp.sLeftSizeExpLen );
				wlTemp.pcLeftSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcRightSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcRightSizeExp, wlTemp.sRightSizeExpLen );
				wlTemp.pcRightSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcTopSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcTopSizeExp, wlTemp.sTopSizeExpLen );
				wlTemp.pcTopSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcBottomSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcBottomSizeExp, wlTemp.sBottomSizeExpLen );
				wlTemp.pcBottomSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcWidthSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcWidthSizeExp, wlTemp.sWidthSizeExpLen );
				wlTemp.pcWidthSizeExp = _vStrings[sStringIndex++].c_str();
			}
			if ( wlTemp.pcHeightSizeExp ) {
				_vStrings[sStringIndex] = mx::CStringDecoder::DecodeToString( wlTemp.pcHeightSizeExp, wlTemp.sHeightSizeExpLen );
				wlTemp.pcHeightSizeExp = _vStrings[sStringIndex++].c_str();
			}
			_vArray.push_back( wlTemp );
		}
	}

	// Removes decrypted strings from an array of LSW_WIDGET_LAYOUT objects.
	void CLayoutManager::CleanEncryptedStrings( std::vector<std::wstring> &_vWStrings,
		std::vector<std::string> &_vStrings ) {
		for ( size_t I = 0; I < _vWStrings.size(); ++I ) {
			for ( size_t J = 0; J < _vWStrings[I].size(); ++J ) {
				_vWStrings[I][J] = L'\0';
			}
		}
		_vWStrings.clear();

		for ( size_t I = 0; I < _vStrings.size(); ++I ) {
			for ( size_t J = 0; J < _vStrings[I].size(); ++J ) {
				_vStrings[I][J] = '\0';
			}
		}
		_vStrings.clear();
	}

}	// namespace mx
