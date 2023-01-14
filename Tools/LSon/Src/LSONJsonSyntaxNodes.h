#pragma once


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// MACROS
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
#ifndef yy
#define yy																lson
#endif	// #ifndef yy
#ifdef YYSTYPE
#undef YYSTYPE
#endif	// #ifdef YYSTYPE
#define YYSTYPE															LSX_SYNTAX_NODES


namespace lson {

	// == Node types.
	enum LSX_NODES {
		LSX_N_ATTRIBUTE,
		LSX_N_ATTRIBUTE_LIST,
		LSX_N_ATTRIBUTE_DECL,
		LSX_N_CONTENT_DATA,
		LSX_N_CONTENT_MISC,
		LSX_N_CONTENT_ELEMENT,
		LSX_N_CONTENT,
		LSX_N_ELEMENT,
		LSX_N_MISC_SEQ,
		LSX_N_ENCODING,
		LSX_N_VERSION,
		LSX_N_PROLOG,
		LSX_N_DOCUMENT,
		LSX_N_EMPTY,
	};


	union YYSTYPE {
		size_t															sStringIndex;							/**< String index. */ 
		struct LSX_NODE {
			LSX_NODES													nType;									/**< The node type. */
			size_t														stNodeIdx;								/**< The node index. */
			union {
				struct LSX_ATTRIBUTE {
					size_t												sAttribute;								/**< The attribute string. */
					size_t												sValue;									/**< The value string or size_t( -1 ). */
				}														aAttributeObj;
				struct LSX_ATTRIBUTE_LIST {
					size_t												sLeft;									/**< The left attribute node. */
					size_t												sRight;									/**< The right attribute node. */
				}														aAttributeListObj;
				struct LSX_ATTRIBUTE_DECL {
					size_t												sName;									/**< The name of the attribute. */
					size_t												sList;									/**< The optional list of attributes. */
				}														aAttributeDeclObj;
				struct LSX_CONTENT {
					size_t												sLeft;									/**< The content left. */
					size_t												sRight;									/**< The content right. */
				}														cContentObj;
				struct LSX_ELEMENT {
					size_t												sName;									/**< The string name of the element. */
					size_t												sAttribute;								/**< The attribute node. */
					size_t												sContent;								/**< The content node. */
				}														eElementObj;
				struct LSX_MISC_SEQ {
					size_t												sLeft;									/**< The left node. */
					size_t												sRight;									/**< The right node. */
				}														msMiscSeqObj;
				struct LSX_ENCODING {
					size_t												sEncodingStr;							/**< The encoding string. */
				}														eEncodingObj;
				struct LSX_VERSION {
					size_t												sVersionStr;							/**< The version string. */
				}														vVersionObj;
				struct LSX_PROLOG {
					size_t												sVersion;								/**< The optional version node. */
					size_t												sEncoding;								/**< The optional encoding node. */
					size_t												sMiscSeq;								/**< The optional misc. sequence node. */
				}														pPrologObj;
				struct LSX_DOCUMENT {
					size_t												sProlog;								/**< The prolog node. */
					size_t												sElement;								/**< The element node. */
					size_t												sMiscSeq;								/**< The optional misc. sequence node. */
				}														dDocumentObj;
			}															u;
		}																nNode;
	};

}	// namespace lson
