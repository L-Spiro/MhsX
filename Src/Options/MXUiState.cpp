#include "MXUiState.h"
#include "../Strings/MXStringDecoder.h"
#include "../Strings/MXStringMacros.h"
#include "../Utilities/MXUtilities.h"

#include <EEExpEval.h>

#include <algorithm>
#include <cmath>


namespace mx {

	/** The most nested levels of a saved splitter arrangement that are read. */
	static constexpr size_t												MX_UI_MAX_DEPTH = 32;

	/**
	 * Adds an object or array member to a JSON object or array.
	 *
	 * \param _peParent The JSON object or array.
	 * \param _sName The name of the member.
	 * \param _bArray If true, an array is added; otherwise an object.
	 * \return Returns the new member.
	 **/
	static lson::CJson::LSON_ELEMENT * MX_AddContainer( lson::CJson::LSON_ELEMENT * _peParent, const std::string &_sName, bool _bArray ) {
		_peParent->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
		lson::CJson::LSON_ELEMENT * peNew = _peParent->vObjectMembers.back().get();
		if ( _bArray ) { lson::CJson::CreateArrayElement( _sName, (*peNew) ); }
		else { lson::CJson::CreateObjectElement( _sName, (*peNew) ); }
		return peNew;
	}

	/**
	 * Adds a string member to a JSON object or array.
	 *
	 * \param _peParent The JSON object or array.
	 * \param _sName The name of the member.
	 * \param _wsValue The string.
	 **/
	static void MX_AddString( lson::CJson::LSON_ELEMENT * _peParent, const std::string &_sName, const std::wstring &_wsValue ) {
		_peParent->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
		lson::CJson::CreateStringElement( _sName, ee::CExpEval::ToJsonString( ee::CExpEval::ToUtf8( _wsValue ) ), (*_peParent->vObjectMembers.back()) );
	}

	// == Functions.
	/**
	 * Adds a file to the top of the recent files (moving it if it is already there).
	 *
	 * \param _wsPath The full path of the file.
	 **/
	void CUiState::AddRecentFile( const std::wstring &_wsPath ) {
		if ( _wsPath.empty() ) { return; }
		try {
			RemoveRecentFile( _wsPath );
			vRecentFiles.insert( vRecentFiles.begin(), CSecureWString( _wsPath ) );
			if ( vRecentFiles.size() > MX_MAX_RECENT_FILES ) { vRecentFiles.resize( MX_MAX_RECENT_FILES ); }
		}
		catch ( ... ) {}
	}

	/**
	 * Removes a file from the recent files.
	 *
	 * \param _wsPath The full path of the file.
	 **/
	void CUiState::RemoveRecentFile( const std::wstring &_wsPath ) {
		vRecentFiles.erase( std::remove_if( vRecentFiles.begin(), vRecentFiles.end(), [&]( const CSecureWString &_swsThis ) {
			return ::_wcsicmp( _swsThis.c_str(), _wsPath.c_str() ) == 0;
		} ), vRecentFiles.end() );
	}

	/**
	 * Saves the state as a "Ui" member of a JSON object.
	 *
	 * \param _peRoot The JSON object.
	 * \return Returns true if the state was added.
	 **/
	bool CUiState::Save( lson::CJson::LSON_ELEMENT * _peRoot ) const {
		if ( !_peRoot ) { return false; }
		try {
			lson::CJson::LSON_ELEMENT * peUi = MX_AddContainer( _peRoot, _DEC_S_B27B6212_Ui, false );
			MX_JSON_NUMBER( _DEC_S_70A1EA5F_Version, 1, peUi );
			if ( wMain.bValid ) { SaveWindow( wMain, _DEC_S_1F1A625A_Main, peUi ); }
			if ( wHexEditor.bValid ) { SaveWindow( wHexEditor, _DEC_S_BE50D00D_HexEditor, peUi ); }

			lson::CJson::LSON_ELEMENT * peRecent = MX_AddContainer( peUi, _DEC_S_CB92E9AC_RecentFiles, true );
			for ( size_t I = 0; I < vRecentFiles.size(); ++I ) {
				MX_AddString( peRecent, std::to_string( I ), std::wstring( vRecentFiles[I].c_str() ) );
			}

			if ( bHaveInspector ) { SaveInspector( sInspector, MX_AddContainer( peUi, _DEC_S_F02CD328_Inspector, false ) ); }
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Loads the state from the "Ui" member of the root object of a JSON file.  Values that are missing keep their current values.
	 *
	 * \param _pjJson The JSON file.
	 * \return Returns true if the state was found.
	 **/
	bool CUiState::Load( lson::CJson * _pjJson ) {
		if ( !_pjJson || !_pjJson->GetContainer() ) { return false; }
		try {
			lson::CJsonContainer * pjcContainer = _pjJson->GetContainer();
			const lson::CJsonContainer::LSON_JSON_VALUE & jvRoot = pjcContainer->GetValue( pjcContainer->GetRoot() );
			if ( jvRoot.vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { return false; }
			const lson::CJsonContainer::LSON_JSON_VALUE * pjvUi = pjcContainer->GetMemberByName( jvRoot, _DEC_S_B27B6212_Ui );
			if ( !pjvUi || pjvUi->vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { return false; }

			MX_WINDOW wTmp;
			if ( LoadWindow( pjcContainer->GetMemberByName( (*pjvUi), _DEC_S_1F1A625A_Main ), pjcContainer, wTmp ) ) { wMain = std::move( wTmp ); }
			wTmp = MX_WINDOW();
			if ( LoadWindow( pjcContainer->GetMemberByName( (*pjvUi), _DEC_S_BE50D00D_HexEditor ), pjcContainer, wTmp ) ) { wHexEditor = std::move( wTmp ); }

			const lson::CJsonContainer::LSON_JSON_VALUE * pjvRecent = pjcContainer->GetMemberByName( (*pjvUi), _DEC_S_CB92E9AC_RecentFiles );
			if ( pjvRecent && pjvRecent->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
				vRecentFiles.clear();
				for ( size_t I = 0; I < pjvRecent->vArray.size() && vRecentFiles.size() < MX_MAX_RECENT_FILES; ++I ) {
					std::wstring wsPath;
					if ( GetString( pjcContainer->GetValue( pjvRecent->vArray[I] ), pjcContainer, wsPath ) && wsPath.size() ) {
						vRecentFiles.push_back( CSecureWString( wsPath ) );
					}
				}
			}

			CInspector::MX_SETTINGS sTmp;
			if ( LoadInspector( pjcContainer->GetMemberByName( (*pjvUi), _DEC_S_F02CD328_Inspector ), pjcContainer, sTmp ) ) {
				sInspector = std::move( sTmp );
				bHaveInspector = true;
			}
			return true;
		}
		catch ( ... ) { return false; }
	}

	/**
	 * Captures the state of a window: its position, the arrangement of its splitter, and which of its dockables are open, docked,
	 *	or floating (and where).
	 *
	 * \param _pwWindow The window.
	 * \param _pmsSplitter Its splitter, or nullptr.
	 * \param _vDockables The dockables the window can have.
	 * \param _wWindow Receives the state.
	 **/
	void CUiState::CaptureWindow( lsw::CWidget * _pwWindow, const lsw::CMultiSplitter * _pmsSplitter,
		const std::vector<MX_KNOWN_DOCKABLE> &_vDockables, MX_WINDOW &_wWindow ) {
		if ( !_pwWindow || !_pwWindow->Wnd() ) { return; }
		try {
			MX_WINDOW wNew;
			WINDOWPLACEMENT wpPlacement = { sizeof( wpPlacement ) };
			if ( !::GetWindowPlacement( _pwWindow->Wnd(), &wpPlacement ) ) { return; }
			wNew.rNormal = wpPlacement.rcNormalPosition;
			wNew.bMaximized = wpPlacement.showCmd == SW_SHOWMAXIMIZED || ::IsZoomed( _pwWindow->Wnd() ) ||
				(::IsIconic( _pwWindow->Wnd() ) && (wpPlacement.flags & WPF_RESTORETOMAXIMIZED));
			if ( _pmsSplitter ) {
				_pmsSplitter->GetLayout( wNew.lnLayout );
				wNew.bHasLayout = true;
			}
			for ( const auto & kdThis : _vDockables ) {
				MX_DOCKABLE dThis;
				dThis.wId = kdThis.first;
				dThis.bOpen = kdThis.second && kdThis.second->Wnd();
				if ( dThis.bOpen ) {
					dThis.bFloating = kdThis.second->DockedInto() == nullptr;
					if ( dThis.bFloating ) { ::GetWindowRect( kdThis.second->Wnd(), &dThis.rWindow ); }
				}
				wNew.vDockables.push_back( dThis );
			}
			wNew.bValid = true;
			_wWindow = std::move( wNew );
		}
		catch ( ... ) {}
	}

	/**
	 * Moves a window to its saved position (if the position is still on a monitor).  A window that was maximized is maximized
	 *	once it is shown.
	 *
	 * \param _pwWindow The window.
	 * \param _wWindow The saved state.
	 **/
	void CUiState::RestorePlacement( lsw::CWidget * _pwWindow, const MX_WINDOW &_wWindow ) {
		if ( !_pwWindow || !_pwWindow->Wnd() || !_wWindow.bValid || !IsOnScreen( _wWindow.rNormal ) ) { return; }
		WINDOWPLACEMENT wpPlacement = { sizeof( wpPlacement ) };
		if ( !::GetWindowPlacement( _pwWindow->Wnd(), &wpPlacement ) ) { return; }
		wpPlacement.rcNormalPosition = _wWindow.rNormal;
		wpPlacement.flags = 0;
		wpPlacement.showCmd = ::IsWindowVisible( _pwWindow->Wnd() ) ? SW_SHOWNORMAL : SW_HIDE;
		::SetWindowPlacement( _pwWindow->Wnd(), &wpPlacement );
		if ( _wWindow.bMaximized ) { ::PostMessageW( _pwWindow->Wnd(), WM_SYSCOMMAND, SC_MAXIMIZE, 0 ); }
	}

	/**
	 * Rearranges a window's splitter and dockables as saved.  The dockables that should be open must already exist.
	 *
	 * \param _pmsSplitter The splitter.
	 * \param _wWindow The saved state.
	 * \param _vFixed Widgets docked in the splitter that are not dockables (such as the main view).
	 * \param _vDockables The dockables the window can have.
	 **/
	void CUiState::RestoreLayout( lsw::CMultiSplitter * _pmsSplitter, const MX_WINDOW &_wWindow,
		const std::vector<lsw::CWidget *> &_vFixed, const std::vector<MX_KNOWN_DOCKABLE> &_vDockables ) {
		if ( !_wWindow.bValid ) { return; }
		try {
			if ( _pmsSplitter && _wWindow.bHasLayout ) {
				std::vector<lsw::CWidget *> vWidgets = _vFixed;
				for ( const auto & kdThis : _vDockables ) {
					if ( kdThis.second ) { vWidgets.push_back( kdThis.second ); }
				}
				_pmsSplitter->SetLayout( _wWindow.lnLayout, vWidgets );
			}
			for ( const auto & kdThis : _vDockables ) {
				if ( !kdThis.second || !kdThis.second->Wnd() ) { continue; }
				const MX_DOCKABLE * pdSaved = _wWindow.Dockable( kdThis.first );
				if ( !pdSaved || !pdSaved->bFloating ) { continue; }
				kdThis.second->Float( IsOnScreen( pdSaved->rWindow ) ? &pdSaved->rWindow : nullptr );
			}
		}
		catch ( ... ) {}
	}

	/**
	 * Saves a window.
	 *
	 * \param _wWindow The window.
	 * \param _sName The name of the JSON member.
	 * \param _peParent The JSON object to which to add it.
	 **/
	void CUiState::SaveWindow( const MX_WINDOW &_wWindow, const std::string &_sName, lson::CJson::LSON_ELEMENT * _peParent ) {
		lson::CJson::LSON_ELEMENT * peWindow = MX_AddContainer( _peParent, _sName, false );
		SaveRect( _wWindow.rNormal, peWindow );
		MX_JSON_BOOL( _DEC_S_09564C55_Maximized, _wWindow.bMaximized, peWindow );
		if ( _wWindow.bHasLayout ) { SaveNode( _wWindow.lnLayout, _DEC_S_3D966ED4_Layout, peWindow ); }
		lson::CJson::LSON_ELEMENT * peDockables = MX_AddContainer( peWindow, _DEC_S_11E7B106_Dockables, true );
		for ( size_t I = 0; I < _wWindow.vDockables.size(); ++I ) {
			const MX_DOCKABLE & dThis = _wWindow.vDockables[I];
			lson::CJson::LSON_ELEMENT * peDockable = MX_AddContainer( peDockables, std::to_string( I ), false );
			MX_JSON_NUMBER( _DEC_S_2ABD43F2_Id, dThis.wId, peDockable );
			MX_JSON_BOOL( _DEC_S_04422C9A_Open, dThis.bOpen, peDockable );
			MX_JSON_BOOL( _DEC_S_10C5A456_Floating, dThis.bFloating, peDockable );
			if ( dThis.bFloating ) { SaveRect( dThis.rWindow, peDockable ); }
		}
	}

	/**
	 * Saves a node of the arrangement of a splitter.
	 *
	 * \param _lnNode The node.
	 * \param _sName The name of the JSON member.
	 * \param _peParent The JSON object or array to which to add it.
	 **/
	void CUiState::SaveNode( const lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE &_lnNode, const std::string &_sName,
		lson::CJson::LSON_ELEMENT * _peParent ) {
		lson::CJson::LSON_ELEMENT * peNode = MX_AddContainer( _peParent, _sName, false );
		MX_JSON_BOOL( _DEC_S_251A0E1E_Layer, _lnNode.bLayer, peNode );
		MX_JSON_NUMBER( _DEC_S_214E7FC9_Dist, _lnNode.iDist, peNode );
		MX_JSON_BOOL( _DEC_S_49BB6296_Far, _lnNode.bLockToFar, peNode );
		if ( _lnNode.bLayer ) {
			MX_JSON_NUMBER( _DEC_S_7034B515_Split, _lnNode.stSplitType, peNode );
			lson::CJson::LSON_ELEMENT * peChildren = MX_AddContainer( peNode, _DEC_S_58E1D3EC_Children, true );
			for ( size_t I = 0; I < _lnNode.vChildren.size(); ++I ) {
				SaveNode( _lnNode.vChildren[I], std::to_string( I ), peChildren );
			}
		}
		else {
			lson::CJson::LSON_ELEMENT * peWidgets = MX_AddContainer( peNode, _DEC_S_52E5DD5D_Widgets, true );
			for ( size_t I = 0; I < _lnNode.vWidgets.size(); ++I ) {
				peWidgets->vObjectMembers.push_back( std::make_unique<lson::CJson::LSON_ELEMENT>() );
				lson::CJson::CreateNumberElement( std::to_string( I ), static_cast<double>(_lnNode.vWidgets[I]), (*peWidgets->vObjectMembers.back()) );
			}
			MX_JSON_NUMBER( _DEC_S_61DFDDD0_Selected, _lnNode.sSelected, peNode );
		}
	}

	/**
	 * Saves the Inspector settings.
	 *
	 * \param _sSettings The settings.
	 * \param _peParent The JSON object to which to add them.
	 **/
	void CUiState::SaveInspector( const CInspector::MX_SETTINGS &_sSettings, lson::CJson::LSON_ELEMENT * _peParent ) {
		lson::CJson::LSON_ELEMENT * peRows = MX_AddContainer( _peParent, _DEC_S_AAB95B9F_Rows, true );
		for ( size_t I = 0; I < _sSettings.vRows.size(); ++I ) {
			lson::CJson::LSON_ELEMENT * peRow = MX_AddContainer( peRows, std::to_string( I ), false );
			MX_JSON_NUMBER( _DEC_S_2CECF817_Type, _sSettings.vRows[I].itType, peRow );
			MX_JSON_BOOL( _DEC_S_CEFCB299_Shown, _sSettings.vRows[I].bShown, peRow );
		}
		MX_JSON_NUMBER( _DEC_S_D91677E9_Format, _sSettings.nfFormat, _peParent );
		MX_AddString( _peParent, _DEC_S_A6837029_DateFormat, _sSettings.wsDateFormat );
		MX_AddString( _peParent, _DEC_S_590179D9_TimeFormat, _sSettings.wsTimeFormat );
		MX_JSON_BOOL( _DEC_S_70B4D80A_AllowDocking, _sSettings.bAllowDocking, _peParent );
	}

	/**
	 * Loads a window.
	 *
	 * \param _pjvWindow The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _wWindow Receives the window.
	 * \return Returns true if the window was loaded.
	 **/
	bool CUiState::LoadWindow( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvWindow, lson::CJsonContainer * _pjcContainer,
		MX_WINDOW &_wWindow ) {
		if ( !_pjvWindow || _pjvWindow->vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { return false; }
		_wWindow = MX_WINDOW();
		if ( !LoadRect( _pjvWindow, _pjcContainer, _wWindow.rNormal ) ) { return false; }
		GetBool( _pjvWindow, _pjcContainer, _DEC_S_09564C55_Maximized, _wWindow.bMaximized );
		_wWindow.bHasLayout = LoadNode( _pjcContainer->GetMemberByName( (*_pjvWindow), _DEC_S_3D966ED4_Layout ), _pjcContainer, _wWindow.lnLayout, 0 );

		const lson::CJsonContainer::LSON_JSON_VALUE * pjvDockables = _pjcContainer->GetMemberByName( (*_pjvWindow), _DEC_S_11E7B106_Dockables );
		if ( pjvDockables && pjvDockables->vtType == lson::CJsonContainer::LSON_VT_ARRAY ) {
			for ( size_t I = 0; I < pjvDockables->vArray.size(); ++I ) {
				const lson::CJsonContainer::LSON_JSON_VALUE * pjvDockable = &_pjcContainer->GetValue( pjvDockables->vArray[I] );
				if ( pjvDockable->vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { continue; }
				MX_DOCKABLE dThis;
				double dId = 0.0;
				if ( !GetNumber( pjvDockable, _pjcContainer, _DEC_S_2ABD43F2_Id, dId ) || dId < 1.0 || dId > 65535.0 ) { continue; }
				dThis.wId = static_cast<WORD>(dId);
				GetBool( pjvDockable, _pjcContainer, _DEC_S_04422C9A_Open, dThis.bOpen );
				GetBool( pjvDockable, _pjcContainer, _DEC_S_10C5A456_Floating, dThis.bFloating );
				if ( dThis.bFloating && !LoadRect( pjvDockable, _pjcContainer, dThis.rWindow ) ) { dThis.rWindow = lsw::LSW_RECT( 0, 0, 0, 0 ); }
				_wWindow.vDockables.push_back( dThis );
			}
		}
		_wWindow.bValid = true;
		return true;
	}

	/**
	 * Loads a node of the arrangement of a splitter.
	 *
	 * \param _pjvNode The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _lnNode Receives the node.
	 * \param _sDepth How deep the node is (deeply nested data is refused).
	 * \return Returns true if the node was loaded.
	 **/
	bool CUiState::LoadNode( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvNode, lson::CJsonContainer * _pjcContainer,
		lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE &_lnNode, size_t _sDepth ) {
		if ( !_pjvNode || _pjvNode->vtType != lson::CJsonContainer::LSON_VT_OBJECT || _sDepth > MX_UI_MAX_DEPTH ) { return false; }
		_lnNode = lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE();
		GetBool( _pjvNode, _pjcContainer, _DEC_S_251A0E1E_Layer, _lnNode.bLayer );
		double dVal = 0.0;
		if ( GetNumber( _pjvNode, _pjcContainer, _DEC_S_214E7FC9_Dist, dVal ) ) { _lnNode.iDist = static_cast<INT>(std::clamp( dVal, 0.0, 100000.0 )); }
		GetBool( _pjvNode, _pjcContainer, _DEC_S_49BB6296_Far, _lnNode.bLockToFar );
		if ( _lnNode.bLayer ) {
			if ( GetNumber( _pjvNode, _pjcContainer, _DEC_S_7034B515_Split, dVal ) ) {
				_lnNode.stSplitType = (dVal == static_cast<double>(lsw::CMultiSplitter::LSW_ST_HOR)) ? lsw::CMultiSplitter::LSW_ST_HOR : lsw::CMultiSplitter::LSW_ST_VER;
			}
			const lson::CJsonContainer::LSON_JSON_VALUE * pjvChildren = _pjcContainer->GetMemberByName( (*_pjvNode), _DEC_S_58E1D3EC_Children );
			if ( !pjvChildren || pjvChildren->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }
			for ( size_t I = 0; I < pjvChildren->vArray.size(); ++I ) {
				lsw::CMultiSplitter::LSW_MS_LAYOUT_NODE lnChild;
				if ( LoadNode( &_pjcContainer->GetValue( pjvChildren->vArray[I] ), _pjcContainer, lnChild, _sDepth + 1 ) ) {
					_lnNode.vChildren.push_back( std::move( lnChild ) );
				}
			}
			return _lnNode.vChildren.size() != 0;
		}
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvWidgets = _pjcContainer->GetMemberByName( (*_pjvNode), _DEC_S_52E5DD5D_Widgets );
		if ( !pjvWidgets || pjvWidgets->vtType != lson::CJsonContainer::LSON_VT_ARRAY ) { return false; }
		for ( size_t I = 0; I < pjvWidgets->vArray.size(); ++I ) {
			const lson::CJsonContainer::LSON_JSON_VALUE & jvId = _pjcContainer->GetValue( pjvWidgets->vArray[I] );
			if ( jvId.vtType == lson::CJsonContainer::LSON_VT_DECIMAL && jvId.u.dDecimal >= 1.0 && jvId.u.dDecimal <= 65535.0 ) {
				_lnNode.vWidgets.push_back( static_cast<WORD>(jvId.u.dDecimal) );
			}
		}
		if ( GetNumber( _pjvNode, _pjcContainer, _DEC_S_61DFDDD0_Selected, dVal ) && dVal >= 0.0 && dVal < 65536.0 ) { _lnNode.sSelected = static_cast<size_t>(dVal); }
		return _lnNode.vWidgets.size() != 0;
	}

	/**
	 * Loads the Inspector settings.
	 *
	 * \param _pjvInspector The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _sSettings Receives the settings.
	 * \return Returns true if the settings were loaded.
	 **/
	bool CUiState::LoadInspector( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvInspector, lson::CJsonContainer * _pjcContainer,
		CInspector::MX_SETTINGS &_sSettings ) {
		if ( !_pjvInspector || _pjvInspector->vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { return false; }
		CInspector::DefaultSettings( _sSettings );
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvRows = _pjcContainer->GetMemberByName( (*_pjvInspector), _DEC_S_AAB95B9F_Rows );
		if ( pjvRows && pjvRows->vtType == lson::CJsonContainer::LSON_VT_ARRAY && pjvRows->vArray.size() ) {
			_sSettings.vRows.clear();
			for ( size_t I = 0; I < pjvRows->vArray.size(); ++I ) {
				const lson::CJsonContainer::LSON_JSON_VALUE * pjvRow = &_pjcContainer->GetValue( pjvRows->vArray[I] );
				if ( pjvRow->vtType != lson::CJsonContainer::LSON_VT_OBJECT ) { continue; }
				double dType = -1.0;
				if ( !GetNumber( pjvRow, _pjcContainer, _DEC_S_2CECF817_Type, dType ) || dType < 0.0 || dType >= static_cast<double>(CInspector::MX_IT_TOTAL) ) { continue; }
				CInspector::MX_ROW_SETTING rsRow;
				rsRow.itType = static_cast<CInspector::MX_INSPECTOR_TYPE>(static_cast<uint32_t>(dType));
				GetBool( pjvRow, _pjcContainer, _DEC_S_CEFCB299_Shown, rsRow.bShown );
				_sSettings.vRows.push_back( rsRow );
			}
		}
		// Types missing from the file (added later) are put in.
		CInspector::FixSettings( _sSettings );
		double dVal = 0.0;
		if ( GetNumber( _pjvInspector, _pjcContainer, _DEC_S_D91677E9_Format, dVal ) && dVal >= 0.0 && dVal <= static_cast<double>(CInspector::MX_NF_BINARY) ) {
			_sSettings.nfFormat = static_cast<CInspector::MX_NUMBER_FORMAT>(static_cast<uint32_t>(dVal));
		}
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvText = _pjcContainer->GetMemberByName( (*_pjvInspector), _DEC_S_A6837029_DateFormat );
		if ( pjvText ) { GetString( (*pjvText), _pjcContainer, _sSettings.wsDateFormat ); }
		pjvText = _pjcContainer->GetMemberByName( (*_pjvInspector), _DEC_S_590179D9_TimeFormat );
		if ( pjvText ) { GetString( (*pjvText), _pjcContainer, _sSettings.wsTimeFormat ); }
		GetBool( _pjvInspector, _pjcContainer, _DEC_S_70B4D80A_AllowDocking, _sSettings.bAllowDocking );
		return true;
	}

	/**
	 * Reads a number member of a JSON object.
	 *
	 * \param _pjvObject The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _sName The name of the member.
	 * \param _dValue Receives the value.
	 * \return Returns true if the member is a number.
	 **/
	bool CUiState::GetNumber( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
		const std::string &_sName, double &_dValue ) {
		if ( !_pjvObject ) { return false; }
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal = _pjcContainer->GetMemberByName( (*_pjvObject), _sName );
		if ( !pjvVal || pjvVal->vtType != lson::CJsonContainer::LSON_VT_DECIMAL || !std::isfinite( pjvVal->u.dDecimal ) ) { return false; }
		_dValue = pjvVal->u.dDecimal;
		return true;
	}

	/**
	 * Reads a true/false member of a JSON object.
	 *
	 * \param _pjvObject The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _sName The name of the member.
	 * \param _bValue Receives the value.
	 * \return Returns true if the member is true or false.
	 **/
	bool CUiState::GetBool( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
		const std::string &_sName, bool &_bValue ) {
		if ( !_pjvObject ) { return false; }
		const lson::CJsonContainer::LSON_JSON_VALUE * pjvVal = _pjcContainer->GetMemberByName( (*_pjvObject), _sName );
		if ( !pjvVal ) { return false; }
		if ( pjvVal->vtType == lson::CJsonContainer::LSON_VT_TRUE ) {
			_bValue = true;
			return true;
		}
		if ( pjvVal->vtType == lson::CJsonContainer::LSON_VT_FALSE ) {
			_bValue = false;
			return true;
		}
		return false;
	}

	/**
	 * Reads a string value.
	 *
	 * \param _jvValue The JSON value.
	 * \param _pjcContainer The JSON container.
	 * \param _wsValue Receives the value.
	 * \return Returns true if the value is a string.
	 **/
	bool CUiState::GetString( const lson::CJsonContainer::LSON_JSON_VALUE &_jvValue, lson::CJsonContainer * _pjcContainer,
		std::wstring &_wsValue ) {
		if ( _jvValue.vtType != lson::CJsonContainer::LSON_VT_STRING ) { return false; }
		std::string sTmp;
		CUtilities::ResolveAllEscapes( _pjcContainer->GetString( _jvValue.u.stString ), sTmp, true );
		_wsValue = ee::CExpEval::ToUtf16( sTmp );
		return true;
	}

	/**
	 * Adds a rectangle to a JSON object (as Left, Top, Right, and Bottom).
	 *
	 * \param _rRect The rectangle.
	 * \param _peParent The JSON object.
	 **/
	void CUiState::SaveRect( const lsw::LSW_RECT &_rRect, lson::CJson::LSON_ELEMENT * _peParent ) {
		MX_JSON_NUMBER( _DEC_S_DA554856_Left, _rRect.left, _peParent );
		MX_JSON_NUMBER( _DEC_S_2694592A_Top, _rRect.top, _peParent );
		MX_JSON_NUMBER( _DEC_S_750B5A10_Right, _rRect.right, _peParent );
		MX_JSON_NUMBER( _DEC_S_8EF37792_Bottom, _rRect.bottom, _peParent );
	}

	/**
	 * Reads a rectangle from a JSON object (Left, Top, Right, and Bottom).
	 *
	 * \param _pjvObject The JSON object.
	 * \param _pjcContainer The JSON container.
	 * \param _rRect Receives the rectangle.
	 * \return Returns true if the rectangle was read.
	 **/
	bool CUiState::LoadRect( const lson::CJsonContainer::LSON_JSON_VALUE * _pjvObject, lson::CJsonContainer * _pjcContainer,
		lsw::LSW_RECT &_rRect ) {
		double dLeft, dTop, dRight, dBottom;
		if ( !GetNumber( _pjvObject, _pjcContainer, _DEC_S_DA554856_Left, dLeft ) || !GetNumber( _pjvObject, _pjcContainer, _DEC_S_2694592A_Top, dTop ) ||
			!GetNumber( _pjvObject, _pjcContainer, _DEC_S_750B5A10_Right, dRight ) || !GetNumber( _pjvObject, _pjcContainer, _DEC_S_8EF37792_Bottom, dBottom ) ) { return false; }
		constexpr double dLimit = 1000000.0;
		if ( std::fabs( dLeft ) > dLimit || std::fabs( dTop ) > dLimit || std::fabs( dRight ) > dLimit || std::fabs( dBottom ) > dLimit ) { return false; }
		_rRect = lsw::LSW_RECT( static_cast<LONG>(dLeft), static_cast<LONG>(dTop), static_cast<LONG>(dRight), static_cast<LONG>(dBottom) );
		return true;
	}

	/**
	 * Determines whether enough of a window rectangle is on a monitor for the window to be put there.
	 *
	 * \param _rRect The window rectangle (screen coordinates).
	 * \return Returns true if the rectangle has a reasonable size and its title area is on a monitor.
	 **/
	bool CUiState::IsOnScreen( const lsw::LSW_RECT &_rRect ) {
		if ( _rRect.Width() < 50 || _rRect.Height() < 30 ) { return false; }
		// The top of the window (where it can be dragged) must be on a monitor.
		RECT rTitle = { _rRect.left + 16, _rRect.top, _rRect.right - 16, _rRect.top + 24 };
		return ::MonitorFromRect( &rTitle, MONITOR_DEFAULTTONULL ) != NULL;
	}

}	// namespace mx
