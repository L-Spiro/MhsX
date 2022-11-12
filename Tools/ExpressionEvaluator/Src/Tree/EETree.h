#pragma once

#include <memory>
#include <vector>

namespace ee {

	template <typename _tType>
	class CTree {
	public :
		CTree() :
			m_tObj(),
			m_ptParent( nullptr ),
			m_ptNext( nullptr ),
			m_ptPrev( nullptr ) {
		}
		CTree( const _tType &_tCopy ) :
			m_tObj( _tCopy ),
			m_ptParent( nullptr ),
			m_ptNext( nullptr ),
			m_ptPrev( nullptr ) {
			
		}
		~CTree() {
			m_ptParent = nullptr;
		}


		// == Types.
		/** The node type. */
		typedef CTree<_tType>							EE_NODE_TYPE;

		/** The pointer to each node. */
		typedef std::unique_ptr<EE_NODE_TYPE>			EE_PTR_TYPE;


		// == Functions.
		/**
		 * Adds a child.
		 *
		 * \param _tChild The child value to insert.
		 * \param _stIdx The index where to insert the child.
		 * \return Returns true if the insertion was successful.
		 */
		bool											InsertChild( const _tType &_tChild, size_t _stIdx ) {
			try {
				auto aIt = InsertChild_It( _tChild, m_vChildren.begin() + _stIdx );
				return true;
			}
			catch ( ... ) { return false; }
		}

		std::vector<EE_PTR_TYPE>::iterator				InsertChild_It( const _tType &_tChild, std::vector<EE_PTR_TYPE>::iterator _iIt ) {
			auto aIt = m_vChildren.insert( _iIt, std::make_unique<CTree<_tType>>( _tChild ) );
			(*aIt)->m_ptParent = this;
			// Fix prev pointer.
			if ( aIt == m_vChildren.begin() ) {
				(*aIt)->m_ptPrev = nullptr;
			}
			else {
				(*aIt)->m_ptPrev = (*(aIt-1)).get();
			}
			// Fix next pointer.
			if ( aIt == m_vChildren.end() - 1 ) {
				(*aIt)->m_ptNext = nullptr;
			}
			else {
				(*aIt)->m_ptNext = (*(aIt+1)).get();
			}

			// Fix neighbors.
			if ( (*aIt)->m_ptPrev ) {
				(*(*aIt)->m_ptPrev).m_ptNext = (*aIt).get();
			}
			if ( (*aIt)->m_ptNext ) {
				(*(*aIt)->m_ptNext).m_ptPrev = (*aIt).get();
			}
			return aIt;
		}

		/**
		 * Removes a child.
		 *
		 * \param _stIdx The index where to remove the child.
		 * \return Returns true if the insertion was successful.
		 */
		bool											RemoveChild( size_t _stIdx ) {
			if ( _stIdx >= m_vChildren.size() ) { return false; }
			// Decouple from the link.
			if ( m_vChildren[_stIdx]->m_ptPrev ) {
				(*m_vChildren[_stIdx]->m_ptPrev).m_ptNext = m_vChildren[_stIdx]->m_ptNext;
			}
			if ( m_vChildren[_stIdx]->m_ptNext ) {
				(*m_vChildren[_stIdx]->m_ptNext).m_ptPrev = m_vChildren[_stIdx]->m_ptPrev;
			}
			m_vChildren.erase( m_vChildren.begin() + _stIdx );
			return true;
		}

		/**
		 * Gets a child by index.
		 *
		 * \param _stIdx The index of the item to get.
		 * \return Returns a pointer to the child to get.
		 */
		CTree<_tType> *									GetChild( size_t _stIdx ) {
			if ( _stIdx >= m_vChildren.size() ) { return nullptr; }
			return m_vChildren[_stIdx].get();
		}

		/**
		 * Gets a child by index.
		 *
		 * \param _stIdx The index of the item to get.
		 * \return Returns a pointer to the child to get.
		 */
		const CTree<_tType> *							GetChild( size_t _stIdx ) const {
			if ( _stIdx >= m_vChildren.size() ) { return nullptr; }
			return m_vChildren[_stIdx].get();
		}

		/**
		 * Gets the value of this object.
		 *
		 * \return Returns a reference to the value of this node.
		 */
		_tType &										Value() { return m_tObj; }

		/**
		 * Gets the value of this object.
		 *
		 * \return Returns a reference to the value of this node.
		 */
		const _tType &									Value() const { return m_tObj; }

		/**
		 * Gets a pointer to this node's parent.
		 *
		 * \return Returns a pointer to the parent node.
		 */
		CTree<_tType> *									Parent() { return m_ptParent; }

		/**
		 * Gets a pointer to this node's parent.
		 *
		 * \return Returns a pointer to the parent node.
		 */
		const CTree<_tType> *							Parent() const { return m_ptParent; }

		/**
		 * Gets a pointer to this node's next.
		 *
		 * \return Returns a pointer to the next node.
		 */
		CTree<_tType> *									Next() { return m_ptNext; }

		/**
		 * Gets a pointer to this node's next.
		 *
		 * \return Returns a pointer to the next node.
		 */
		const CTree<_tType> *							Next() const { return m_ptNext; }

		/**
		 * Gets a pointer to this node's previous.
		 *
		 * \return Returns a pointer to the previous node.
		 */
		CTree<_tType> *									Prev() { return m_ptPrev; }

		/**
		 * Gets a pointer to this node's previous.
		 *
		 * \return Returns a pointer to the previous node.
		 */
		const CTree<_tType> *							Prev() const { return m_ptPrev; }

		/**
		 * Gets the total numbr of children on this node.
		 *
		 * \return Returns the total numbr of children on this node.
		 */
		size_t											Size() const { return m_vChildren.size(); }


	protected :
		// == Members.
		/** The parent. */
		CTree<_tType> *									m_ptParent;
		/** The next sibling. */
		CTree<_tType> *									m_ptNext;
		/** The previous sibling. */
		CTree<_tType> *									m_ptPrev;
		/** Children. */
		std::vector<EE_PTR_TYPE>						m_vChildren;
		/** The object of the tree. */
		_tType											m_tObj;
	};

}	// namespace ee
