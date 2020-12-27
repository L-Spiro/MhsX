#pragma once

#include "../EEExpEval.h"

//#define EE_GEN_TABLE

namespace ee {

	class CUnicode {
	public :

		// == Enums.
		enum : uint32_t {
			EE_INVALID				= 0xFFFFFFFF
		};

		// == Types.
		struct EE_NICKS {
			const char *			pcName;
			uint32_t				uiVal;
			uint32_t				uiLen;
		};


		// == Functions.
		// Gets the number of names.
		static size_t				TotalNames();

		// Gets a name by index.
		static std::string &		GetName( std::string &_sRet, size_t _sIndex );

		// Gets a name by index.
		static std::wstring &		GetName( std::wstring &_sRet, size_t _sIndex );

		// Gets the Unicode number for a name by index.
		static uint32_t				GetUnicodeNum( size_t _sIndex ) { return _sIndex < TotalNames() ?
			m_nNames[_sIndex].uiVal :
			EE_INVALID;
		}

		// Gets the length of the name by index.
		static size_t				GetNameLength( size_t _sIndex ) { return _sIndex < TotalNames() ?
			m_nNames[_sIndex].uiLen :
			0;
		}

		// Gets the Unicode numeric given its name, case-sensitive.
		static uint32_t				GetCode( const char * _pcName, size_t _sLen = 0 );

#ifdef EE_GEN_TABLE
		static void					MakeNickNameTable();
#endif	// #if EE_GEN_TABLE



	protected :
		// == Members.
		// Nick names.
		static EE_NICKS				m_nNames[];


		// == Functions.
		// Encrypts a string.
		static void					Encrypt( const char * _pcString, size_t _sLen, std::string &_sResult );

	};
}
