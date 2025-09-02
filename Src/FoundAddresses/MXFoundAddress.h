#pragma once

#include "../MXMhsX.h"
#include "../CodePages/MXCodePages.h"
#include "../Utilities/MXUtilities.h"
#include "MXFoundAddressBase.h"

#include <EEExpEval.h>

#include <atomic>


namespace mx {

	class CFoundAddress : public CFoundAddressBase {
	public :
		CFoundAddress( CMemHack * _pmhMemHack );
		virtual ~CFoundAddress();


		// == Constants.
		// Maximum array length.
		static constexpr uint32_t							MaxArrayLen() { return 0x1FFFF; }

		// Maximum stride amount.
		static constexpr uint16_t							MaxArrayStride() { return UINT16_MAX; }


		// == Types.
		// A value (data/pointer target/string).
		struct MX_VALUE {
			std::vector<std::vector<uint8_t>>				vValue;
			std::vector<size_t>								vOffsets;
		};

		// A pointer.
		struct MX_POINTER {
			int32_t											i32Offset = 0;									// The offset to add after the read.
		};


		// == Functions.
		// Gets the item type.
		virtual MX_FOUND_ADDRESS_TYPES						Type() const { return MX_FAT_FOUND_ADDRESS; }

		// Sets the address, type, and original data.
		bool												InitWithAddressAndDataType( uint64_t _ui64Address, CUtilities::MX_DATA_TYPES _dtType, const uint8_t * _pui8Data );

		// Sets the address, string type, and original data.
		bool												InitWithAddressandStringType( uint64_t _ui64Address, UINT _uiCodePage, const uint8_t * _pui8Data, size_t _sLen );

		// Sets the Data Type.  Call within a try/catch block.
		void												SetAsDataType( std::vector<std::vector<uint8_t>> &_vLockValue, CUtilities::MX_DATA_TYPES _dtDataType, uint32_t _ui32ArrayLen, uint32_t _ui32ArrayStride, std::vector<std::vector<uint8_t>> * _pvMaxLock = nullptr );

		// Sets the data type as a string.
		void												SetAsString( const std::string &_sLockString, UINT _uiCodePage );

		// Public ID.
		inline uint32_t										PublicId() const { return m_ui32PublicId; }

		// Sets the public ID.
		inline void											SetPublicIf( uint32_t _ui32Id ) { m_ui32PublicId = _ui32Id; }

		// Gets the Address text.
		virtual std::wstring								AddressText() const;

		// Gets the Value text.
		virtual std::wstring								ValueText( bool * _pbRead = nullptr ) const;

		// Gets the Value When Locked text.
		virtual std::wstring								ValueWhenLockedText() const {
			if MX_UNLIKELY( m_vtValueType == CUtilities::MX_VT_STRING ) {
				return LockedLeftText();
			}
			CSecureWString swsTmp, swsError;
			CUtilities::ArrayBytesToWString( m_vLockedData, nullptr, swsTmp, m_dtDataType, std::max<uint32_t>( m_ui32ArrayLen, 1 ), 0, m_bContiguous, swsError );
			if ( swsError.size() ) { return swsError; }
			return swsTmp;
		}

		// Gets the Max Value When Locked text.
		virtual std::wstring								MaxValueWhenLockedText() const {
			if MX_UNLIKELY( m_vtValueType == CUtilities::MX_VT_STRING ) {
				return std::wstring();
			}
			CSecureWString swsTmp, swsError;
			CUtilities::ArrayBytesToWString( m_vLockedDataMax, nullptr, swsTmp, m_dtDataType, std::max<uint32_t>( m_ui32ArrayLen, 1 ), 0, m_bContiguous, swsError );
			if ( swsError.size() ) { return swsError; }
			return swsTmp;
		}

		// Gets the Type text.
		virtual std::wstring								TypeText() const;

		// Gets the global type of the object.
		inline CUtilities::MX_VALUE_TYPE					ValueType() const { return m_vtValueType; }

		// Gets the data type.
		inline CUtilities::MX_DATA_TYPES					DataType() const { return m_dtDataType; }

		// Gets the pre-processing method (byteswapping).
		inline CUtilities::MX_BYTESWAP						PreProcessing() const { return m_bsByteSwap; }

		// Gets the lock type.
		inline CUtilities::MX_LOCK_TYPES					LockType() const { return m_ltLockType; }

		// Sets the lock type.
		inline void											SetLockType( CUtilities::MX_LOCK_TYPES _ltType ) { m_ltLockType = _ltType; }

		// Gets the locked status.
		virtual bool										Locked() const { return m_bLocked; }

		// Sets the locked status.
		void												SetLocked( bool _bLocked );

		// Gets the item display color.
		virtual const RGBQUAD &								Color() const {
			return Locked() ? m_rgbqLockedColor : m_rgbqColor;
		}

		// Gets the base color (doesn't depend on being locked or not).
		inline const RGBQUAD &								BaseColor() const { return m_rgbqColor; }

		// Gets the locked color.
		inline const RGBQUAD &								LockedColor() const { return m_rgbqLockedColor; }

		// Sets the Pre-Processing Type.  Call within a try/catch block.
		inline void											SetPreProcessing( CUtilities::MX_BYTESWAP _bsByteSwap ) { m_bsByteSwap = _bsByteSwap; PrepareValueStructures(); }

		// Gets the actual final address of the item.
		uint64_t											FinalAddress() const {
			if MX_UNLIKELY( m_bDirtyAddress ) {
				m_bDirtyAddress = false;
				m_ui64BufferedAddress = GetUpdatedFinalAddress();
			}
			return m_ui64BufferedAddress;
		}

		// Adjusts the basic address by the given amount.  Returns the new basic address.
		uint64_t											AdjustAddress( int64_t _i64Amnt ) {
			if MX_UNLIKELY ( !_i64Amnt ) { return m_ui64Address; }
			m_bDirtyAddress = true;
			return m_ui64Address += _i64Amnt;
		}

		// Gets the array length.
		inline uint32_t										ArrayLen() const { return m_vtValueType == CUtilities::MX_VT_DATA_TYPE ? m_ui32ArrayLen : 0; }

		// Gets the array stride.
		inline uint16_t										ArrayStride() const { return m_vtValueType == CUtilities::MX_VT_DATA_TYPE ? m_ui16ArrayStride : 0; }

		// Adds a pointer level.  Call within a try/catch block.
		inline void											AddPointer( int32_t _i32Offset ) {
			m_vPointers.push_back( { .i32Offset = _i32Offset } );
			PrepareValueStructures();
		}

		// Gets the code page for strings.
		inline UINT											CodePage() const { return m_uiCodePage; }

		// Dirties the item.  Address and current data need to be updated after this.
		virtual void										Dirty() {
			m_bDirtyAddress = true;
			m_bDirtyCurValue = true;
		}

		// Is the address dynamic?
		virtual bool										SimpleAddress() const { return m_bBasicAddress; }

		// Applies the lock.  A return of false indicates the process could not be written (or potentially read if needed).
		virtual bool										ApplyLock( bool _bEvenIfNotLocked = false );

		// Saves to JSON format if _peJson is not nullptr, otherwise it saves to binary stored in _psBinary.
		virtual bool										SaveSettings( lson::CJson::LSON_ELEMENT * _peJson, CStream * _psBinary ) const;

		// Loads settings from either a JSON object or a byte buffer.
		virtual bool										LoadSettings( const lson::CJsonContainer::LSON_JSON_VALUE * _pjJson, lson::CJsonContainer * _pjcContainer, CStream * _psBinary, uint32_t _ui32Version, size_t &_sId );


	protected :
		// == Members.
		// The address expression (as text).
		std::string											m_sAddressTxt;
		// The address expression.
		mutable lsw::CExpression							m_eAddressExp;
		// The address buffered (if not 0).
		uint64_t											m_ui64Address = 0;
		// The buffered address.
		mutable uint64_t									m_ui64BufferedAddress = 0;
		// The public ID.
		uint32_t											m_ui32PublicId = 0;
		// The color of the item in the display.  If rgbReserved is 0, use the default display color.
		RGBQUAD												m_rgbqLockedColor = { .rgbBlue = 0x06, .rgbGreen = 0xEC, .rgbRed = 0x36, .rgbReserved = 0x9F };
		// The size of the data type (for m_vtValueType == CUtilities::MX_VT_DATA_TYPE).
		mutable uint32_t									m_ui32DataTypeSize = 0;
		// Array length (for m_vtValueType == CUtilities::MX_VT_DATA_TYPE).
		uint32_t											m_ui32ArrayLen = 0;
		// Array stride.
		mutable uint16_t									m_ui16ArrayStride = 0;
		// Actual array stride.
		mutable uint16_t									m_ui16ActuaArrayStride = 0;

		// The type of data type.
		CUtilities::MX_VALUE_TYPE							m_vtValueType = CUtilities::MX_VT_DATA_TYPE;
		// The type of data.
		CUtilities::MX_DATA_TYPES							m_dtDataType = CUtilities::MX_DT_VOID;
		// Codepage (for m_vtValueType == CUtilities::MX_VT_STRING).
		UINT												m_uiCodePage = CCodePages::GetSystemDefaultAnsiCodePage();
		// Byteswapping.
		CUtilities::MX_BYTESWAP								m_bsByteSwap = CUtilities::MX_BS_NONE;
		

		// The lock type.
		CUtilities::MX_LOCK_TYPES							m_ltLockType = CUtilities::MX_LT_EXACT;

		// Any pointers.
		std::vector<MX_POINTER>								m_vPointers;

		// The original data when added.
		std::vector<uint8_t>								m_vOriginalData;
		// The data when locked.
		std::vector<std::vector<uint8_t>>					m_vLockedData;
		// The data when locked (max).
		std::vector<std::vector<uint8_t>>					m_vLockedDataMax;
		// Current data.
		mutable MX_VALUE									m_vCurData;
		// The final target address for which the use of m_vCurData has been prepared.
		//mutable uint64_t									m_ui64FinalTargetPreparedAddress = 0;

		// The text form of the locked left value.
		mutable CSecureWString								m_swsLockedLeftText;

		// Whether to use a basic address or not.
		bool												m_bBasicAddress = true;
		// Locked.
		bool												m_bLocked = false;
		// Does the address need updating?
		mutable bool										m_bDirtyAddress = true;
		// Does the current value need updating?
		mutable bool										m_bDirtyCurValue = true;
		// Does the locked value need updating?
		mutable bool										m_bDirtyLockedLeft = true;
		// Is the array data contiguous?
		mutable bool										m_bContiguous = true;


		// == Functions.
		// Takes a blob of data and converts it to text.
		inline std::wstring									ToText( const std::vector<uint8_t> &_vBlob ) const { return ToText( _vBlob.data() ); }

		// Takes a blob of data and converts it to text.
		std::wstring										ToText( const uint8_t * _pui8Blob ) const;

		// Gets the actual final address of the item.
		uint64_t											GetUpdatedFinalAddress() const;

		// Undirty the current value.
		bool												UndirtyCurValue() const;

		// Prepares the MX_VALUE structures based on the value type, array length/stride.  Call within a try/catch block.
		void												PrepareValueStructures() const;

		// Updates the current-data buffer(s) without reading into them.
		//bool												PrepareCurValueBuffer() const;

		// Gets the final target address via derefencing pointers.
		uint64_t											FinalTargetAddress( bool * _pbSuccess = nullptr ) const;

		// Updates/gets the locked left text.
		const CSecureWString &								LockedLeftText() const;

		// Sets the array length.  Call within a try/catch block.
		inline void											SetArrayLen( uint32_t _ui32Len ) { m_ui32ArrayLen = std::min<uint32_t>( _ui32Len, MaxArrayLen() ); PrepareValueStructures(); }

		// Sets the array stride.  Call within a try/catch block.
		inline void											SetArrayStride( uint16_t _ui16Stride ) {
			m_ui16ArrayStride = _ui16Stride;
			PrepareValueStructures();
		}

	};

}	// namespace mx
