#pragma once

#include "../LSWWin.h"
#include "../Widget/LSWWidget.h"

namespace lsw {

	/**
	 * A widget class representing an up-down (spin) control.
	 **/
	class CUpDown : public CWidget {
	public :
		CUpDown( const LSW_WIDGET_LAYOUT &_wlLayout, CWidget * _pwParent, bool _bCreateWidget = true, HMENU _hMenu = NULL, uint64_t _ui64Data = 0 );



	protected :
		// == Functions.
		/**
		 * Setting the HWND after the control has been created.
		 * 
		 * \param _hWnd Handle to the window.
		 **/
		virtual void						InitControl( HWND _hWnd );

		/**
		 * Gets the radix base (10 or 16).
		 * 
		 * \return Returns the current radix base.
		 **/
		virtual uint32_t					GetBase() const;

		/**
		 * Gets the position.
		 * 
		 * \return Returns the current position of the up-down control.
		 **/
		virtual int32_t						GetPos() const;

		/**
		 * Gets the buddy control.
		 * 
		 * \return Returns a pointer to the buddy control widget.
		 **/
		virtual CWidget *					GetBuddy() const;

		/**
		 * Gets the range.
		 * 
		 * \param _i32Lower A reference to an integer that receives the lower limit of the range.
		 * \param _i32Upper A reference to an integer that receives the upper limit of the range.
		 **/
		virtual void						GetRange( int32_t &_i32Lower, int32_t &_i32Upper ) const;

		/**
		 * Gets the range.
		 * 
		 * \return Returns the range of the control.
		 **/
		virtual int64_t						GetRange() const;

		/**
		 * Determines if the control is using Unicode or not.
		 * 
		 * \return Returns TRUE if the control is using Unicode, or FALSE otherwise.
		 **/
		virtual BOOL						GetUnicodeFormat() const;

		/**
		 * Sets the base radix (10 or 16) and returns the previous radix.
		 * 
		 * \param _ui32Base The new radix base to set.
		 * \return Returns the previous radix base.
		 **/
		virtual uint32_t					SetBase( uint32_t _ui32Base );

		/**
		 * Sets the buddy control and returns the previous buddy control.
		 * 
		 * \param _pwBuddy A pointer to the new buddy control widget.
		 * \return Returns a pointer to the previous buddy control widget.
		 **/
		virtual CWidget *					SetBuddy( CWidget * _pwBuddy );

		/**
		 * Sets the position and returns the previous position.
		 * 
		 * \param _i32NewPos The new position to set.
		 * \return Returns the previous position.
		 **/
		virtual int32_t						SetPos( int32_t _i32NewPos );

		/**
		 * Sets the range.
		 * 
		 * \param _i32Lower The new lower limit for the range.
		 * \param _i32Upper The new upper limit for the range.
		 **/
		virtual void						SetRange( int32_t _i32Lower, int32_t _i32Upper );

		/**
		 * Sets to use Unicode (TRUE) or ASCII (FALSE) and returns the previous Unicode setting.
		 * 
		 * \param _bUseUnicode TRUE to use Unicode format, FALSE to use ASCII format.
		 * \return Returns the previous Unicode setting.
		 **/
		virtual BOOL						SetUnicodeFormat( BOOL _bUseUnicode );

		// Sort routine.
		//static int CALLBACK					CompareFunc( LPARAM _lParam1, LPARAM _lParam2, LPARAM _lParamSort );

	private :
		/**
		 * A typedef for the parent class structure.
		 **/
		typedef CWidget						Parent;
	};

}	// namespace lsw
