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

			sResult = "static MX_INSTR_EXT						InstExtEnum( const std::string &_sVal ) {\r\n";
			sResult += "\tif ( _sVal == \"\" ) { return MX_IE_NONE; }\r\n";
			for ( auto I = aGrp.begin(); I != aGrp.end(); ++I ) {
				sResult += std::format( "\tif ( _sVal == \"{1}\" ) {{ return MX_IE_{0}; }}\r\n", CUtilities::ToUpper( (*I) ), (*I) );
			}

			sResult += "}\r\n";
			::OutputDebugStringA( sResult.c_str() );
		}
#endif	// #ifdef _DEBUG
	}

	// Generates the instruction table.
	void CDisassemGen::GenerateInstructionTable() {
#ifdef _DEBUG
		if ( GetContainer() ) {
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
		}
		return;
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
			iInst.ieInstExt = InstExtEnum( sValue );
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
