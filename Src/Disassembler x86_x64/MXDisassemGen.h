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

		// Generates operand addressing flags.
		void									GenerateOperandAddressFlags();

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
			return MX_OPCODE_GROUPS( 0 );
		}

		static MX_INSTR_EXT						InstExtToEnum( const std::string &_sVal ) {
			if ( _sVal == "mmx" ) { return MX_IE_MMX; }
			if ( _sVal == "smx" ) { return MX_IE_SMX; }
			if ( _sVal == "sse1" ) { return MX_IE_SSE1; }
			if ( _sVal == "sse2" ) { return MX_IE_SSE2; }
			if ( _sVal == "sse3" ) { return MX_IE_SSE3; }
			if ( _sVal == "sse41" ) { return MX_IE_SSE41; }
			if ( _sVal == "sse42" ) { return MX_IE_SSE42; }
			if ( _sVal == "ssse3" ) { return MX_IE_SSSE3; }
			if ( _sVal == "vmx" ) { return MX_IE_VMX; }
			return MX_IE_NONE;
		}

		static MX_ADDRESS_MODES					AddressModeToEnum( const std::string &_sVal ) {
			if ( _sVal == "A" ) { return MX_AM_JMF; }
			if ( _sVal == "BA" ) { return MX_AM_BAX; }
			if ( _sVal == "BB" ) { return MX_AM_BBX; }
			if ( _sVal == "BD" ) { return MX_AM_BDX; }
			if ( _sVal == "C" ) { return MX_AM_CRX; }
			if ( _sVal == "D" ) { return MX_AM_DRX; }
			if ( _sVal == "E" ) { return MX_AM_MRG; }
			if ( _sVal == "ES" ) { return MX_AM_MF4; }
			if ( _sVal == "EST" ) { return MX_AM_RST; }
			if ( _sVal == "F" ) { return MX_AM_FLG; }
			if ( _sVal == "G" ) { return MX_AM_REG; }
			if ( _sVal == "H" ) { return MX_AM_RR4; }
			if ( _sVal == "I" ) { return MX_AM_IMM; }
			if ( _sVal == "J" ) { return MX_AM_JOW; }
			if ( _sVal == "M" ) { return MX_AM_MRM; }
			if ( _sVal == "N" ) { return MX_AM_MR8; }
			if ( _sVal == "O" ) { return MX_AM_IMA; }
			if ( _sVal == "P" ) { return MX_AM_RMX; }
			if ( _sVal == "Q" ) { return MX_AM_MRQ; }
			if ( _sVal == "R" ) { return MX_AM_MRGE; }
			if ( _sVal == "S" ) { return MX_AM_SGM; }
			if ( _sVal == "S2" ) { return MX_AM_SGM2; }
			if ( _sVal == "S30" ) { return MX_AM_SGM30; }
			if ( _sVal == "S33" ) { return MX_AM_SGM33; }
			if ( _sVal == "SC" ) { return MX_AM_STK; }
			if ( _sVal == "T" ) { return MX_AM_TRX; }
			if ( _sVal == "U" ) { return MX_AM_RMXE; }
			if ( _sVal == "V" ) { return MX_AM_RRXE; }
			if ( _sVal == "W" ) { return MX_AM_MXMM; }
			if ( _sVal == "X" ) { return MX_AM_MSI; }
			if ( _sVal == "Y" ) { return MX_AM_MDI; }
			if ( _sVal == "Z" ) { return MX_AM_RCM; }
			return MX_AM_NONE;
		}
#endif	// #ifdef _DEBUG
	};

}	// namespace mx
