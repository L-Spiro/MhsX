#pragma once

#include "LSONJsonContainer.h"
#include "LSONJsonSyntaxNodes.h"
#include <string>
#include <vector>


namespace lson {

	class CJsonContainer;

	/**
	 * Class CJson
	 * \brief Parses an XML file.
	 *
	 * Description: Parses an XML file.
	 */
	class CJson {
	public :
		CJson();
		~CJson();


		// == Functions.
		/**
		 * Resets the object back to its defult useable state.
		 */
		void											Reset();

		/**
		 * Sets the loaded XML data.
		 *
		 * \param _pcXml The NULL-terminated XML data.
		 * \return Returns true if the XML data was successfully parsed.
		 */
		bool											SetXml( const char * _pcXml );

		/**
		 * Gets the container pointer.
		 *
		 * \return Returns the container pointer.
		 */
		CJsonContainer *								GetContainer() { return m_pxcContainer; }


	protected :
		// == Members.
		/** The container of the parsed results. */
		CJsonContainer *								m_pxcContainer;
	};

}	// namespace lson
