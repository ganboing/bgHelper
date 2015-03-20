#pragma once

template<size_t ...>
struct seq_{};

template<size_t N, size_t ...S>
struct gens_ : gens_ < N - 1, N - 1, S... > { };

template<size_t ...S>
struct gens_ < 0, S... > {
	typedef seq_<S...> type;
};

struct call_atexit{
	typedef void(*F)();
	F f;
	call_atexit(F _f) : f(_f){}
	~call_atexit()
	{
		f();
	}
};

template<typename T, size_t N>
size_t _my_strncpy(T(&Dest)[N], const volatile T* Src, size_t Len = SIZE_MAX){
	if (!Src){
		Dest[0] = T(0);
		return 1;
	}
	size_t i;
	for (i = 0; Len == SIZE_MAX ? Src[i] : i < Len; ++i){
		if (i < N - 1){
			Dest[i] = Src[i];
		}
		else{
			break;
		}
	}
	Dest[i] = T(0);
	return ++i;
}

inline char my_toupper(char a){
	if (a >= 'a' && a <= 'z'){
		return a - 'a' + 'A';
	}
	return a;
}

inline size_t my_strlen(volatile const char* str){
	size_t i = 0;
	while (str[i]){
		++i;
	}
	return i;
}

inline int my_stricmp(const char* str1, const char* str2){
	char c1, c2;
	do{
		c1 = my_toupper(*str1++);
		c2 = my_toupper(*str2++);
		if (c1 != c2){
			return (c1 < c2) ? -1 : 1;
		}
	}while(c1);
	return 0;
}

template<typename T>
inline size_t get_upper_cnt(size_t s){
	return (s + sizeof(T) - 1) / sizeof(T);
}

union EncodeInt{
	char c[16];
	unsigned short s;
	unsigned int i;
	unsigned long l;
	unsigned long long ll;
	double d;
};