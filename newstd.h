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

template<size_t N>
size_t my_strncpy(char(&Dest)[N], const volatile char* &Src)
{
	if (!Src){
		Src = "null";
	}
	size_t i;
	for (i = 0; i < N - 1 && Src[i]; ++i)
	{
		Dest[i] = Src[i];
	}
	Src += i;
	Dest[i] = 0;
	return i + 1;
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