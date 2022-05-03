const float pointFive=0.5f;
#if 1

#pragma warning( disable : 4035 )

__int64 __cdecl _ftol(){
	__int64 result;
	__asm{
		fsub pointFive
		fistp result
		mov eax,dword ptr result
		mov edx,dword ptr result+4
	}
}
#endif

#pragma warning( default : 4035 )
