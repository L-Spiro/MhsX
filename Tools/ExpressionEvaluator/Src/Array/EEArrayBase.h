#pragma once

#include "../EEExpEval.h"
#include "../EEExpEvalContainer.h"
#include <vector>

namespace ee {

	class CArrayBase {
	public :
		CArrayBase( CExpEvalContainer * _peecContainer ) :
			m_peecContainer( _peecContainer ) {
		}
		virtual										~CArrayBase() {}


		// == Functions.
		/**
		 * \brief Sets the number of elements in the array.
		 *
		 * \param _sNewSize The new size, in elements.
		 * \return Returns true if the resize succeeded; false otherwise.
		 *
		 * \note Implementations may allocate, deallocate, or otherwise reinitialize storage.
		 */
		virtual bool								SetSize( size_t _sNewSize ) = 0;

		/**
		 * \brief Gets the number of elements in the array.
		 *
		 * \return Returns the current size, in elements.
		 */
		virtual size_t								GetSize() const = 0;

		/**
		 * \brief Initializes the array using values interpolated between two expressions.
		 *
		 * \param _rStart The starting value/expression for interpolation.
		 * \param _rEnd The ending value/expression for interpolation.
		 * \return Returns true if initialization succeeded; false otherwise.
		 *
		 * \note The interpolation method and type conversion behavior are implementation-defined.
		 * \note The array must already have a defined size (via SetSize()) unless the implementation specifies otherwise.
		 */
		virtual bool								Initialize( const CExpEvalContainer::EE_RESULT &_rStart, const CExpEvalContainer::EE_RESULT &_rEnd ) = 0;

		/**
		 * \brief Reads a value from the array.
		 *
		 * \param _sIdx The zero-based element index to read.
		 * \param _rRet Receives the value stored at \p _sIdx.
		 * \return Returns true if the read succeeded; false otherwise.
		 *
		 * \note Implementations typically return false if \p _sIdx is out of range.
		 */
		virtual bool								ReadValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Writes a value to the array.
		 *
		 * \param _sIdx The zero-based element index to write.
		 * \param _rRet On input, supplies the value to write. On output, receives the actual stored value after conversion/clamping.
		 * \return Returns true if the write succeeded; false otherwise.
		 *
		 * \note The output value in \p _rRet may differ from the input if conversion, clamping, or type coercion is required.
		 * \note Implementations typically return false if \p _sIdx is out of range.
		 */
		virtual bool								WriteValue( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c += operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								PlusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c -= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								MinusEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c *= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								TimesEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c /= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								DivideEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c %= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ModEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c <<= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ShiftLeftEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c >>= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the shift amount. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								ShiftRightEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c ^= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 *
		 * \note This is named CarrotEquals() to represent the caret (^) operator.
		 */
		virtual bool								CarrotEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c |= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								OrEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;

		/**
		 * \brief Applies the \c &= operator to an element.
		 *
		 * \param _sIdx The zero-based element index to modify.
		 * \param _rRet On input, supplies the right-hand operand. On output, receives the resulting stored value.
		 * \return Returns true if the operation succeeded; false otherwise.
		 */
		virtual bool								AndEquals( size_t _sIdx, CExpEvalContainer::EE_RESULT &_rRet ) = 0;


	protected :
		// == Members.
		CExpEvalContainer *							m_peecContainer;
	};

}	// namespace ee
