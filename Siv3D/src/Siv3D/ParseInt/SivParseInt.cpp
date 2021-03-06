//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (c) 2008-2019 Ryo Suzuki
//	Copyright (c) 2016-2019 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# include <Siv3D/Fwd.hpp>
# include <Siv3D/String.hpp>
# include <Siv3D/Optional.hpp>
# include <Siv3D/ParseInt.hpp>
# include <Siv3D/Error.hpp>
# include <abseil/numbers.hpp>

namespace s3d
{
	template <>
	int8 ParseInt<int8>(const StringView view, Arg::radix_<uint32> radix)
	{
		int32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<int8>(\"{}\") failed"_fmt(view));
		}
		else if (!InRange<int32>(result, INT8_MIN, INT8_MAX))
		{
			throw ParseError(U"Integer overflow occurred in ParseInt<int8>(\"{}\")"_fmt(view));
		}
		
		return static_cast<int8>(result);
	}

	template <>
	uint8 ParseInt<uint8>(StringView view, Arg::radix_<uint32> radix)
	{
		uint32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<uint8>(\"{}\") failed"_fmt(view));
		}
		else if (UINT8_MAX < result)
		{
			throw ParseError(U"Integer overflow occurred in ParseInt<uint8>(\"{}\")"_fmt(view));
		}

		return static_cast<uint8>(result);
	}

	template <>
	int16 ParseInt<int16>(StringView view, Arg::radix_<uint32> radix)
	{
		int32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<int16>(\"{}\") failed"_fmt(view));
		}
		else if (!InRange<int32>(result, INT16_MIN, INT16_MAX))
		{
			throw ParseError(U"Integer overflow occurred in ParseInt<int16>(\"{}\")"_fmt(view));
		}

		return static_cast<int16>(result);
	}

	template <>
	uint16 ParseInt<uint16>(StringView view, Arg::radix_<uint32> radix)
	{
		uint32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<uint16>(\"{}\") failed"_fmt(view));
		}
		else if (UINT16_MAX < result)
		{
			throw ParseError(U"Integer overflow occurred in ParseInt<uint16>(\"{}\")"_fmt(view));
		}

		return static_cast<uint16>(result);
	}

	template <>
	int32 ParseInt<int32>(StringView view, Arg::radix_<uint32> radix)
	{
		int32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<int32>(\"{}\") failed"_fmt(view));
		}

		return result;
	}

	template <>
	uint32 ParseInt<uint32>(StringView view, Arg::radix_<uint32> radix)
	{
		uint32 result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<uint32>(\"{}\") failed"_fmt(view));
		}

		return result;
	}

	template <>
	long ParseInt<long>(StringView view, Arg::radix_<uint32> radix)
	{
		long result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<long>(\"{}\") failed"_fmt(view));
		}

		return result;
	}

	template <>
	unsigned long ParseInt<unsigned long>(StringView view, Arg::radix_<uint32> radix)
	{
		unsigned long result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<unsigned long>(\"{}\") failed"_fmt(view));
		}

		return result;
	}

	template <>
	long long ParseInt<long long>(StringView view, Arg::radix_<uint32> radix)
	{
		long long result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<int64>(\"{}\") failed"_fmt(view));
		}

		return result;
	}

	template <>
	unsigned long long ParseInt<unsigned long long>(StringView view, Arg::radix_<uint32> radix)
	{
		unsigned long long result;

		if (!detail::safe_int_internal(view, &result, *radix))
		{
			throw ParseError(U"ParseInt<uint64>(\"{}\") failed"_fmt(view));
		}

		return result;
	}


	template <>
	Optional<int8> ParseIntOpt<int8>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<int8>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<uint8> ParseIntOpt<uint8>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<uint8>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<int16> ParseIntOpt<int16>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<int16>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<uint16> ParseIntOpt<uint16>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<uint16>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<int32> ParseIntOpt<int32>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<int32>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<uint32> ParseIntOpt<uint32>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<uint32>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<long> ParseIntOpt<long>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<long>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<unsigned long> ParseIntOpt<unsigned long>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<unsigned long>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<long long> ParseIntOpt<long long>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<long long>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}

	template <>
	Optional<unsigned long long> ParseIntOpt<unsigned long long>(StringView view, Arg::radix_<uint32> radix)
	{
		try
		{
			return ParseInt<unsigned long long>(view, radix);
		}
		catch (const ParseError&)
		{
			return none;
		}
	}
}
