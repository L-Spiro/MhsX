#pragma once

#include "LSXmlContainer.h"
#include "LSXmlSyntaxNodes.h"
#include <string>
#include <vector>


namespace lsx {

	class CXmlContainer;

	/**
	 * Class CXml
	 * \brief Parses an XML file.
	 *
	 * Description: Parses an XML file.
	 */
	class CXml {
	public :
		CXml();
		~CXml();


		// == Functions.
		/**
		 * Resets the object back to its defult useable state.
		 */
		void										Reset();

		/**
		 * Sets the loaded XML data.
		 *
		 * \param _pcXml The NULL-terminated XML data.
		 * \return Returns true if the XML data was successfully parsed.
		 */
		bool										SetXml( const char * _pcXml );

		/**
		 * Gets the container pointer.
		 *
		 * \return Returns the container pointer.
		 */
		CXmlContainer *								GetContainer() { return m_pxcContainer; }


	protected :
		// == Members.
		/** The container of the parsed results. */
		CXmlContainer *								m_pxcContainer;
	};

}	// namespace lsx
