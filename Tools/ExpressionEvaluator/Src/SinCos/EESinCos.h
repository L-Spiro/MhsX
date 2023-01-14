#pragma once

extern "C" {

#if defined(_M_AMD64)

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

#endif

} // extern "C"
