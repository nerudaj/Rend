#pragma once

#include <concepts>
#include <stdexcept>
#include <memory>

namespace mem
{
	/**
	 *  \brief Single-threaded smart pointer with following properties:
	 *
	 *  * Non-nullable
	 *  * Non-movable
	 *  * Has to be initialized
	 *  * Owns memory
	 *  * Can be copied (reference counting)
	 *
	 *  Due to this, following errors can never occur:
	 *  * Use after free
	 *  * Use after move
	 *  * Null dereference
	 */
	template<class T>
	class [[nodiscard]] Rc final
	{
	public:
		Rc(std::nullptr_t) = delete;

		template<class ... Args>
			requires std::constructible_from<T, Args...>
		[[nodiscard]]
		constexpr Rc(Args&& ... args)
		{
			ptr = new T(std::forward<Args>(args)...);
			[[unlikely]]
			if (!ptr) throw std::bad_alloc();
			referenceCounter = new unsigned long(1);
		}

		[[nodiscard]]
		constexpr Rc(const Rc& other) noexcept
		{
			++(*other.referenceCounter);
			ptr = other.ptr;
			referenceCounter = other.referenceCounter;
		}

		constexpr Rc& operator=(Rc other) noexcept
		{
			swap(other);
			return *this;
		}

		template<class Derived>
			requires std::derived_from<Derived, T>
		[[nodiscard]]
		constexpr Rc(const Rc<Derived>& other) noexcept
		{
			++(*other.referenceCounter);
			ptr = dynamic_cast<T*>(other.ptr);
			referenceCounter = other.referenceCounter;
		}

		template<class Derived>
			requires std::derived_from<Derived, T>
		[[nodiscard]] Rc<Derived> castTo() const
		{
			return Rc<Derived>(dynamic_cast<Derived*>(ptr), referenceCounter);
		}

		constexpr ~Rc() noexcept
		{
			--(*referenceCounter);

			if (*referenceCounter > 0)
				return;

			std::default_delete<T>()(ptr);
			delete referenceCounter;
			ptr = nullptr;
			referenceCounter = nullptr;
		}

	private:
		// Helper constructor for downcasting
		[[nodiscard]] constexpr Rc(T* ptr, unsigned long* rc) noexcept : ptr(ptr), referenceCounter(rc)
		{
			++(*referenceCounter);
		}

	public:
		[[nodiscard]]
		constexpr T& operator*() noexcept
		{
			return *ptr;
		}

		[[nodiscard]]
		constexpr const T& operator*() const noexcept
		{
			return *ptr;
		}

		[[nodiscard]]
		constexpr T* operator->() noexcept
		{
			return ptr;
		}

		[[nodiscard]]
		constexpr T const* const operator->() const noexcept
		{
			return ptr;
		}

	protected:
		template<class>
		friend class Rc;

		constexpr void swap(Rc& other) noexcept
		{
			using std::swap;
			swap(ptr, other.ptr);
			swap(referenceCounter, other.referenceCounter);
		}

	protected:
		T* ptr = nullptr;
		unsigned long* referenceCounter = nullptr;
	};
}