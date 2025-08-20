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
		 * Removes all children.
		 **/
		void											RemoveAllChildren() {
			m_vChildren.clear();
			m_vChildren = std::vector<EE_PTR_TYPE>();
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

		/**
		 * Sorts the children of this item.
		 * 
		 * \param _vOrder Vector of indices in the new order for the children.
		 * \return returns true if the children of this item were reanngaed into the new order specified by _vOrder.
		 **/
		bool											ArrangeChildren( const std::vector<size_t> &_vOrder ) {
			if ( _vOrder.size() != m_vChildren.size() ) { return false; }

			try {
				std::vector<EE_PTR_TYPE> vTmp;
				std::vector<bool> vSeen;
				vTmp.reserve( m_vChildren.size() );
				vSeen.resize( m_vChildren.size(), false );
				for( size_t I = 0; I < _vOrder.size(); ++I ) {
					if ( _vOrder[I] >= m_vChildren.size() ) { return false; }
					if ( vSeen[_vOrder[I]] ) { return false; }
					vTmp.emplace_back( std::move( m_vChildren[_vOrder[I]] ) );
					vSeen[_vOrder[I]] = true;
				}
				// Fix Next()/Prev() pointers.
				for ( size_t I = 0; I < vTmp.size(); ++I ) {
					if ( I == 0 ) {
						vTmp[I]->m_ptPrev = nullptr;
					}
					else {
						vTmp[I]->m_ptPrev = vTmp[I-1].get();
					}

					if ( I == _vOrder.size() - 1 ) {
						vTmp[I]->m_ptNext = nullptr;
					}
					else {
						vTmp[I]->m_ptNext = vTmp[I+1].get();
					}
				}

				// Move.
				m_vChildren = std::move( vTmp );
			}
			catch ( ... ) { return false; }
			return true;
		}

		/**
		 * Gets the next child linearly down the tree.
		 *
		 * \param _ptItem The item after this item is returned or nullptr is returned if this is the last item in the tree.
		 * \return Returns the item after the given item or nullptr.
		 */
		static CTree<_tType> *							Next( CTree<_tType> * _ptItem ) {
			if ( nullptr == _ptItem ) { return nullptr; }
			if ( _ptItem->Size() ) { return _ptItem->GetChild( 0 ); }
			if ( _ptItem->Next() ) { return _ptItem->Next(); }
			// Nothing after this.  Go to the parent.
			while ( _ptItem->Parent() ) {
				_ptItem = _ptItem->Parent();
				if ( _ptItem->Next() ) { return _ptItem->Next(); }
			}
			// That's the end.
			return nullptr;
		}

		/**
		 * Gets the previous child linearly up the tree.
		 *
		 * \param _ptItem The item before this item is returned or nullptr is returned if this is the first item in the tree.
		 * \return Returns the item after the given item or nullptr.
		 */
		static CTree<_tType> *							Prev( CTree<_tType> * _ptItem ) {
			if ( nullptr == _ptItem ) { return nullptr; }
			if ( _ptItem->Prev() ) {
				_ptItem = _ptItem->Prev();
				while ( _ptItem ) {
					if ( _ptItem->Size() ) {
						_ptItem = _ptItem->GetChild( _ptItem->Size() - 1 );
						continue;
					}
					break;
				}
				return _ptItem;
			}
			// If there is no previous, go to the parent.
			return _ptItem->Parent();
		}

		/**
		 * Moves an item one up, if possible.  It does this by swapping with the item in front of it.
		 **/
		static void										MoveUp( CTree<_tType> * _ptItem ) {
			if ( _ptItem && _ptItem->m_ptPrev ) {
				// Decouple from the link.
				auto * ptInsertAt = _ptItem->m_ptPrev;
				if ( _ptItem->Parent() == ptInsertAt->Parent() ) {
					if ( _ptItem->m_ptPrev ) {
						(*_ptItem->m_ptPrev).m_ptNext = _ptItem->m_ptNext;
					}
					if ( _ptItem->m_ptNext ) {
						(*_ptItem->m_ptNext).m_ptPrev = _ptItem->m_ptPrev;
					}

					// Insert again.
					_ptItem->m_ptPrev = ptInsertAt->m_ptPrev;
					_ptItem->m_ptNext = ptInsertAt;
					// Fix neighbors.
					if ( _ptItem->m_ptPrev ) {
						(*_ptItem->m_ptPrev).m_ptNext = _ptItem;
					}
					if ( _ptItem->m_ptNext ) {
						(*_ptItem->m_ptNext).m_ptPrev = _ptItem;
					}

					// Fix the parent array.
					if ( _ptItem->Parent() ) {
						for ( size_t I = 1; I < _ptItem->Parent()->Size(); ++I ) {
							if ( _ptItem->Parent()->m_vChildren[I].get() == _ptItem ) {
								std::swap( _ptItem->Parent()->m_vChildren[I], _ptItem->Parent()->m_vChildren[I-1] );
								break;
							}
						}
					}
				}
			}
		}

		/**
		 * Moves an item one down, if possible.  It does this by swapping with the item in front of it.
		 **/
		static void										MoveDown( CTree<_tType> * _ptItem ) {
			if ( _ptItem && _ptItem->m_ptNext ) {
				// Decouple from the link.
				auto * ptInsertAt = _ptItem->m_ptNext;
				if ( _ptItem->Parent() == ptInsertAt->Parent() ) {
					if ( _ptItem->m_ptPrev ) {
						(*_ptItem->m_ptPrev).m_ptNext = _ptItem->m_ptNext;
					}
					if ( _ptItem->m_ptNext ) {
						(*_ptItem->m_ptNext).m_ptPrev = _ptItem->m_ptPrev;
					}

					// Insert again.
					_ptItem->m_ptPrev = ptInsertAt;
					_ptItem->m_ptNext = ptInsertAt->m_ptNext;
					// Fix neighbors.
					if ( _ptItem->m_ptPrev ) {
						(*_ptItem->m_ptPrev).m_ptNext = _ptItem;
					}
					if ( _ptItem->m_ptNext ) {
						(*_ptItem->m_ptNext).m_ptPrev = _ptItem;
					}

					// Fix the parent array.
					if ( _ptItem->Parent() ) {
						for ( size_t I = 0; I < _ptItem->Parent()->Size() - 1; ++I ) {
							if ( _ptItem->Parent()->m_vChildren[I].get() == _ptItem ) {
								std::swap( _ptItem->Parent()->m_vChildren[I], _ptItem->Parent()->m_vChildren[I+1] );
								break;
							}
						}
					}
				}
			}
		}


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
