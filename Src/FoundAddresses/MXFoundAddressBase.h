#pragma once

#include "../MXMhsX.h"

#include <atomic>
#include <string>
#include <vector>


namespace mx {

	// == Enumerations.
	// Types of items that can go into the Found Addresses list.
	enum MX_FOUND_ADDRESS_TYPES {
		MX_FAT_INVALID,										// This base class.
		MX_FAT_FOUND_ADDRESS,								// A normal Found Address with locking mechanisms and scripts etc.
		MX_FAT_FOLDER,										// An organizational item that can have child items.
		MX_FAT_CODE,										// A text item that is assembled into machine language for a selectable target machine.
	};

	class CFoundAddressBase {
	public :
		virtual ~CFoundAddressBase();

		// == Functions.
		// Gets the ID of this item (0 is invalid.)
		inline size_t										Id() const { return m_sId; }

		// Gets the ID of the parent item.
		inline size_t										Parent() const { return m_sParent; }

		// Unparents the item.
		inline void											UnParent() {
			m_sParent = 0;
		}

		// Adds an item as a child to this item.
		inline bool											AddChild( CFoundAddressBase * _pfabOther ) {
			try {
				if ( _pfabOther ) {
					if ( HasChild( _pfabOther->Id() ) ) {
						m_vChildren.push_back( _pfabOther->Id() );
					}
					_pfabOther->m_sParent = Id();
				}
				return true;
			}
			catch ( ... ) { return false; }
		}

		// Removes a child from this item.
		inline void											RemoveChild( CFoundAddressBase * _pfabOther ) {
			try {
				if ( _pfabOther ) {
					auto aFound = std::find( m_vChildren.begin(), m_vChildren.end(), _pfabOther->Id() );
					if ( aFound != m_vChildren.end() ) {
						m_vChildren.erase( aFound );
						_pfabOther->m_sParent = 0;
					}
				}
			}
			catch ( ... ) {}
		}

		// Do we have the given child?
		inline bool											HasChild( size_t _sId ) const {
			return std::find( m_vChildren.begin(), m_vChildren.end(), _sId ) == m_vChildren.end();
		}

		// Gets the name of the item.
		inline const std::wstring &							Name() const { return m_wsName; }

		// Gets the name of the item.
		inline const std::wstring &							SetName( const std::wstring &_wsName ) { m_wsName = _wsName; return _wsName; }

		// Gets the item type.
		virtual MX_FOUND_ADDRESS_TYPES						Type() const { return MX_FAT_INVALID; }

		// Gets the item display color.
		inline const RGBQUAD &								Color() const { return m_rgbqColor; }

		// Sets the display color.
		inline CFoundAddressBase &							SetColor( BYTE _bR, BYTE _bG, BYTE _bB, BYTE _bA ) {
			m_rgbqColor.rgbRed = _bR;
			m_rgbqColor.rgbGreen = _bG;
			m_rgbqColor.rgbBlue = _bB;
			m_rgbqColor.rgbReserved = _bA;
			return (*this);
		}



	protected :
		CFoundAddressBase();
		

		// == Members.
		// The ID of the current item.
		size_t												m_sId = 0;
		// The parent item.
		size_t												m_sParent = 0;
		// The name of the item.
		std::wstring										m_wsName;
		// The children items.
		std::vector<size_t>									m_vChildren;
		// The color of the item in the display.  If rgbReserved is 0, use the default display color.
		RGBQUAD												m_rgbqColor = { .rgbReserved = 0 };

		

		// The number of instances of this class.
		static std::atomic<size_t>							s_aClassCount;
		// The ID counter.
		static std::atomic<size_t>							s_aId;
	};

}	// namespace mx
