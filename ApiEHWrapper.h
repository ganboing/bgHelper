#pragma once
#include <windows.h>
#include <Psapi.h>

template<typename Fty>
struct WinApiEHWrapper_status;

template<typename Ret, typename ...Args>
struct WinApiEHWrapper_status<Ret WINAPI(Args...)>
{
	typedef Ret WINAPI Fty(Args...);

	template<Ret S, Fty F>
	struct helper{
		static void WINAPI call(Args... args){
			Ret ret = F(args...);
			if (ret != S)
			{
#ifdef _CPPUNWIND
				throw ret;
#else
				DbgRaiseAssertionFailure();
#endif
			}
		}
	};
};

template<typename Fty>
struct WinApiEHWrapper_result;

template<typename Ret, typename ...Args>
struct WinApiEHWrapper_result<Ret WINAPI(Args...)>
{
	typedef Ret WINAPI Fty(Args...);

	template<Ret S, Fty F>
	struct helper{
		static Ret WINAPI call(Args... args){
			Ret ret = F(args...);
			if (ret == S)
			{
#ifdef _CPPUNWIND
				throw GetLastError();
#else
				DbgRaiseAssertionFailure();
#endif
			}
			return ret;
		}
	};
};

#define GEN_WINAPI_EH_STATUS(status, api) static const auto& EH_##api = WinApiEHWrapper_status<decltype(api)>::helper<status, api>::call
#define GEN_WINAPI_EH_RESULT(result, api) static const auto& EH_##api = WinApiEHWrapper_result<decltype(api)>::helper<result, api>::call