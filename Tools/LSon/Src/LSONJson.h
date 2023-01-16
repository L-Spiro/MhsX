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


	protected :
		// == Members.
		/** The container of the parsed results. */
		CJsonContainer *								m_pjcContainer;
	};

}	// namespace lson
