#pragma once

#include "../MXMhsX.h"
#include "../Strings/MXSecureWString.h"
#include "MXHexEditorInterface.h"
#include "MXHexFind.h"

#include <algorithm>
#include <memory>
#include <vector>


namespace mx {

	/**
	 * The results of the Hex Editor's Find All searches.  Each search is a set of matches in one file; with Allow Multiple Find
	 *	Ranges the sets of several searches are kept, otherwise each search replaces the last.
	 **/
	class CFindResults {
	public :
		// == Types.
		/** The results of one Find All. */
		struct MX_FIND_SET {
			CSecureWString											swsQuery;										/**< What was searched for (as typed). */
			CHexFind::MX_FIND_TYPE									ftType = CHexFind::MX_FT_HEX;					/**< What it was (after a type specifier). */
			std::weak_ptr<CHexEditorInterface>						wpSource;										/**< What was searched. */
			CSecureWString											swsSourceName;									/**< The name of what was searched. */
			uint64_t												ui64RangeStart = 0;								/**< The start of the range searched. */
			uint64_t												ui64RangeEnd = 0;								/**< The end of the range searched. */
			bool													bCustomColor = false;							/**< The matches use crFore and crBack. */
			bool													bCustomFore = false;							/**< crFore is used. */
			COLORREF												crFore = 0;										/**< The custom text color. */
			COLORREF												crBack = 0;										/**< The custom background color. */
			bool													bTooMany = false;								/**< The search stopped at the limit. */
			std::vector<CHexFind::MX_FIND_MATCH>					vMatches;										/**< The matches, in order. */
		};


		// == Functions.
		/**
		 * Adds the results of a search.
		 *
		 * \param _fsSet The results (moved into the list).
		 * \param _bKeepOthers If false, the results of earlier searches are removed.
		 **/
		void														Add( MX_FIND_SET &&_fsSet, bool _bKeepOthers ) {
			if ( !_bKeepOthers ) { m_vSets.clear(); }
			m_vSets.push_back( std::move( _fsSet ) );
			++m_ui64Revision;
		}

		/**
		 * Removes every result.
		 **/
		void														Clear() {
			m_vSets.clear();
			++m_ui64Revision;
		}

		/**
		 * Removes the results of searches in a data source (when it is closed).
		 *
		 * \param _pheiSource The data source.
		 **/
		void														RemoveSource( const CHexEditorInterface * _pheiSource ) {
			const size_t sBefore = m_vSets.size();
			m_vSets.erase( std::remove_if( m_vSets.begin(), m_vSets.end(), [&]( const MX_FIND_SET &_fsSet ) {
				auto spSource = _fsSet.wpSource.lock();
				return !spSource || spSource.get() == _pheiSource;
			} ), m_vSets.end() );
			if ( m_vSets.size() != sBefore ) { ++m_ui64Revision; }
		}

		/**
		 * Gets the sets of results, oldest first.
		 *
		 * \return Returns the sets.
		 **/
		inline const std::vector<MX_FIND_SET> &						Sets() const { return m_vSets; }

		/**
		 * Gets a number that changes whenever the results change (so that a view of them knows when to update).
		 *
		 * \return Returns the revision number.
		 **/
		inline uint64_t												Revision() const { return m_ui64Revision; }


	protected :
		// == Members.
		/** The sets of results. */
		std::vector<MX_FIND_SET>									m_vSets;
		/** Changes whenever the results change. */
		uint64_t													m_ui64Revision = 0;
	};

}	// namespace mx
