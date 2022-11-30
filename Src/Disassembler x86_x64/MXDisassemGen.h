#pragma once

#include "../MXMhsX.h"
#include "MXDisassem.h"
#include <LSXml.h>
#include <LSXmlContainer.h>

namespace mx {

	class CDisassemGen {
	public :
		// == Functions.
		// Generates 

		// Loads the XML file containing all of the instructions.
		bool									LoadXml( const wchar_t * _pwcFile );

		// Prints the XML tree.
		void									PrintTree();

		// Gets the container.
		lsx::CXmlContainer *					GetContainer();

		// Generates group masks.
		void									GenerateGroupMasks();

		// Generate instruction extensions.
		void									GenerateInstExt();

		// Generates the instruction table.
		void									GenerateInstructionTable();



	protected :
		// == Types.
		// The intermediate storage structure for the loaded instruction data.
		struct MX_INSTRUCTION {
			std::string							sMnem;															// The mnemonic.
			uint16_t							ui16Prefix							= 0;						// The prefix byte.
			uint16_t							ui160FPrefix						= 0;						// The 0F prefix byte.
			uint16_t							ui16PrimaryOp						= 0;						// The primary opcode.
			uint16_t							ui16SecondaryOp						= 0;						// The secondary opcode.
			uint64_t							ui64Group							= 0;						// The opcode groups.
			MX_INSTR_EXT						ieInstExt							= MX_INSTR_EXT( 0 );		// Instruction extension.
			std::string							sBrief;															// Brief description.
			std::string							sDetailed;														// Detailed description.

			bool								bLockable							= false;					// LOCK prefix allowed?
			bool								bEscape								= false;					// 2-byte escape sequence?
		};


		// == Members.
		// The XML data.
		lsx::CXml								m_xXml;
		// The instruction table.
		std::vector<MX_INSTRUCTION>				m_vInstructions;


		// == Functions.
		// Adds an entry to the instruction table.
		void									AddEntryToTable( lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * _ptEntry, const MX_INSTRUCTION &_iInstr );

#ifdef _DEBUG
		static MX_OPCODE_GROUPS					GroupToEnum( const std::string &_sVal ) {
			if ( _sVal == "arith" ) { return MX_OG_ARITH; }
			if ( _sVal == "binary" ) { return MX_OG_BINARY; }
			if ( _sVal == "bit" ) { return MX_OG_BIT; }
			if ( _sVal == "branch" ) { return MX_OG_BRANCH; }
			if ( _sVal == "break" ) { return MX_OG_BREAK; }
			if ( _sVal == "cachect" ) { return MX_OG_CACHECT; }
			if ( _sVal == "compar" ) { return MX_OG_COMPAR; }
			if ( _sVal == "cond" ) { return MX_OG_COND; }
			if ( _sVal == "control" ) { return MX_OG_CONTROL; }
			if ( _sVal == "conver" ) { return MX_OG_CONVER; }
			if ( _sVal == "datamov" ) { return MX_OG_DATAMOV; }
			if ( _sVal == "decimal" ) { return MX_OG_DECIMAL; }
			if ( _sVal == "fetch" ) { return MX_OG_FETCH; }
			if ( _sVal == "flgctrl" ) { return MX_OG_FLGCTRL; }
			if ( _sVal == "gen" ) { return MX_OG_GEN; }
			if ( _sVal == "inout" ) { return MX_OG_INOUT; }
			if ( _sVal == "ldconst" ) { return MX_OG_LDCONST; }
			if ( _sVal == "logical" ) { return MX_OG_LOGICAL; }
			if ( _sVal == "mxcsrsm" ) { return MX_OG_MXCSRSM; }
			if ( _sVal == "obsol" ) { return MX_OG_OBSOL; }
			if ( _sVal == "order" ) { return MX_OG_ORDER; }
			if ( _sVal == "pcksclr" ) { return MX_OG_PCKSCLR; }
			if ( _sVal == "pcksp" ) { return MX_OG_PCKSP; }
			if ( _sVal == "prefix" ) { return MX_OG_PREFIX; }
			if ( _sVal == "segreg" ) { return MX_OG_SEGREG; }
			if ( _sVal == "shftrot" ) { return MX_OG_SHFTROT; }
			if ( _sVal == "shift" ) { return MX_OG_SHIFT; }
			if ( _sVal == "shunpck" ) { return MX_OG_SHUNPCK; }
			if ( _sVal == "simdfp" ) { return MX_OG_SIMDFP; }
			if ( _sVal == "simdint" ) { return MX_OG_SIMDINT; }
			if ( _sVal == "sm" ) { return MX_OG_SM; }
			if ( _sVal == "stack" ) { return MX_OG_STACK; }
			if ( _sVal == "string" ) { return MX_OG_STRING; }
			if ( _sVal == "strtxt" ) { return MX_OG_STRTXT; }
			if ( _sVal == "sync" ) { return MX_OG_SYNC; }
			if ( _sVal == "system" ) { return MX_OG_SYSTEM; }
			if ( _sVal == "trans" ) { return MX_OG_TRANS; }
			if ( _sVal == "unpack" ) { return MX_OG_UNPACK; }
			if ( _sVal == "x87fpu" ) { return MX_OG_X87FPU; }
		}

		static MX_INSTR_EXT						InstExtEnum( const std::string &_sVal ) {
			if ( _sVal == "" ) { return MX_IE_NONE; }
			if ( _sVal == "mmx" ) { return MX_IE_MMX; }
			if ( _sVal == "smx" ) { return MX_IE_SMX; }
			if ( _sVal == "sse1" ) { return MX_IE_SSE1; }
			if ( _sVal == "sse2" ) { return MX_IE_SSE2; }
			if ( _sVal == "sse3" ) { return MX_IE_SSE3; }
			if ( _sVal == "sse41" ) { return MX_IE_SSE41; }
			if ( _sVal == "sse42" ) { return MX_IE_SSE42; }
			if ( _sVal == "ssse3" ) { return MX_IE_SSSE3; }
			if ( _sVal == "vmx" ) { return MX_IE_VMX; }
		}
#endif	// #ifdef _DEBUG
	};

}	// namespace mx
