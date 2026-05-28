#pragma once

#include <concepts>
#include <stdexcept>
#include <functional>

#include <c_api.h>

namespace utils
{
	template<typename Callable, typename... Args>
	auto callApiFunction(Callable&& callable, Args&&... args) requires (std::invocable<Callable, Args..., Exception*>);
}

namespace utils
{
	template<typename Callable, typename... Args>
	auto callApiFunction(Callable&& callable, Args&&... args) requires (std::invocable<Callable, Args..., Exception*>)
	{
		Exception exception = nullptr;

		using ReturnType = std::invoke_result_t<Callable, Args..., Exception*>;

		if constexpr (std::same_as<ReturnType, void>)
		{
			std::invoke(callable, std::forward<Args>(args)..., &exception);

			if (exception)
			{
				std::string message = getExceptionMessage(exception);

				deleteException(exception);

				throw std::runtime_error(message);
			}
		}
		else
		{
			auto result = std::invoke(callable, std::forward<Args>(args)..., &exception);

			if (exception)
			{
				std::string message = getExceptionMessage(exception);

				deleteException(exception);

				throw std::runtime_error(message);
			}

			return result;
		}		
	}
}
