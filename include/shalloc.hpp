#pragma once
#include "ipcmalloc.h"
#include "murmurhash.hpp"
#include <cstddef>
#include <cstdlib>
#include <new>
#include <string>
#include <unordered_map>

namespace nano
{
template <typename T>
class object_traits
{
public:
	typedef T type;

	template <typename U>
	struct rebind
	{
		typedef object_traits<U> other;
	};

	// Constructor
	object_traits (void)
	{
	}

	// Copy Constructor
	template <typename U>
	object_traits (object_traits<U> const & other)
	{
	}

	// Address of object
	type * address (type & obj) const
	{
		return &obj;
	}
	type const * address (type const & obj) const
	{
		return &obj;
	}

	// Construct object
	void construct (type * ptr, type const & ref) const
	{
		// In-place copy construct
		new (ptr) type (ref);
	}

	// Destroy object
	void destroy (type * ptr) const
	{
		// Call destructor
		ptr->~type ();
	}
};

#define ALLOCATOR_TRAITS(T)                     \
	typedef T type;                             \
	typedef type value_type;                    \
	typedef value_type * pointer;               \
	typedef value_type const * const_pointer;   \
	typedef value_type & reference;             \
	typedef value_type const & const_reference; \
	typedef std::size_t size_type;              \
	typedef std::ptrdiff_t difference_type;

template <typename T>
struct max_allocations
{
	enum
	{
		value = static_cast<std::size_t> (-1) / sizeof (T)
	};
};

template <typename T>
class heap
{
public:
	ALLOCATOR_TRAITS (T)

	template <typename U>
	struct rebind
	{
		typedef heap<U> other;
	};

	// Default Constructor
	heap (void)
	{
	}

	// Copy Constructor
	template <typename U>
	heap (heap<U> const & other)
	{
	}

	// Allocate memory
	pointer allocate (size_type count, const_pointer /* hint */ = 0)
	{
		if (count > max_size ())
		{
			throw std::bad_alloc ();
		}
		auto allocated = dlmalloc (count * sizeof (type));
		if (allocated == 0)
			throw std::bad_alloc ();

		return static_cast<pointer> (allocated);
	}

	// Delete memory
	void deallocate (pointer ptr, size_type /* count */)
	{
		dlfree (ptr);
	}

	// Max number of objects that can be allocated in one call
	size_type max_size (void) const
	{
		return max_allocations<T>::value;
	}
};

#define FORWARD_ALLOCATOR_TRAITS(C)                      \
	typedef typename C::value_type value_type;           \
	typedef typename C::pointer pointer;                 \
	typedef typename C::const_pointer const_pointer;     \
	typedef typename C::reference reference;             \
	typedef typename C::const_reference const_reference; \
	typedef typename C::size_type size_type;             \
	typedef typename C::difference_type difference_type;

template <typename T,
    typename PolicyT = heap<T>,
    typename TraitsT = object_traits<T>>
class allocator : public PolicyT, public TraitsT
{
public:
	// Template parameters
	typedef PolicyT Policy;
	typedef TraitsT Traits;

	FORWARD_ALLOCATOR_TRAITS (Policy)

	template <typename U>
	struct rebind
	{
		typedef allocator<U,
		    typename Policy::template rebind<U>::other,
		    typename Traits::template rebind<U>::other>
		    other;
	};

	allocator (void)
	{
	}

	/** Copy constructor */
	template <typename U, typename PolicyU, typename TraitsU>
	allocator (allocator<U, PolicyU, TraitsU> const & other) :
	    Policy (other), Traits (other)
	{
	}
};

/** Check if two allocators are equal (always false) */
template <typename T, typename PolicyT, typename TraitsT,
    typename U, typename PolicyU, typename TraitsU>
bool operator== (allocator<T, PolicyT, TraitsT> const & left,
    allocator<U, PolicyU, TraitsU> const & right)
{
	return false;
}

/** Allocator inequality (always true) */
template <typename T, typename PolicyT, typename TraitsT,
    typename U, typename PolicyU, typename TraitsU>
bool operator!= (allocator<T, PolicyT, TraitsT> const & left, allocator<U, PolicyU, TraitsU> const & right)
{
	return !(left == right);
}

/** The allocator is never equal to a different type of allocator (always false) */
template <typename T, typename PolicyT, typename TraitsT,
    typename OtherAllocator>
bool operator== (allocator<T, PolicyT, TraitsT> const & left, OtherAllocator const & right)
{
	return false;
}

/** Inequality of allocator and a different type of allocator (always true) */
template <typename T, typename PolicyT, typename TraitsT,
    typename OtherAllocator>
bool operator!= (allocator<T, PolicyT, TraitsT> const & left,
    OtherAllocator const & right)
{
	return !(left == right);
}

/** Heap policy equality (always true) */
template <typename T, typename TraitsT, typename U, typename TraitsU>
bool operator== (allocator<T, heap<T>, TraitsT> const & left,
    allocator<U, heap<U>, TraitsU> const & right)
{
	return true;
}

/** Heap policy inequality (always false) */
template <typename T, typename TraitsT, typename U, typename TraitsU>
bool operator!= (allocator<T, heap<T>, TraitsT> const & left,
    allocator<U, heap<U>, TraitsU> const & right)
{
	return !(left == right);
}

/* Template aliases to reduce verbosity */
template <typename T>
using nalloc = allocator<T, heap<T>>;
template <typename K, typename V>
using umap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, std::allocator<std::pair<const K, V>>>;
using shm_string = std::basic_string<char, std::char_traits<char>, nalloc<char>>;

// TODO: new w/perfect forwards + delete, or template<typename T> ipcpp(...)

} // ns

namespace std
{
/** 
  * Some compilers/c++libs require std::hash specializations for stl container 
  * template arguments with allocators 
  */
template <>
struct hash<nano::shm_string>
{
	std::size_t operator() (nano::shm_string const & s) const
	{
		return (size_t)murmur_hash (s.c_str (), s.size (), 0);
	}
};
}
