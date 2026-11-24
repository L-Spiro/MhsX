#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif // #ifndef M_PI
#ifndef M_1_PI
#define M_1_PI		0.318309886183790671538	// 1/PI.
#endif // #ifndef M_1_PI

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

/**
 * Double sine.
 */
inline double		SinD( double _fX ) {
	int64_t i32I = int64_t( _fX * M_1_PI );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - double( i32I ) * M_PI);

	double fX2 = _fX * _fX;

	return (i32I & 1) ?
		-_fX * (double( 9.99999999958141594369e-01 ) +
			fX2 * (double( -1.66666666451352446510e-01 ) +
			fX2 * (double( 8.33333301181456848061e-03 ) +
			fX2 * (double( -1.98412483604294194060e-04 ) +
			fX2 * (double( 2.75565598751558709709e-06 ) +
			fX2 * (double( -2.50368914257412183582e-08 ) +
			fX2 * (double( 1.58850007480674172780e-10 ) +
			fX2 * double( -6.58076002675210620158e-13 )))))))) :
		_fX * (double( 9.99999999958141594369e-01 ) +
			fX2 * (double( -1.66666666451352446510e-01 ) +
			fX2 * (double( 8.33333301181456848061e-03 ) +
			fX2 * (double( -1.98412483604294194060e-04 ) +
			fX2 * (double( 2.75565598751558709709e-06 ) +
			fX2 * (double( -2.50368914257412183582e-08 ) +
			fX2 * (double( 1.58850007480674172780e-10 ) +
			fX2 * double( -6.58076002675210620158e-13 ))))))));
}

/**
 * Double cosine.
 */
inline double		CosD( double _fX ) {
	int64_t i32I = int64_t( _fX * M_1_PI );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - double( i32I ) * M_PI);

	double fX2 = _fX * _fX;

	return (i32I & 1) ?
		double( -1.00000000040559489101e+00 ) -
			fX2 * (double( -4.99999998886526908493e-01 ) +
			fX2 * (double( 4.16666641590361999659e-02 ) +
			fX2 * (double( -1.38888674687691338061e-03 ) +
			fX2 * (double( 2.48006913718665254265e-05 ) +
			fX2 * (double( -2.75369918573799524591e-07 ) +
			fX2 * (double( 2.06207503915813507868e-09 ) +
			fX2 * double( -9.77507131527006491862e-12 ))))))) :
		double( 1.00000000040559489101e+00 ) +
			fX2 * (double( -4.99999998886526908493e-01 ) +
			fX2 * (double( 4.16666641590361999659e-02 ) +
			fX2 * (double( -1.38888674687691338061e-03 ) +
			fX2 * (double( 2.48006913718665254265e-05 ) +
			fX2 * (double( -2.75369918573799524591e-07 ) +
			fX2 * (double( 2.06207503915813507868e-09 ) +
			fX2 * double( -9.77507131527006491862e-12 )))))));
}

/**
 * Double sine and cosine at once.
 */
inline void			SinCosD( double _fA, double &_fSine, double &_fCosine ) {
	int64_t i32I = int64_t( _fA * M_1_PI );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fA = (_fA - double( i32I ) * M_PI);

	double fX2 = _fA * _fA;

	if ( i32I & 1 ) {
		_fSine = -_fA * (double( 9.99999999958141594369e-01 ) +
			fX2 * (double( -1.66666666451352446510e-01 ) +
			fX2 * (double( 8.33333301181456848061e-03 ) +
			fX2 * (double( -1.98412483604294194060e-04 ) +
			fX2 * (double( 2.75565598751558709709e-06 ) +
			fX2 * (double( -2.50368914257412183582e-08 ) +
			fX2 * (double( 1.58850007480674172780e-10 ) +
			fX2 * double( -6.58076002675210620158e-13 ))))))));
		_fCosine = double( -1.00000000040559489101e+00 ) -
			fX2 * (double( -4.99999998886526908493e-01 ) +
			fX2 * (double( 4.16666641590361999659e-02 ) +
			fX2 * (double( -1.38888674687691338061e-03 ) +
			fX2 * (double( 2.48006913718665254265e-05 ) +
			fX2 * (double( -2.75369918573799524591e-07 ) +
			fX2 * (double( 2.06207503915813507868e-09 ) +
			fX2 * double( -9.77507131527006491862e-12 )))))));
	}
	else {
		_fSine = _fA * (double( 9.99999999958141594369e-01 ) +
			fX2 * (double( -1.66666666451352446510e-01 ) +
			fX2 * (double( 8.33333301181456848061e-03 ) +
			fX2 * (double( -1.98412483604294194060e-04 ) +
			fX2 * (double( 2.75565598751558709709e-06 ) +
			fX2 * (double( -2.50368914257412183582e-08 ) +
			fX2 * (double( 1.58850007480674172780e-10 ) +
			fX2 * double( -6.58076002675210620158e-13 ))))))));
		_fCosine = double( 1.00000000040559489101e+00 ) +
			fX2 * (double( -4.99999998886526908493e-01 ) +
			fX2 * (double( 4.16666641590361999659e-02 ) +
			fX2 * (double( -1.38888674687691338061e-03 ) +
			fX2 * (double( 2.48006913718665254265e-05 ) +
			fX2 * (double( -2.75369918573799524591e-07 ) +
			fX2 * (double( 2.06207503915813507868e-09 ) +
			fX2 * double( -9.77507131527006491862e-12 )))))));
	}
}

/**
 * 11th-degree float sine.
 */
inline float		SinF_Medium( float _fX ) {
	int32_t i32I = int32_t( _fX * float( M_1_PI ) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * float( M_PI ));

	float fX2 = _fX * _fX;
	// Average error:
	//	0.000000071991304556528727213541666666667
	// Max error:
	//	0.000000543892383575439453125
	return (i32I & 1) ?
		-_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 )))))) :
		_fX * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
}

/**
 * 10th-degree float cosine.
 */
inline float		CosF_Medium( float _fX ) {
	int32_t i32I = int32_t( _fX * float( M_1_PI ) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fX = (_fX - float( i32I ) * float( M_PI ));

	float fX2 = _fX * _fX;

	// Average error:
	//	0.00000079472732583333333333333333333333
	// Max error:
	//	0.00000262260437
	return (i32I & 1) ?
		float( -9.99999463558197021484e-01 ) -
			fX2 * (float( -4.99995589256286621094e-01 ) +
			fX2 * (float( 4.16610352694988250732e-02 ) +
			fX2 * (float( -1.38627504929900169373e-03 ) +
			fX2 * (float( 2.42532332777045667171e-05 ) +
			fX2 * float( -2.21941789391166821588e-07 ))))) :
		float( 9.99999463558197021484e-01 ) +
			fX2 * (float( -4.99995589256286621094e-01 ) +
			fX2 * (float( 4.16610352694988250732e-02 ) +
			fX2 * (float( -1.38627504929900169373e-03 ) +
			fX2 * (float( 2.42532332777045667171e-05 ) +
			fX2 * float( -2.21941789391166821588e-07 )))));
}

/**
 * Float sine and cosine at once.
 */
inline void			SinCosF_Medium( float _fA, float &_fSine, float &_fCosine ) {
	int32_t i32I = int32_t( _fA * float( M_1_PI ) );	// 0.31830988618379067153776752674503 = 1 / PI.
	_fA = (_fA - float( i32I ) * float( M_PI ));

	float fX2 = _fA * _fA;
	if ( i32I & 1 ) {
		_fSine = -_fA * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
		_fCosine = float( -9.99999463558197021484e-01 ) -
			fX2 * (float( -4.99995589256286621094e-01 ) +
			fX2 * (float( 4.16610352694988250732e-02 ) +
			fX2 * (float( -1.38627504929900169373e-03 ) +
			fX2 * (float( 2.42532332777045667171e-05 ) +
			fX2 * float( -2.21941789391166821588e-07 )))));
	}
	else {
		_fSine = _fA * (float( 9.99999701976776123047e-01 ) +
			fX2 * (float( -1.66665777564048767090e-01 ) +
			fX2 * (float( 8.33255797624588012695e-03 ) +
			fX2 * (float( -1.98125766473822295666e-04 ) +
			fX2 * (float( 2.70405212177138309926e-06 ) +
			fX2 * float( -2.05329886426852681325e-08 ))))));
		_fCosine = float( 9.99999463558197021484e-01 ) +
			fX2 * (float( -4.99995589256286621094e-01 ) +
			fX2 * (float( 4.16610352694988250732e-02 ) +
			fX2 * (float( -1.38627504929900169373e-03 ) +
			fX2 * (float( 2.42532332777045667171e-05 ) +
			fX2 * float( -2.21941789391166821588e-07 )))));
	}
}

#if defined( __cplusplus )
} // extern "C"
#endif
