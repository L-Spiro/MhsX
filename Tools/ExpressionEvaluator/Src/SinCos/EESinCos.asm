.code

PUBLIC SinCos
; void SinCos( double _dRadians, double * _pdSin, double * _pdCos );

AngleOnStack$ = 8

SinCos PROC

  movsd QWORD PTR AngleOnStack$[rsp], xmm0	; Argument angle is in xmm0, move it to the stack.
  fld QWORD PTR AngleOnStack$[rsp]			; Push angle onto the FPU stack where we can do FLOPs.
  fsincos
  fstp QWORD PTR [r8]						; Store/pop cosine output argument.
  fstp QWORD PTR [rdx]						; Store/pop sine output argument.
  ret 0

SinCos ENDP

PUBLIC SinCosF
; void SinCosF( float _fAngle, float * _pfSin, float * _pfCos );

fStackAngle$ = 4

SinCosF PROC

	movss DWORD PTR fStackAngle$[rsp], xmm0 ; argument angle is in xmm0, move it to the stack
	fld DWORD PTR fStackAngle$[rsp]         ; push angle onto the FPU stack where we can do FLOPs
	fsincos
	fstp DWORD PTR [r8]  ; store/pop cosine output argument 
	fstp DWORD PTR [rdx] ; store/pop sine output argument
	ret 0

SinCosF ENDP

END
