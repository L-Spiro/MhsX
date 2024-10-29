#pragma once

extern "C" {

#if defined( _M_AMD64 )

// 64-bit implementation in MASM.
// Enable MASM by right clicking your project in solution explorer then:
// Build Dependencies -> Build Customizations -> MASM
extern void SinCos( double _dRadians, double * _pdSin, double * _pdCos );
// Add your assembly file as a source, but exclude it from 32 bit build.

#else

// 32-bit implementation in inline assembly.
inline void SinCos( double _dRadians, double * _pdSin, double * _pdCos ) {
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

inline void SinCosF( float _fAngle, float * _pfSin, float * _pfCos ) {
    float fSinT, fCosT;
    __asm {
        fld DWORD PTR[_fAngle]		// Load the 32-bit float into the FPU stack.
        fsincos						// Compute cosine and sine.
        fstp DWORD PTR[fCosT]		// Store the cosine value.
        fstp DWORD PTR[fSinT]		// Store the sine value.
        fwait						// Wait for the FPU to finish.
    }
    (*_pfSin) = fSinT;
    (*_pfCos) = fCosT;
}

#endif

} // extern "C"
