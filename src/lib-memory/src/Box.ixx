module;

#include <concepts>
#include <stdexcept>

export module Box;

export namespace mem
{
	/**
	 *  Memory safe version of std::unique_ptr
	 *  It cannot be null, it has to be initialized
	 *  and can be moved.
	 *
	 *  use-after-move static analysis should be
	 *  turned on for 100% safety.
	 */
	template<class T>
	class Box
	{
	public:
		template<class ... Args>
			requires std::constructible_from<T, Args...>
		[[nodiscard]]
		constexpr Box(Args&& ... args)
		{
			ptr = new T(std::forward<Args>(args)...);
			if (!ptr) [[unlikely]] throw std::bad_alloc();
		}

		Box(std::nullptr_t) = delete;
		Box(const Box&) = delete;

		[[nodiscard]]
		constexpr Box(Box&& other) noexcept
		{
			std::swap(ptr, other.ptr);
		}

		constexpr Box& operator=(Box&& other) noexcept
		{
			std::swap(ptr, other.ptr);
			return *this;
		}

		constexpr ~Box() noexcept
		{
			delete ptr;
			ptr = nullptr;
		}

	public: // construct from derived
		template<class D>
			requires std::derived_from<D, T>
		[[nodiscard]]
		constexpr Box(Box<D>&& other) noexcept
		{
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		template<class D>
			requires std::derived_from<D, T>
		constexpr Box& operator=(Box<D>&& other) noexcept
		{
			ptr = other.ptr;
			other.ptr = nullptr;
			return *this;
		}

	public:
		[[nodiscard]]
		constexpr T* operator->() noexcept
		{
			return ptr;
		}

		[[nodiscard]]
		constexpr T const* operator->() const noexcept
		{
			return ptr;
		}

		[[nodiscard]]
		constexpr T& operator*() noexcept
		{
			return *ptr;
		}

		[[nodiscard]]
		constexpr T const& operator*() const noexcept
		{
			return *ptr;
		}

	private:
		T* ptr = nullptr;

		// must be friend of itself so construction from derived works
		friend class Box;
	};
};