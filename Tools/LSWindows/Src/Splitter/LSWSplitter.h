#pragma once
#include "../LSWWin.h"
#include "../Layout/LSWWidgetLayout.h"
#include "../Widget/LSWWidget.h"


namespace lsw {

	class CSplitter : public CWidget {
	public :
		CSplitter( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );
		~CSplitter();


		// == Enumerations.
		// Splitter styles.
		enum LSW_SPLITTER_STYLES : DWORD {
			LSW_SS_HORIZONTAL				= 0x01,
			LSW_SS_VERTICAL					= 0x02,
			LSW_SS_BOTH						= LSW_SS_HORIZONTAL | LSW_SS_VERTICAL,
		};

		// Attachment type.
		enum LSW_ATTACHMENT {
			LSW_A_NONE,						// No attachment.
			LSW_A_UPPERLEFT,				// Only for LSW_SS_BOTH.
			LSW_A_UPPERRIGHT,				// Only for LSW_SS_BOTH.
			LSW_A_LOWERLEFT,				// Only for LSW_SS_BOTH.
			LSW_A_LOWERRIGHT,				// Only for LSW_SS_BOTH.
			LSW_A_UPPER,					// Only for LSW_SS_HORIZONTAL.
			LSW_A_BOTTOM,					// Only for LSW_SS_HORIZONTAL.
			LSW_A_LEFT,						// Only for LSW_SS_VERTICAL.
			LSW_A_RIGHT,					// Only for LSW_SS_VERTICAL.
		};
#define LSW_S_NOATTACH						static_cast<size_t>(-1)


		// == Functions.
		// Window rectangle.
		virtual LSW_RECT					WindowRect( const CWidget * pwChild ) const;

		// Client rectangle.
		virtual LSW_RECT					ClientRect( const CWidget * pwChild ) const;

		// Attach a control to the splitter.
		virtual void						Attach( CWidget * _pwWidget, LSW_ATTACHMENT _aAttachment );

		// Set splitter styles.
		void								SetSplitterType( LSW_SPLITTER_STYLES _ssType );

		// Get the splitter type.
		DWORD								GetSplitterType() const { return m_dwSplitterType; }

		// Sets the bar width.
		void								SetBarWidth( DWORD _dwWidth );

		// Determines the type of control this is.
		virtual uint32_t					WidgetType() const { return LSW_LT_SPLITTER; }

		// Returns true if this is a CSplitter class.
		virtual bool						IsSplitter() const { return true; }


	protected :
		// == Types.
		// An attachment.
		struct LSW_SPLITTERATTACH {
			CWidget *						pwWidget;
			LSW_ATTACHMENT					aAttachment;
		};

		// == Members.
		// Are we dragging?
		BOOL								m_bDragging = FALSE;
		// Last-drawn point.
		POINT								m_pLastPoint;
		// Splitter type.
		DWORD								m_dwSplitterType;
		// Width of the bar.
		DWORD								m_dwBarWidth = 4;
		// Y position of the horizontal bar.
		INT									m_iHorPos = 10;
		// X position of the vertical bar.
		INT									m_iVertPos = 10;
		// Attached controls.
		std::vector<LSW_SPLITTERATTACH>		m_vAttachments;
		// Set when the mouse moves, unset when SetCursor() is called.
		bool								m_bSetCursorToggle = false;


		// == Functions.
		// Draw the XOR bar.
		void								DrawXorBar( HDC _hDc, INT _iX1, INT _iY1, INT _iWidth, INT _iHeight );

		// Draw the bar for this control.
		void								DrawBar( POINT &_pPoint, LSW_RECT &_rClient, BOOL _bEnforceNew );

		// Hit test.  _pPoint should be in client coordinates.
		bool								HitTest( const POINT &_pPoint, DWORD &_dwHit ) const;

		// Gets the Y position of the horizontal bar.
		INT									GetHorBarY() const;

		// Gets the X position of the vertical bar.
		INT									GetVertBarX() const;

		// Gets the upper-left rectangle in a horizontal and vertical splitter.
		LSW_RECT							UpperLeftRect() const;

		// Gets the upper-right rectangle in a horizontal and vertical splitter.
		LSW_RECT							UpperRightRect() const;

		// Gets the lower-left rectangle in a horizontal and vertical splitter.
		LSW_RECT							LowerLeftRect() const;

		// Gets the lower-right rectangle in a horizontal and vertical splitter.
		LSW_RECT							LowerRightRect() const;

		// Gets the top rectangle in a horizontal splitter.
		LSW_RECT							UpperRect() const;

		// Gets the bottom rectangle in a horizontal splitter.
		LSW_RECT							LowerRect() const;

		// Gets the left rectangle in a vertical splitter.
		LSW_RECT							LeftRect() const;

		// Gets the right rectangle in a vertical splitter.
		LSW_RECT							RightRect() const;

		// Gets a control's attachment by pointer.
		DWORD								FindAttachment( const CWidget * _pwWidget ) const;

		// Gets the index of a control or LSW_S_NOATTACH.
		size_t								IndexOfAttachment( const CWidget * _pwWidget ) const;

		// Remove a child.
		virtual void						RemoveChild( const CWidget * _pwChild );

		// Size all attachments.
		void								SizeAttachments() const;

		/**
		 * The WM_SIZE handler.
		 *
		 * \param _wParam The type of resizing requested.
		 * \param _lWidth The new width of the client area.
		 * \param _lHeight The new height of the client area.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					Size( WPARAM _wParam, LONG _lWidth, LONG _lHeight );

		/**
		 * The WM_SIZING handler.
		 *
		 * \param _iEdge The edge of the window that is being sized.
		 * \param _prRect A pointer to a RECT structure with the screen coordinates of the drag rectangle. To change the size or position of the drag rectangle, an application must change the members of this structure.
		 * \return Returns a LSW_HANDLED enumeration.
		 */
		virtual LSW_HANDLED					Sizing( INT _iEdge, LSW_RECT * _prRect );

		/**
		 * WM_SETCURSOR.
		 * 
		 * \param _pwControl A pointer to the window that contains the cursor
		 * \param _wHitTest Specifies the hit-test result for the cursor position.
		 * \param _wIdent Specifies the mouse window message which triggered this event, such as WM_MOUSEMOVE. When the window enters menu mode, this value is zero.
		 * \return Returns a LSW_HANDLED enumeration.
		 **/
		virtual LSW_HANDLED					SetCursor( CWidget * _pwControl, WORD _wHitTest, WORD _wIdent );

		// WM_PAINT.
		virtual LSW_HANDLED					Paint();

		// WM_LBUTTONDOWN.
		virtual LSW_HANDLED					LButtonDown( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_MOUSEMOVE.
		virtual LSW_HANDLED					MouseMove( DWORD _dwVirtKeys, const POINTS &_pCursorPos );

		// WM_LBUTTONUP.
		virtual LSW_HANDLED					LButtonUp( DWORD _dwVirtKeys, const POINTS &_pCursorPos );
	};

}	// namespace lsw
