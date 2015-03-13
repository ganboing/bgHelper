#define GLOBAL_INITER(TYPE, NAME) \
	extern TYPE& GlobalIniterImpl_##NAME();\
	TYPE& NAME = GlobalIniterImpl_##NAME();

#define GLOBAL_INITER_IMPL(TYPE, NAME, ...) \
	extern TYPE& NAME;\
	TYPE& GlobalIniterImpl_##NAME(){\
		static TYPE NAME{__VA_ARGS__};\
		return NAME;\
}