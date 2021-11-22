#include "hf2_shared_ptr.hpp"

namespace HF2 {
	// Scalar
	template<class T>
	SharedPointer<T>& SharedPointer<T>::operator=(SharedPointer<T>&& sp) {
		if (this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
		this->ctrl_ = sp.ctrl_;
		sp.ctrl_ = nullptr;
		return *this;
	}

	template<class T>
	SharedPointer<T>& SharedPointer<T>::operator=(const SharedPointer<T>& sp) {
		if (this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
		this->ctrl_ = sp.ctrl_;
		this->ctrl_->incrementRefCount();
		return *this;
	}

	// Vector

	template<class T>
	SharedPointer<T[]>::SharedPointer(SharedPointer<T[]>&& sp) noexcept :
		Abstract_SmartPointer<T[]>{ sp.ctrl_ } {
		this->offset_ = sp.offset_;
		sp.ctrl_ = nullptr;
		sp.offset_ = 0U;
	}

	template<class T>
	SharedPointer<T[]>& SharedPointer<T[]>::operator=(SharedPointer<T[]>&& sp) noexcept {
		if (this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
		this->ctrl_ = sp.ctrl_;
		sp.ctrl_ = nullptr;
		this->offset_ = sp.offset_;
		sp.offset_ = 0U;
		return *this;
	}

	template<class T>
	SharedPointer<T[]>& SharedPointer<T[]>::operator=(const SharedPointer<T[]>& sp) {
		if (this->ctrl_ != nullptr)
			this->ctrl_->decrementRefCount();
		this->ctrl_ = sp.ctrl_;
		this->ctrl_->incrementRefCount();
		this->offset_ = sp.offset_;
		return *this;
	}
}
