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

END
