#pragma once

#include <windows.h>

#include "memory/basictypes.h"

namespace binding
{
	namespace subtle
	{
		typedef int32 Atomic32;
#if defined(_M_X64)
		// We need to be able to go between Atomic64 and AtomicWord implicitly.  This
		// means Atomic64 and AtomicWord should be the same type on 64-bit.
		typedef intptr_t Atomic64;
#endif

		// Use AtomicWord for a machine-sized pointer.  It will use the Atomic32 or
		// Atomic64 routines below, depending on your architecture.
		typedef intptr_t AtomicWord;

		inline Atomic32 NoBarrier_CompareAndSwap(volatile Atomic32* ptr,
			Atomic32 old_value,
			Atomic32 new_value) {
				LONG result = InterlockedCompareExchange(
					reinterpret_cast<volatile LONG*>(ptr),
					static_cast<LONG>(new_value),
					static_cast<LONG>(old_value));
				return static_cast<Atomic32>(result);
		}

		inline Atomic32 NoBarrier_AtomicExchange(volatile Atomic32* ptr,
			Atomic32 new_value) {
				LONG result = InterlockedExchange(
					reinterpret_cast<volatile LONG*>(ptr),
					static_cast<LONG>(new_value));
				return static_cast<Atomic32>(result);
		}

		inline Atomic32 Barrier_AtomicIncrement(volatile Atomic32* ptr,
			Atomic32 increment) {
				return InterlockedExchangeAdd(
					reinterpret_cast<volatile LONG*>(ptr),
					static_cast<LONG>(increment)) + increment;
		}

		inline Atomic32 NoBarrier_AtomicIncrement(volatile Atomic32* ptr,
			Atomic32 increment) {
				return Barrier_AtomicIncrement(ptr, increment);
		}

		inline void MyMemoryBarrier() {
#if defined(_M_X64)
			// See #undef and note at the top of this file.
			__faststorefence();
#else
			// We use MemoryBarrier from WinNT.h
			::MemoryBarrier();
#endif
		}

		inline Atomic32 Acquire_CompareAndSwap(volatile Atomic32* ptr,
			Atomic32 old_value,
			Atomic32 new_value) {
				return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
		}

		inline Atomic32 Release_CompareAndSwap(volatile Atomic32* ptr,
			Atomic32 old_value,
			Atomic32 new_value) {
				return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
		}

		inline void NoBarrier_Store(volatile Atomic32* ptr, Atomic32 value) {
			*ptr = value;
		}

		inline void Acquire_Store(volatile Atomic32* ptr, Atomic32 value) {
			NoBarrier_AtomicExchange(ptr, value);
			// acts as a barrier in this implementation
		}

		inline void Release_Store(volatile Atomic32* ptr, Atomic32 value) {
			*ptr = value; // works w/o barrier for current Intel chips as of June 2005
			// See comments in Atomic64 version of Release_Store() below.
		}

		inline Atomic32 NoBarrier_Load(volatile const Atomic32* ptr) {
			return *ptr;
		}

		inline Atomic32 Acquire_Load(volatile const Atomic32* ptr) {
			Atomic32 value = *ptr;
			return value;
		}

		inline Atomic32 Release_Load(volatile const Atomic32* ptr) {
			MyMemoryBarrier();
			return *ptr;
		}

#if defined(_WIN64)

		// 64-bit low-level operations on 64-bit platform.

		COMPILE_ASSERT(sizeof(Atomic64) == sizeof(PVOID), atomic_word_is_atomic);

		inline Atomic64 NoBarrier_CompareAndSwap(volatile Atomic64* ptr,
			Atomic64 old_value,
			Atomic64 new_value) {
				PVOID result = InterlockedCompareExchangePointer(
					reinterpret_cast<volatile PVOID*>(ptr),
					reinterpret_cast<PVOID>(new_value), reinterpret_cast<PVOID>(old_value));
				return reinterpret_cast<Atomic64>(result);
		}

		inline Atomic64 NoBarrier_AtomicExchange(volatile Atomic64* ptr,
			Atomic64 new_value) {
				PVOID result = InterlockedExchangePointer(
					reinterpret_cast<volatile PVOID*>(ptr),
					reinterpret_cast<PVOID>(new_value));
				return reinterpret_cast<Atomic64>(result);
		}

		inline Atomic64 Barrier_AtomicIncrement(volatile Atomic64* ptr,
			Atomic64 increment) {
				return InterlockedExchangeAdd64(
					reinterpret_cast<volatile LONGLONG*>(ptr),
					static_cast<LONGLONG>(increment)) + increment;
		}

		inline Atomic64 NoBarrier_AtomicIncrement(volatile Atomic64* ptr,
			Atomic64 increment) {
				return Barrier_AtomicIncrement(ptr, increment);
		}

		inline void NoBarrier_Store(volatile Atomic64* ptr, Atomic64 value) {
			*ptr = value;
		}

		inline void Acquire_Store(volatile Atomic64* ptr, Atomic64 value) {
			NoBarrier_AtomicExchange(ptr, value);
			// acts as a barrier in this implementation
		}

		inline void Release_Store(volatile Atomic64* ptr, Atomic64 value) {
			*ptr = value; // works w/o barrier for current Intel chips as of June 2005

			// When new chips come out, check:
			//  IA-32 Intel Architecture Software Developer's Manual, Volume 3:
			//  System Programming Guide, Chatper 7: Multiple-processor management,
			//  Section 7.2, Memory Ordering.
			// Last seen at:
			//   http://developer.intel.com/design/pentium4/manuals/index_new.htm
		}

		inline Atomic64 NoBarrier_Load(volatile const Atomic64* ptr) {
			return *ptr;
		}

		inline Atomic64 Acquire_Load(volatile const Atomic64* ptr) {
			Atomic64 value = *ptr;
			return value;
		}

		inline Atomic64 Release_Load(volatile const Atomic64* ptr) {
			MyMemoryBarrier();
			return *ptr;
		}

		inline Atomic64 Acquire_CompareAndSwap(volatile Atomic64* ptr,
			Atomic64 old_value,
			Atomic64 new_value) {
				return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
		}

		inline Atomic64 Release_CompareAndSwap(volatile Atomic64* ptr,
			Atomic64 old_value,
			Atomic64 new_value) {
				return NoBarrier_CompareAndSwap(ptr, old_value, new_value);
		}


#endif  // defined(_WIN64)
	}
}