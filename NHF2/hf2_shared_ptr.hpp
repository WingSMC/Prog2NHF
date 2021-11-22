#ifndef SHARED_PTR_H
#define SHARED_PTR_H 1

#include <cstddef>

#include "hf2_smart_ptr.hpp"

namespace HF2 {

/// Autómatikusan törlődő pointer
/**
 * Lehetőleg *ne* dinamikusan allokáljuk.
 * Mozgató konstruktorok és operátorok az std::move()-val használhatók
 */
template<class T>
class SharedPointer : public Abstract_SmartPointer<T> {
public:
	/// "új" pointer inicializálás
	explicit SharedPointer(T* t) : Abstract_SmartPointer<T> { t } { }

	/// mozgató konstruktor
	SharedPointer(SharedPointer<T>&& sp) noexcept 
		: Abstract_SmartPointer<T>{ sp.ctrl_ } {
		sp.ctrl_ = nullptr;
	}

	/// másoló konstruktor
	SharedPointer(const SharedPointer<T>& sp) 
		: Abstract_SmartPointer<T>{ sp.ctrl_ } {
		this->ctrl_->incrementRefCount();
	}

	/// destruktor
	~SharedPointer() {
		if(this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
	}


	//* virtuális függvények implementációja

	void newPointer(T* t) override {
		if (this->ctrl_ != nullptr) {
			if (t == this->ctrl_->ptr())
				throw "Alredy managed pointer";
			this->ctrl_->decrementRefCount();
		}
		this->ctrl_ = new ControlBlock<T>{ t };
	}

	void reassignPointer(T* t) override {
		if (this->ctrl_ == nullptr) {
			this->ctrl_ = new ControlBlock<T>{ t };
			return;
		}
		try {
			this->ctrl_->setPtr(t);
		} catch (const char*) {
			throw;
		}
	}


	/// mozgató értékadás
	SharedPointer<T>& operator=(SharedPointer<T>&& sp);

	/// másoló értékadás
	SharedPointer<T>& operator=(const SharedPointer<T>& sp);
};

/// Autómatikusan törlődő tömb pointer
/**
 * Lehetőleg *ne* dinamikusan allokáljuk.
 * Mozgató konstruktorok és operátorok az std::move()-val használhatók
 */
template<class T>
class SharedPointer <T[]> : public Abstract_SmartPointer<T[]> {
public:
	/// "új" pointer inicializálás
	explicit SharedPointer(T* t, std::size_t size): Abstract_SmartPointer<T[]>{ t, size } { }

	/// mozgató konstruktor
	SharedPointer(SharedPointer<T[]>&& sp) noexcept;

	/// másoló konstruktor
	SharedPointer(const SharedPointer<T[]>& sp): Abstract_SmartPointer<T[]>{ sp.ctrl_ } {
		this->offset_ = sp.offset_;
		this->ctrl_->incrementRefCount();
	}

	/// destruktor
	~SharedPointer() {
		if (this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
	}


	//* virtuális függvények implementációja

	void newPointer(T* t, std::size_t size) override {
		this->offset_ = 0U;
		if (this->ctrl_ != nullptr) {
			if (this->ctrl_->ptr() == t)
				throw "Alredy managed pointer";
			this->ctrl_->decrementRefCount();
		}
		this->ctrl_ = new ControlBlock<T[]>{ t, size };
	}
	//* Ki-indexeléshez vezethet, ha az új size
	//* kisebb mint a this->size_ és az offset_ kilóg a többi példányban
	void reassignPointer(T* t, std::size_t size) override {
		this->offset_ = 0U;
		if (this->ctrl_ == nullptr) {
			this->ctrl_ = new ControlBlock<T[]>{ t, size };
			return;
		}
		try {
			this->ctrl_->setPtr(t, size);
		} catch (const char*) {
			throw;
		}
	}


	/// mozgató értékadás
	SharedPointer<T[]>& operator=(SharedPointer<T[]>&& sp) noexcept;

	/// másoló értékadás
	SharedPointer<T[]>& operator=(const SharedPointer<T[]>& sp);
};

}

#endif
