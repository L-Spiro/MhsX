#include "MXDisassemGen.h"
#include "../Files/MXFile.h"
#include "../Utilities/MXUtilities.h"
#include <LSWWin.h>

#include <format>


namespace mx {

	// Loads the XML file containing all of the instructions.
	bool CDisassemGen::LoadXml( const wchar_t * _pwcFile ) {
		std::vector<uint8_t> vBytes;
		CFile::LoadToMemory( _pwcFile, vBytes );
		vBytes.push_back( 0 );

		return m_xXml.SetXml( (const char *)(vBytes.data()) );
	}

	// Prints the XML tree.
	void CDisassemGen::PrintTree() {
		if ( m_xXml.GetContainer() ) {
			m_xXml.GetContainer()->PrintTree();
		}
	}

	// Gets the container.
	lsx::CXmlContainer * CDisassemGen::GetContainer() {
		return m_xXml.GetContainer();
	}

	// Generates group masks.
	void CDisassemGen::GenerateGroupMasks() {
#ifdef _DEBUG
		if ( GetContainer() ) {
			std::map<std::string, std::set<std::string>> mMap;
			GetContainer()->GatherElements( mMap );

			std::set<std::string> sMerged;
			auto aGrp = mMap["grp1"];
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sMerged.insert( (*I) );
			}
			aGrp = mMap["grp2"];
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sMerged.insert( (*I) );
			}
			aGrp = mMap["grp3"];
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sMerged.insert( (*I) );
			}

			size_t J = 0;
			std::string sResult;
			for ( auto I = sMerged.begin(); I != sMerged.end(); ++I ) {
				sResult += std::format( "MX_OG_{0:<13} = (1ULL << {1}ULL),\r\n", CUtilities::ToUpper( (*I) ), J++ );
			}
			
			::OutputDebugStringA( sResult.c_str() );

			sResult = "static MX_OPCODE_GROUPS					GroupToEnum( const std::string &_sVal ) {\r\n";

			for ( auto I = sMerged.begin(); I != sMerged.end(); ++I ) {
				sResult += std::format( "\tif ( _sVal == \"{1}\" ) {{ return MX_OG_{0}; }}\r\n", CUtilities::ToUpper( (*I) ), (*I) );
			}
			sResult += "\treturn MX_OPCODE_GROUPS( 0 );\r\n";
			sResult += "}\r\n";
			::OutputDebugStringA( sResult.c_str() );
		}
#endif	// #ifdef _DEBUG
	}

	// Generate instruction extensions.
	void CDisassemGen::GenerateInstExt() {
#ifdef _DEBUG
		if ( GetContainer() ) {
			std::map<std::string, std::set<std::string>> mMap;
			GetContainer()->GatherElements( mMap );
			auto aGrp = mMap["instr_ext"];
			std::string sResult;
			sResult += "MX_IE_NONE,\r\n";
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sResult += std::format( "MX_IE_{0},\r\n", CUtilities::ToUpper( (*I) ) );
			}
			
			::OutputDebugStringA( sResult.c_str() );

			sResult = "static MX_INSTR_EXT						InstExtToEnum( const std::string &_sVal ) {\r\n";
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sResult += std::format( "\tif ( _sVal == \"{1}\" ) {{ return MX_IE_{0}; }}\r\n", CUtilities::ToUpper( (*I) ), (*I) );
			}
			sResult += "\treturn MX_IE_NONE;\r\n";
			sResult += "}\r\n";
			::OutputDebugStringA( sResult.c_str() );
		}
#endif	// #ifdef _DEBUG
	}

	// Generates operand addressing flags.
	void CDisassemGen::GenerateOperandAddressFlags() {
#ifdef _DEBUG
		if ( GetContainer() ) {
			std::map<std::string, std::set<std::string>> mAddr, mAttri;
			GetContainer()->GatherElements( mAddr );
			GetContainer()->GatherAttributes( mAttri );
			struct MX_REPLACEMENTS {
				const char *	pcSrc;
				const char *	pcDst;
			};
			static const MX_REPLACEMENTS rReplacements[] = {
				{ "A",	"JMF" },				// Direct address. The instruction has no ModR/M byte; the address of the operand is encoded in the instruction; no base register, index register, or scaling factor can be applied (for example, far JMP (EA)).
				{ "BA",	"BAX" },				// Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 MONITOR).
				{ "BB",	"BBX" },				// Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT). (This code changed from single B in revision 1.00).
				{ "BD",	"BDX" },				// Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 MASKMOVDQU) (This code changed from YD (introduced in 1.00) in revision 1.02).
				{ "C",	"CRX" },				// The reg field of the ModR/M byte selects a control register (only MOV (0F20, 0F22)).
				{ "D",	"DRX" },				// The reg field of the ModR/M byte selects a debug register (only MOV (0F21, 0F23)).
				{ "E",	"MRG" },				// A ModR/M byte follows the opcode and specifies the operand. The operand is either a general-purpose register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, or a displacement.
				{ "ES",	"MF4" },/*?*/			// (Implies original E). A ModR/M byte follows the opcode and specifies the operand. The operand is either a x87 FPU stack register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, or a displacement.
				{ "EST","RST" },				// (Implies original E). A ModR/M byte follows the opcode and specifies the x87 FPU stack register.
				{ "F",	"FLG" },				// rFLAGS register.
				{ "G",	"REG" },				// The reg field of the ModR/M byte selects a general register (for example, AX (000)).
				{ "H",	"RR4" },				// The r/m field of the ModR/M byte always selects a general register, regardless of the mod field (for example, MOV (0F20)).
				{ "I",	"IMM" },				// Immediate data. The operand value is encoded in subsequent bytes of the instruction.
				{ "J",	"JOW" },				// The instruction contains a relative offset to be added to the instruction pointer register (for example, JMP (E9), LOOP)).
				{ "M",	"MRM" },				// The ModR/M byte may refer only to memory: mod != 11bin (BOUND, LEA, CALLF, JMPF, LES, LDS, LSS, LFS, LGS, CMPXCHG8B, CMPXCHG16B, F20FF0 LDDQU).
				{ "N",	"MR8" },				// The R/M field of the ModR/M byte selects a packed quadword MMX technology register.
				{ "O",	"IMA" },				// The instruction has no ModR/M byte; the offset of the operand is coded as a word, double word or quad word (depending on address size attribute) in the instruction. No base register, index register, or scaling factor can be applied (only MOV  (A0, A1, A2, A3)).
				{ "P",	"RMX" },				// The reg field of the ModR/M byte selects a packed quadword MMX technology register.
				{ "Q",	"MRQ" },				// A ModR/M byte follows the opcode and specifies the operand. The operand is either an MMX technology register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement.
				{ "R",	"MRGE" },				// The mod field of the ModR/M byte may refer only to a general register (only MOV (0F20-0F24, 0F26)).
				{ "S",	"SGM" },				// The reg field of the ModR/M byte selects a segment register (only MOV (8C, 8E)).
				{ "S2",	"SGM2" },				// The two bits at bit index three of the opcode byte selects one of original four segment registers (for example, PUSH ES).
				{ "S30","SGM30" },				// The three least-significant bits of the opcode byte selects segment register SS, FS, or GS (for example, LSS).
				{ "S33","SGM33" },				// The three bits at bit index three of the opcode byte selects segment register FS or GS (for example, PUSH FS).
				{ "SC",	"STK" },				// Stack operand, used by instructions which either push an operand to the stack or pop an operand from the stack. Pop-like instructions are, for example, POP, RET, IRET, LEAVE. Push-like are, for example, PUSH, CALL, INT. No Operand type is provided along with this method because it depends on source/destination operand(s).
				{ "T",	"TRX" },				// The reg field of the ModR/M byte selects a test register (only MOV (0F24, 0F26)).
				{ "U",	"RMXE" },				// The R/M field of the ModR/M byte selects a 128-bit XMM register.
				{ "V",	"RRXE" },				// The reg field of the ModR/M byte selects a 128-bit XMM register.
				{ "W",	"MXMM" },				// A ModR/M byte follows the opcode and specifies the operand. The operand is either a 128-bit XMM register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement.
				{ "X",	"MSI" },				// Memory addressed by the DS:eSI or by RSI (only MOVS, CMPS, OUTS, and LODS). In 64-bit mode, only 64-bit (RSI) and 32-bit (ESI) address sizes are supported. In non-64-bit modes, only 32-bit (ESI) and 16-bit (SI) address sizes are supported.
				{ "Y",	"MDI" },				// Memory addressed by the ES:eDI or by RDI (only MOVS, CMPS, INS, STOS, and SCAS). In 64-bit mode, only 64-bit (RDI) and 32-bit (EDI) address sizes are supported. In non-64-bit modes, only 32-bit (EDI) and 16-bit (DI) address sizes are supported. The implicit ES segment register cannot be overriden by a segment prefix.
				{ "Z",	"RCM" },				// The instruction has no ModR/M byte; the three least-significant bits of the opcode byte selects a general-purpose register.
			};
			auto lReplace = [&]( const std::string &_sReplaceMe ) {
				for ( size_t J = std::size( rReplacements ); J--; ) {
					if ( _sReplaceMe == rReplacements[J].pcSrc ) { return rReplacements[J].pcDst; }
				}
				return "";
			};

			auto aGrp = mAddr["a"];
			auto aAddMe = mAttri["address"];
			for ( auto I = aAddMe.begin(); I != aAddMe.end(); ++I ) {
				aGrp.insert( (*I) );
			}
			std::string sResult;
			sResult += "MX_AM_NONE,\r\n";
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sResult += std::format( "MX_AM_{0},\r\n", CUtilities::ToUpper( lReplace( (*I) ) ) );
			}
			
			::OutputDebugStringA( sResult.c_str() );

			sResult = "static MX_ADDRESS_MODES					AddressModeToEnum( const std::string &_sVal ) {\r\n";
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sResult += std::format( "\tif ( _sVal == \"{1}\" ) {{ return MX_AM_{0}; }}\r\n", CUtilities::ToUpper( lReplace( (*I) ) ), (*I) );
			}
			sResult += "\treturn MX_AM_NONE;\r\n";
			sResult += "}\r\n";
			::OutputDebugStringA( sResult.c_str() );
		}
#endif	// #ifdef _DEBUG
	}

	// Generates the instruction table.
	void CDisassemGen::GenerateInstructionTable() {
#ifdef _DEBUG
		if ( GetContainer() ) {
			std::map<std::string, std::set<std::string>> mAddr, mAttri;
			GetContainer()->GatherElements( mAddr );
			GetContainer()->GatherAttributes( mAttri );

			lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptParent = GetContainer()->Next( nullptr );
			std::vector<size_t> vByte = GetContainer()->GatherChildElementIndices( ptParent, "one-byte" );
			for ( size_t I = 0; I < vByte.size(); ++I ) {
				lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptThis = ptParent->GetChild( vByte[I] );
				std::vector<size_t> vPrimary = GetContainer()->GatherChildElementIndices( ptThis, "pri_opcd" );
				for ( size_t J = 0; J < vPrimary.size(); ++J ) {
					lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptOp = ptThis->GetChild( vPrimary[J] );
					std::string sValue;
					MX_INSTRUCTION iInst;
					if ( GetContainer()->GetAttributeValue( ptOp, "value", sValue ) ) {
						iInst.ui16PrimaryOp = std::stoull( sValue.c_str(), nullptr, 16 );

						std::vector<size_t> vEntry = GetContainer()->GatherChildElementIndices( ptOp, "entry" );
						for ( size_t K = 0; K < vEntry.size(); ++K ) {
							lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptEntry = ptOp->GetChild( vEntry[K] );
							AddEntryToTable( ptEntry, iInst );
						}
					}
				}
			}


			//ptParent = GetContainer()->Next( ptParent );
			vByte = GetContainer()->GatherChildElementIndices( ptParent, "two-byte" );
			for ( size_t I = 0; I < vByte.size(); ++I ) {
				lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptThis = ptParent->GetChild( vByte[I] );
				std::vector<size_t> vPrimary = GetContainer()->GatherChildElementIndices( ptThis, "pri_opcd" );
				for ( size_t J = 0; J < vPrimary.size(); ++J ) {
					lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptOp = ptThis->GetChild( vPrimary[J] );
					std::string sValue;
					MX_INSTRUCTION iInst;
					if ( GetContainer()->GetAttributeValue( ptOp, "value", sValue ) ) {
						iInst.ui16PrimaryOp = std::stoull( sValue.c_str(), nullptr, 16 );
						iInst.ui160FPrefix = 0x0F;
						std::vector<size_t> vEntry = GetContainer()->GatherChildElementIndices( ptOp, "entry" );
						for ( size_t K = 0; K < vEntry.size(); ++K ) {
							lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptEntry = ptOp->GetChild( vEntry[K] );
							AddEntryToTable( ptEntry, iInst );
						}
					}
				}
			}
			return;
		}
#endif	// #ifdef _DEBUG
	}

	// Adds an entry to the instruction table.
	void CDisassemGen::AddEntryToTable( lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * _ptEntry, const MX_INSTRUCTION &_iInstr ) {
#ifdef _DEBUG
		MX_INSTRUCTION iInst = _iInstr;

		std::string sValue;
		{
			lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptSyntax = GetContainer()->GetChildElement( _ptEntry, "syntax" );
			if ( ptSyntax ) {
				lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptTmp = GetContainer()->GetChildElement( ptSyntax, "mnem" );
				if ( ptTmp ) {
					iInst.sMnem = ptTmp->Value().sData;
				}
			}
		}
		{
			lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptP = GetContainer()->GetChildElement( _ptEntry, "note" );
			if ( ptP ) {
				lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptC = GetContainer()->GetChildElement( ptP, "brief" );
				if ( ptC ) {
					iInst.sBrief = ptC->Value().sData;
				}
				ptC = GetContainer()->GetChildElement( ptP, "det" );
				if ( ptC ) {
					iInst.sDetailed = ptC->Value().sData;
				}
			}
		}

		if ( GetContainer()->GetChildElementData( _ptEntry, "pref", sValue ) ) {
			iInst.ui16Prefix = std::stoull( sValue.c_str(), nullptr, 16 );
		}
		if ( GetContainer()->GetChildElementData( _ptEntry, "grp1", sValue ) ) {
			iInst.ui64Group |= uint64_t( GroupToEnum( sValue ) );
		}
		if ( GetContainer()->GetChildElementData( _ptEntry, "grp2", sValue ) ) {
			iInst.ui64Group |= uint64_t( GroupToEnum( sValue ) );
		}
		if ( GetContainer()->GetChildElementData( _ptEntry, "grp3", sValue ) ) {
			iInst.ui64Group |= uint64_t( GroupToEnum( sValue ) );
		}
		if ( GetContainer()->GetChildElementData( _ptEntry, "instr_ext", sValue ) ) {
			iInst.ieInstExt = InstExtToEnum( sValue );
		}
		{
			lsx::CTree<lsx::CXmlContainer::LSX_XML_ELEMENT> * ptSecondary = GetContainer()->GetChildElement( _ptEntry, "sec_opcd" );
			if ( ptSecondary ) {
				iInst.ui16SecondaryOp = std::stoull( ptSecondary->Value().sData.c_str(), nullptr, 16 );
				if ( GetContainer()->GetAttributeValue( ptSecondary, "escape", sValue ) ) {
					iInst.bEscape = sValue == "yes" ? true : false;
					if ( iInst.bEscape ) {
						iInst.ui160FPrefix = 0x0F;
					}
				}
			}
		}
		


		m_vInstructions.push_back( iInst );
#endif	// #ifdef _DEBUG
	}

}	// namespace mx
