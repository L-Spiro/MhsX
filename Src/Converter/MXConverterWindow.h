#pragma once

#include "../Utilities/MXUtilities.h"
#include <Docking/LSWDockable.h>
#include <ImageList/LSWImageList.h>
#include <Images/LSWBitmap.h>

using namespace lsw;

namespace mx {

	class CConverterWindow : public lsw::CDockable {
	public :
		CConverterWindow( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CConverterWindow();


		// == Enumerations.
		// Images.
		enum MX_IMAGES {
			MX_I_SCIENTIFIC_NOTATION,

			MX_I_TOTAL
		};


		// == Functions.
		// WM_INITDIALOG.
		virtual LSW_HANDLED					InitDialog();

		// WM_COMMAND from control.
		virtual LSW_HANDLED					Command( WORD _wCtrlCode, WORD _Id, CWidget * _pwSrc );

		// Virtual client rectangle.  Can be used for things that need to be adjusted based on whether or not status bars, toolbars, etc. are present.
		virtual const LSW_RECT				VirtualClientRect( const CWidget * pwChild ) const;

		// Returns the desired width when first docking into a vertical split.
		virtual INT							PreferredDockWidth() { return 300; }

		// Returns the desired height when first docking into a horizontal split.
		virtual INT							PreferredDockHeight() { return 200; }

		/**
		 * Handles WM_ERASEBKGND.
		 * \brief Allows custom background erasing.
		 *
		 * \param _hDc Device context provided for erasing.
		 * \return Returns a LSW_HANDLED code.
		 */
		virtual LSW_HANDLED					EraseBkgnd( HDC /*_hDc*/ ) { return LSW_H_CONTINUE; }


	protected :
		// == Types.
		typedef void						(* PfSetValueFunc)( CWidget *, ee::CExpEvalContainer::EE_RESULT, ee::EE_NUM_CONSTANTS, size_t, bool, size_t, bool );
		typedef ee::CExpEvalContainer::EE_RESULT (* PfGetValueFunc)( CWidget *, ee::EE_NUM_CONSTANTS, size_t &, bool );

		// Conversion information associated with each edit.
		struct MX_CONV_INFO {
			WORD							wId;
			ee::EE_NUM_CONSTANTS			ncType;
			size_t							sBits;
			bool							bBigEnd;
			bool							bHexInput;
			PfGetValueFunc					pfGetValueFunc;
			PfSetValueFunc					pfSetValueFunc;
		};

		// == Members.
		// Image list.
		CImageList							m_iImages;

		// Images.
		CBitmap								m_bBitmaps[MX_I_TOTAL];

		// Image mapping.
		INT									m_iImageMap[MX_I_TOTAL];

		// Last update.
		WORD								m_wLastUpdateCtl;

		// The edit color for rare formats.
		static HBRUSH						m_hRarest;

		// The edit color for float formats.
		static HBRUSH						m_hFloats;

		// The edit color for signed formats.
		static HBRUSH						m_hSign;

		// The edit color for unsigned formats.
		static HBRUSH						m_hUnsign;

		// The edit color for the hex edit.
		static HBRUSH						m_hHex;

		// The edit color for the oct edit.
		static HBRUSH						m_hOct;

		// The edit color for the binary edit.
		static HBRUSH						m_hBinary;

		// The edit color for the string edits.
		static HBRUSH						m_hStrings;

		// The interlocked counter.
		static LONG							m_lCount;

		// Data for each edit.
		static MX_CONV_INFO					m_ciInfo[];


		// == Functions.
		// WM_CTLCOLOREDIT.
		virtual LSW_HANDLED					CtlColorEdit( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		// WM_CTLCOLORSTATIC.
		virtual LSW_HANDLED					CtlColorStatic( HDC _hDc, CWidget * _pwControl, HBRUSH &_hBrush );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValue( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueHex( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueOct( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueChar( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueUtf8( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueUtf16( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueUtf16_BE( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueUtf32( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueUtf32_BE( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Gets the value from the given text edit.
		static ee::CExpEvalContainer::EE_RESULT
											GetValueBinary( CWidget * _pwControl, ee::EE_NUM_CONSTANTS _ncType, size_t &_sSize, bool _bHex );

		// Writes the value to the given edit.
		static void							SetValueStd( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize );

		// Writes the value to the given edit.
		template <typename _tT>
		static void							SetValueSigned( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
			if ( _bByteSwap ) {
				_rRes = ByteSwapBySize( _rRes, _sSize );
			}
			std::wstring wsTmp;
			CUtilities::ToSigned( static_cast<_tT>(_rRes.u.i64Val), wsTmp );
			wsTmp.push_back( L' ' );
			wsTmp.push_back( L'/' );
			wsTmp.push_back( L'*' );
			int64_t i64Val = (_rRes.u.i64Val << (64ULL - (sizeof( _tT ) * 8))) >> (64ULL - (sizeof( _tT ) * 8));
			if ( i64Val < 0 ) {
				wsTmp.push_back( L'-' );
				CUtilities::ToHex( -i64Val, wsTmp, ((sizeof( _tT ) * 8) + 3) / 4 );
				wsTmp.push_back( L',' );
				wsTmp.push_back( L' ' );
			}
			_rRes.u.ui64Val = (_rRes.u.ui64Val << (64ULL - (sizeof( _tT ) * 8))) >> (64ULL - (sizeof( _tT ) * 8));
			CUtilities::ToHex( _rRes.u.ui64Val, wsTmp, ((sizeof( _tT ) * 8) + 3) / 4 );
			wsTmp.push_back( L'*' );
			wsTmp.push_back( L'/' );
			_pwControl->SetTextW( wsTmp.c_str() );
		}

		// Writes the value to the given edit.
		template <typename _tT>
		static void							SetValueUnsigned( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
			if ( _bByteSwap ) {
				_rRes = ByteSwapBySize( _rRes, _sSize );
			}
			std::wstring wsTmp;
			CUtilities::ToUnsigned( static_cast<_tT>(_rRes.u.ui64Val), wsTmp );
			wsTmp.push_back( L' ' );
			wsTmp.push_back( L'/' );
			wsTmp.push_back( L'*' );
			CUtilities::ToHex( static_cast<_tT>(_rRes.u.ui64Val), wsTmp, ((sizeof( _tT ) * 8) + 3) / 4 );
			wsTmp.push_back( L'*' );
			wsTmp.push_back( L'/' );
			_pwControl->SetTextW( wsTmp.c_str() );
		}

		// Writes the value to the given edit.
		template <unsigned _uE, unsigned _uM, signed _sImplB, signed _sSignB, unsigned _uPrec>
		static void							SetValueFloating( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp ) {
			if ( _bByteSwap ) {
				_rRes = ByteSwapBySize( _rRes, _sSize );
			}
			/*volatile int iE = _uE;
			volatile int iM = _uM;
			volatile int iImplB = _sImplB;
			volatile int iSignB = _sSignB;
			volatile int iPrec = _uPrec;*/
			ee::CFloatX fFloat;
			fFloat.CreateFromBits( _rRes.u.ui64Val, _uE, _uM, _sImplB, _sSignB );
			//volatile uint64_t uiSize = fFloat.TotalBits( _uE, _uM, _sImplB, _sSignB );
			std::wstring wsTmp;
			CUtilities::ToDouble( fFloat.AsDouble(), wsTmp, _bAltDisp ? -_uPrec : 0 );
			wsTmp.push_back( L' ' );
			wsTmp.push_back( L'/' );
			wsTmp.push_back( L'*' );
			_rRes.u.ui64Val = (_rRes.u.ui64Val << (64ULL - _sSize)) >> (64ULL - _sSize);
			CUtilities::ToHex( _rRes.u.ui64Val, wsTmp, (_sSize + 3) / 4 );
			wsTmp.push_back( L'*' );
			wsTmp.push_back( L'/' );
			_pwControl->SetTextW( wsTmp.c_str() );
		}

		// Writes the value to the given edit.
		static void							SetValueHex( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueOct( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueChar( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueUtf8( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueUtf16( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueUtf16_BE( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueUtf32( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueUtf32_BE( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Writes the value to the given edit.
		static void							SetValueBinary( CWidget * _pwControl, ee::CExpEvalContainer::EE_RESULT _rRes, ee::EE_NUM_CONSTANTS _ncType, size_t _sSize, bool _bByteSwap, size_t _sSrcSize, bool _bAltDisp );

		// Byteswaps based on the number of bits passed.
		static ee::CExpEvalContainer::EE_RESULT
											ByteSwapBySize( ee::CExpEvalContainer::EE_RESULT _rVal, size_t _sSize );

		// Updates all texts given an edit that was modified to trigger the update.
		void								UpdateAll( CWidget * _pwControl, WORD _wId, const MX_CONV_INFO &_ciInfo );
		
		// Updates all texts based on which was last used for the update..
		void								UpdateAll();
	};

}	// namespace mx
