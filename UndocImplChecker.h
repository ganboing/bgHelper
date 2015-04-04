#define CHECK_OFFSET_SAME(s1, s2, m)\
	(&(((s1*)nullptr)->m) == &(((s2*)nullptr)->m))

#define CHECK_UNDOC_STRUCT(s, m)\
	static_assert(CHECK_OFFSET_SAME(s, s##_IMPL, m), "check member " #m " of struct " #s);