#ifndef CONTROL_BLOCK_H
#define CONTROL_BLOCK_H 1

#include <cstddef>
#include <iostream>

#ifdef DEBUG
#include "memtrace.h"
#endif

namespace HF2 {

/// Skaláris Pointer tárolására
template<class T>
class ControlBlock {
private:
	/// A ControlBlock-ot birtokló pointerek száma
	//*  A birtoklás opcionális és implementáció függő.
	mutable std::size_t referenceCount_;

	/// Na tippelj mire való ez a pointer. (Nem, nem prezentációkhoz mutogatni.)
	T* ptr_;

	/// törölt default konstruktor
	ControlBlock();
public:

	/// explicit konstruktor
	explicit ControlBlock(T* t) : referenceCount_{ 1U }, ptr_{ t } {}

	/// destruktor
	~ControlBlock() noexcept { delete ptr_; }


	/// ptr_ getter
	T* ptr() const { return this->ptr_; }

	/// ptr_ setter
	void setPtr(T* ptr) {
		if (ptr == ptr_) {
			throw "Alredy managed pointer";
		}
		delete ptr_;
		ptr_ = ptr;
	}

	/// referenceCount_ getter
	std::size_t referenceCount() const { return referenceCount_; }

	//* A következő két függvény befolyásolja
	//* ennek a blokknak a törlődését

	/// inkrementálja a referenceCount_ értékét
	void incrementRefCount() const { ++referenceCount_; }

	/// dekrementálja a referenceCount_ értékét
	//* Ha 0 a referenceCount_ törli a blokkot
	void decrementRefCount() const {
		// Elvileg, ha 0 akkor végez magával
		if (--referenceCount_ == 0U)
			delete this;
	}
};

/// Vektorális Pointer tárolására
/**
* A setPtr<T[]>(...) potenciálisan a blokkot használó objektumok
* offsetjének a túlindexelését jelentheti,
* exception-t meg inkább nem dobatok vele mert általános használat közben
* könnyen elkerülhető egy ilyen probléma,
* feliratkozós; callbackes; event emitteres; frissítés
* meg már sok lenne ide. Szóval bízok a felhasználóban.
* Mellesleg javallott a ++/-- operátorokat csak loopokon belül használni
* és a végén 0-ba állítani
*/
template<class T>
class ControlBlock <T[]> {
private:
	/// A ControlBlock-ot birtokló pointerek száma
	//* A birtoklás opcionális és implementáció függő.
	mutable std::size_t referenceCount_;

	/// Na tippelj mire való ez a pointer.
	//* Nem, nem prezentációkhoz való.
	T* ptr_;

	/// Tömb mérete
	std::size_t size_;

	/// törölt default konstruktor
	ControlBlock();

public:

	/// explicit konstruktor
	explicit ControlBlock(T* t, std::size_t size) : referenceCount_{ 1U }, ptr_{ t }, size_{ size } {}

	/// Destruktor
	~ControlBlock() noexcept { delete[] ptr_; }


	/// ptr_ getter
	T* ptr() const { return this->ptr_; }

	/// ptr_ setter
	//* Ki-indexeléshez vezethet, ha az új size
	//* kisebb mint a this->size_ és az offset_ kilóg
	void setPtr(T* ptr, std::size_t size) {
		if (ptr == ptr_)
			throw "Alredy managed pointer";
		delete[] ptr_;
		ptr_ = ptr;
		size_ = size;
	}

	/// referenceCount_ getter
	std::size_t referenceCount() const { return referenceCount_; }

	//* A következő két függvény befolyásolja
	//* ennek a blokknak a törlődését
	/// inkrementálja a referenceCount_ értékét
	void incrementRefCount() const { ++referenceCount_; }

	/// dekrementálja a referenceCount_ értékét
	//* Ha 0 a referenceCount_ törli a blokkot
	inline void decrementRefCount() const {
		// Elvileg, ha 0 akkor végez magával
		if (--referenceCount_ == 0U)
			delete this;
	}

	/// size_ getter
	std::size_t size() const { return this->size_; }
};

}
#endif // !CONTROL_BLOCK_H
