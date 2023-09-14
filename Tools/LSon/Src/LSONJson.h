#pragma once

#include "LSONJsonContainer.h"
#include "LSONJsonSyntaxNodes.h"
#include <string>
#include <vector>


namespace lson {

	class CJsonContainer;

	/**
	 * Class CJson
	 * \brief Parses an JSON file.
	 *
	 * Description: Parses an JSON file.
	 */
	class CJson {
	public :
		CJson();
		~CJson();


		// == Enumerations.
		/** The node types. */
		enum LSON_NODE_TYPE {
			LSON_OBJECT,								/**< An object. */
			LSON_ARRAY,									/**< An array. */
			LSON_STRING,								/**< A string. */
			LSON_NUMBER,								/**< A number. */
			LSON_TRUE,									/**< "true". */
			LSON_FALSE,									/**< "false". */
			LSON_NULL,									/**< "null". */
		};


		// == Types.
		/** An element. */
		struct LSON_ELEMENT {
			LSON_NODE_TYPE								ntType;								/**< The element type. */
			std::string									sName;								/**< The name of the element. */
			std::vector<LSON_ELEMENT>					vObjectMembers;						/**< If ntType is LSON_OBJECT. */
			std::vector<LSON_ELEMENT>					vArray;								/**< If ntType is LSON_ARRAY. */
			std::string									sString;							/**< If ntType is LSON_STRING. */
			double										dNumber;							/**< If ntType is LSON_NUMBER. */
		};


		// == Functions.
		/**
		 * Resets the object back to its defult useable state.
		 */
		void											Reset();

		/**
		 * Sets the loaded JSON data.
		 *
		 * \param _pcJson The NULL-terminated JSON data.
		 * \return Returns true if the JSON data was successfully parsed.
		 */
		bool											SetJson( const char * _pcJson );

		/**
		 * Gets the container pointer.
		 *
		 * \return Returns the container pointer.
		 */
		CJsonContainer *								GetContainer() { return m_pjcContainer; }

		/**
		 * Creates an object JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _eElement The return structure.
		 **/
		static void										CreateObjectElement( const std::string &_sName, LSON_ELEMENT &_eElement );

		/**
		 * Creates an array JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _eElement The return structure.
		 **/
		static void										CreateArrayElement( const std::string &_sName, LSON_ELEMENT &_eElement );

		/**
		 * Creates a string JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _sString The string.
		 * \param _eElement The return structure.
		 **/
		static void										CreateStringElement( const std::string &_sName, const std::string &_sString, LSON_ELEMENT &_eElement );

		/**
		 * Creates a number JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _dNumber The number.
		 * \param _eElement The return structure.
		 **/
		static void										CreateNumberElement( const std::string &_sName, double _dNumber, LSON_ELEMENT &_eElement );

		/**
		 * Creates a true JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _eElement The return structure.
		 **/
		static void										CreateTrueElement( const std::string &_sName, LSON_ELEMENT &_eElement );

		/**
		 * Creates a false JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _eElement The return structure.
		 **/
		static void										CreateFalseElement( const std::string &_sName, LSON_ELEMENT &_eElement );

		/**
		 * Creates a null JSON element.
		 * 
		 * \param _sName The name of the element.
		 * \param _eElement The return structure.
		 **/
		static void										CreateNullElement( const std::string &_sName, LSON_ELEMENT &_eElement );


	protected :
		// == Members.
		/** The container of the parsed results. */
		CJsonContainer *								m_pjcContainer;
	};

}	// namespace lson
