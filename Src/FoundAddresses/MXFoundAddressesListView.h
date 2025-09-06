#pragma once

#include "../MXMhsX.h"

#include <ListView/LSWListView.h>


using namespace lsw;

namespace mx {
	
	class CFoundAddressesListView : public lsw::CListView {
	public :
		CFoundAddressesListView( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );


		// == Functions.
		// Requesting information (notification responder).
		virtual BOOL						GetDispInfoNotify( NMLVDISPINFOW * _plvdiInfo );

		// Gets the text for an address line.
		virtual bool						GetAddressText( std::wstring &_wsResult, int _iIndex ) const;

		// Gets the value text for a line.
		virtual bool						GetValueText( std::wstring &_wsResult, int _iIndex ) const;

		// Gets the other text for a line.
		virtual bool						GetOtherText( std::wstring &_wsResult, int _iIndex, int _iSub ) const;


	protected :
		// The main window.
		class CMhsMainWindow *				m_pmmwMhsWindow;


		// == Functions.
		// Gets the display text for a data-type search result.
		bool								GetDataTypeSearchValue( LPWSTR _pwStr, int _iMaxLen, uint32_t _ui32Type, const uint8_t * _pui8Value ) const;

		// Gets the display test for a string search result.
		bool								GetStringSearchValue( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const;

		// Gets the display test for an expression search result.
		bool								GetExpressionSearchValue( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const;

		// Gets the display test for an expression search result.
		bool								GetExpressionSearchValueFixed( LPWSTR _pwStr, int _iMaxLen, uint64_t _uiAddress, const uint8_t * _pui8Value ) const;

		/**
		 * The WM_NOTIFY -> NM_CUSTOMDRAW -> CDDS_ITEMPREPAINT handler.
		 *
		 * \param _lpcdParm The notifacation structure.
		 * \return Returns an LSW_HANDLED code.
		 */
		virtual DWORD						Notify_CustomDraw_ItemPrePaint( LPNMLVCUSTOMDRAW /*_lpcdParm*/ );
	};

}	// namespace mx

