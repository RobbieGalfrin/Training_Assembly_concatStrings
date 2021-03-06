// constrs1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

char enterstr1[] = "Enter 1st string, less than 32 chars\n";
char enterstr2[] = "Enter 2nd string, less than 32 chars\n";
char putstr[] = "%s";
char strdisp[] = "The concatenated string is: %s";



__declspec(naked) int  _CheckLen(char* mystr) {
	_asm {
		; receives &str
		; returns length of string
		push    ebp
		mov     ebp, esp
		xor     eax, eax; eax = 0
		mov		edi, 8[ebp]; 8(ebp)contains the address of the string
		mov		cl, 20h
		repne	scasb; scasb compares AL with *(edi)
		; repne repeats scasb until cl is 0 or *(edi) = al
		jcxz 	_lenlarge; if cl reached 0, max_len was exeeded
		sub		al, cl; len = 20 - cl-- > stored in al
		add		al, 1Fh
		pop     ebp
		ret
		_lenlarge :
		sub		eax, 01h; return -1
		pop     ebp
		ret
	}
}

__declspec(naked) void _ConcatStrs(int str_len1, int str_len2, char* concatstr, char* str1, char* str2) {
	_asm {
		; receives str1_len, str2_len, &str1, &str2, &concatenated_str
		; fills concatenated_str
		push    ebp
		mov     ebp, esp
		mov		eax, 14h[ebp]; eax = str1_len
		mov     edi, 10h[ebp]; edi = &concatstr
		mov     esi, 08h[ebp]; esi = &str1
		mov		ecx, eax; ecx = str1_len
		rep		movs DWORD PTR ES : [EDI], DWORD PTR[ESI]; copy str1 to concat_str
		mov		ecx, 18h[ebp]; ecx = str2_len
		mov		edi, 10h[ebp]; edi = &concatstr
		add		edi,eax; edi = address of place in concatenated str after str1 is added
		mov     esi, 0Ch[ebp]; esi = &str2
		rep		movs DWORD PTR ES : [EDI], DWORD PTR[ESI]; copy str2 to concat_str
		pop     ebp
		ret
	}
}


__declspec(naked) int main() {
	_asm {
		push    ebp
		mov     ebp, esp
		sub		esp, 80h; Allocate space for two stings(20h max len each) and conatenated string on stack
		sub		esp, 8; space for two string length paremeters
		_rescanf1 :
		push    OFFSET enterstr1; 'Enter 1st string, less than 32 chars\n'
		call	printf
		add		esp, 04h; clean stack
		lea     eax, [ebp - 80h]; pointer to str1
		push    eax
		push    OFFSET putstr
		call	scanf
		add		esp, 8
		lea     eax, [ebp - 80h]; pointer to str1
		push	eax
		call    _CheckLen
		add		esp, 4; clean stack
		cmp     eax, -1; check if length is valid
		jz      _rescanf1; if string not valid - retry
		mov[ebp - 88h], eax; save length of str1
		_rescanf2 :
		push    OFFSET enterstr2; 'Enter 2nd string, less than 32 chars\n'
		call	printf
		add		esp, 4
		lea     eax, DWORD PTR[ebp - 60h]; pointer to str2
		push    eax
		push    OFFSET putstr
		call	scanf
		add		esp, 8
		lea     eax, DWORD PTR[ebp - 60h]; pointer to str2
		push	eax
		call    _CheckLen
		add		esp, 4
		cmp     eax, -1; check if length is valid
		jz      _rescanf2; if string not valid - retry
		mov		[ebp - 84h], eax; save length of str2
		;were not pushing len1,len2 becase at this point they're already on top of the stack
		lea     eax, [ebp - 40h]; load &(concatenated string) to eax
		push    eax; push &concatenated_str
		lea		eax, [ebp - 60h]; load &str2 to ecx
		push    eax; push &str2
		lea		eax, [ebp - 80h]; load &str1 to ecx
		push    eax; push &str1
		call    _ConcatStrs
		add		esp, 0Ch
		lea     eax, [ebp - 40h]; load &(concatenated string) to eax
		push	eax
		push    OFFSET strdisp
		call	printf
		add		esp, 08h
		pop     ebp
		ret
	}
}