#pragma once

#include <cmath>

#if defined( __cplusplus )
extern "C" {
#endif

#if defined( _MSC_VER ) && defined( _M_AMD64 )

/**
 * \brief Computes the sine and cosine of an angle in radians.
 *
 * MSVC x64 builds use a MASM implementation.
 *
 * \param _dRadians Angle in radians.
 * \param _pdSin Receives sin(_dRadians). Must be non-null.
 * \param _pdCos Receives cos(_dRadians). Must be non-null.
 */
extern void 		SinCos( double _dRadians, double * _pdSin, double * _pdCos );

#elif defined( _MSC_VER ) && defined( _M_IX86 )

/**
 * \brief Computes the sine and cosine of an angle in radians.
 *
 * MSVC x86 builds use inline x87 assembly (FSINCOS).
 *
 * \param _dRadians Angle in radians.
 * \param _pdSin Receives sin(_dRadians). Must be non-null.
 * \param _pdCos Receives cos(_dRadians). Must be non-null.
 */
inline void 		SinCos( double _dRadians, double * _pdSin, double * _pdCos ) {
	double dSin, dCos;
	__asm {
		fld QWORD PTR[_dRadians]
		fsincos
		fstp QWORD PTR[dCos]
		fstp QWORD PTR[dSin]
		fwait
	}
	(*_pdSin) = dSin;
	(*_pdCos) = dCos;
}

/**
 * \brief Computes the sine and cosine of an angle in radians (single-precision).
 *
 * MSVC x86 builds use inline x87 assembly (FSINCOS).
 *
 * \param _fAngle Angle in radians.
 * \param _pfSin Receives sin(_fAngle). Must be non-null.
 * \param _pfCos Receives cos(_fAngle). Must be non-null.
 */
inline void 		SinCosF( float _fAngle, float * _pfSin, float * _pfCos ) {
	float fSinT, fCosT;
	__asm {
		fld DWORD PTR[_fAngle]
		fsincos
		fstp DWORD PTR[fCosT]
		fstp DWORD PTR[fSinT]
		fwait
	}
	(*_pfSin) = fSinT;
	(*_pfCos) = fCosT;
}

#else

/**
 * \brief Computes the sine and cosine of an angle in radians.
 *
 * On Clang/GCC (including Xcode), attempts to call the platform/libm combined
 * implementation when available; otherwise falls back to std::sin/std::cos.
 *
 * \param _dRadians Angle in radians.
 * \param _pdSin Receives sin(_dRadians). Must be non-null.
 * \param _pdCos Receives cos(_dRadians). Must be non-null.
 */
inline void 		SinCos( double _dRadians, double * _pdSin, double * _pdCos ) {
#if defined( __APPLE__ ) || defined( __GNUC__ ) || defined( __clang__ )
	::__sincos( _dRadians, _pdSin, _pdCos );
#else
	(*_pdSin) = std::sin( _dRadians );
	(*_pdCos) = std::cos( _dRadians );
#endif
}

/**
 * \brief Computes the sine and cosine of an angle in radians (single-precision).
 *
 * On Clang/GCC (including Xcode), attempts to call the platform/libm combined
 * implementation when available; otherwise falls back to std::sinf/std::cosf.
 *
 * \param _fAngle Angle in radians.
 * \param _pfSin Receives sin(_fAngle). Must be non-null.
 * \param _pfCos Receives cos(_fAngle). Must be non-null.
 */
inline void 		SinCosF( float _fAngle, float * _pfSin, float * _pfCos ) {
#if defined( __APPLE__ ) || defined( __GNUC__ ) || defined( __clang__ )
	::__sincosf( _fAngle, _pfSin, _pfCos );
#else
	(*_pfSin) = std::sinf( _fAngle );
	(*_pfCos) = std::cosf( _fAngle );
#endif
}

#endif

#if defined( __cplusplus )
} // extern "C"
#endif
