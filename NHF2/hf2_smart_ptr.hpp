#ifndef SMART_PTR_H
#define SMART_PTR_H 1

#include <cstddef>
#include <utility>

#include "hf2_control_block.hpp"

namespace HF2 {

/// Skaláris okospointer alaposztály
template<class T>
class Abstract_SmartPointer {
protected:
	/// Pointer kontrollblokkra
	ControlBlock<T>* ctrl_;

	/// "új" pointer inicializálás
	explicit Abstract_SmartPointer(T* t): ctrl_{ new ControlBlock<T>{t} } {}

	/// létező pointerrel osztozkodás
	Abstract_SmartPointer(ControlBlock<T>* ctrl): ctrl_{ ctrl } {}
public:
	/// virtuális destruktor, hogy biztosan meghívódjon a leszármazotté
	virtual ~Abstract_SmartPointer() {}


	/// ctrl_->referenceCount() facade
	inline std::size_t referenceCount() const { return ctrl_->referenceCount(); }


	/// új ControlBlock inicializálása a példánynak
	virtual void newPointer(T* t) = 0;

	/// új pointer beállítása az összes pointernek ami ezt a ControlBlock -ot használja
	virtual void reassignPointer(T* t) = 0;


	/// arrow
	inline T* operator->() { return ctrl_->ptr(); }

	/// dereferálás
	inline T& operator*() { return *(ctrl_->ptr()); }


	/// t == ctrl_->t
	inline bool operator==(const T* t) const { return t == ctrl_->ptr(); }
	/// t != ctrl_->t
	inline bool operator!=(const T* t) const { return t != ctrl_->ptr(); }
	/// sp.ctrl_->t != ctrl_->t
	inline bool operator==(const Abstract_SmartPointer<T>& sp) const { return sp.ctrl_->ptr() == ctrl_->ptr(); }
	/// sp.ctrl_->t != ctrl_->t
	inline bool operator!=(const Abstract_SmartPointer<T>& sp) const { return sp.ctrl_->ptr() != ctrl_->ptr(); }
	/// sp.ctrl_->t + offset == ctrl_->t
	inline bool operator==(const Abstract_SmartPointer<T[]>& sp) const { return sp.ctrl_->ptr() + sp.offset() == ctrl_->ptr(); }
	/// sp.ctrl_->t + offset != ctrl_->t
	inline bool operator!=(const Abstract_SmartPointer<T[]>& sp) const { return sp.ctrl_->ptr() + sp.offset() != ctrl_->ptr(); }
};

/// Vektorális okospointer alaposztály
/**
* javallott a ++/-- operátorokat csak loopokon belül használni
* és a végén 0-ba állítani
*/
template<class T>
class Abstract_SmartPointer <T[]> { 
protected:
	/// Pointer kontrollblokkra
	ControlBlock<T[]>* ctrl_;


	/// Bármikor módosítható offset
	mutable std::size_t offset_;


	/// "új" pointer inicializálás
	Abstract_SmartPointer(T* t, std::size_t size) : ctrl_{ new ControlBlock<T[]>{t, size} }, offset_{ 0U } {}

	/// létező pointerrel osztozkodás
	Abstract_SmartPointer(ControlBlock<T[]>* ctrl): ctrl_{ ctrl }, offset_{ 0U } {}
public:
	/// virtuális destruktor, hogy biztosan meghívódjon a leszármazotté
	virtual ~Abstract_SmartPointer() {}


	/// ctrl_->referenceCount() facade
	inline std::size_t referenceCount() const { return ctrl_->referenceCount(); }


	/// ctrl_->size() facade
	inline std::size_t size() const { return ctrl_->size(); }


	/// offset_ getter
	inline std::size_t offset() const { return offset_; }

	/// offset_ setter
	inline void setOffset(std::size_t offset) const {
		if (offset >= ctrl_->size())
			throw "New offset is too big";
		offset_ = offset;
	}


	/// új ControlBlock<T[]> inicializálása a példánynak
	//* érdemes az offsetet 0-ra állítani
	virtual void newPointer(T* t, std::size_t size) = 0;

	/// új pointer beállítása az összes pointernek ami ezt a ControlBlock<T[]> -ot használja
	//* Ki-indexeléshez vezethet, ha az új size
	//* kisebb mint a this->size_ és az offset_ kilóg
	virtual void reassignPointer(T* t, std::size_t size) = 0;


	/// indexelés 0. elemtől | const *char exception-t dob túlindexelésnél
	inline T& operator[](const std::size_t idx) {
		if (idx < ctrl_->size())
			return ctrl_->ptr()[idx];
		throw "Index out of range";
	}

	/// Pont arra jó mint a másik csak ez még const is.
	inline const T& operator[](const std::size_t idx) const {
		if (idx < ctrl_->size())
			return ctrl_->ptr()[idx];
		throw "Index out of range";
	}


	/// ++offset_ | nincs post-increment | ha eléri a ctrl_->size() -t akkor 0-ra áll vissza és false-t dob
	bool operator++() const {
		if (++offset_ < ctrl_->size())
			return true;
		offset_ = 0U;
		return false;
	}

	/// --offset_ | nincs post-decrement | ha elérné a -1 -t akkor size() - 1-re áll vissza és false-t dob
	bool operator--() const {
		if (offset_ == 0) {
			offset_ = ctrl_->size() - 1;
			return false;
		}
		--offset_;
		return true;
	}


	/// jelenleg offsettel kiválaszott elem tagjának hozzáférése
	inline T* operator->() const { return ctrl_->ptr() + offset_; }

	/// jelenleg offsettel kiválaszott elem dereferálása
	inline T& operator*() const { return *(ctrl_->ptr() + offset_); }


	/// t != ctrl_->t + offset
	inline bool operator==(const T* t) const { return t == ctrl_->ptr() + offset_; }
	/// t != ctrl_->t + offset
	inline bool operator!=(const T* t) const { return t != ctrl_->ptr() + offset_; }
	/// sp.ctrl_->t == ctrl_->t + offset
	inline bool operator==(const Abstract_SmartPointer<T>& sp) const { return sp.ctrl_->ptr() == ctrl_->ptr(); }
	/// sp.ctrl_->t != ctrl_->t + offset
	inline bool operator!=(const Abstract_SmartPointer<T>& sp) const { return sp.ctrl_->ptr() != ctrl_->ptr(); }
	/// sp.ctrl_->t + sp.offset == ctrl_->t + offset
	inline bool operator==(const Abstract_SmartPointer<T[]>& sp) const { return sp.ctrl_->ptr() + sp.offset() == ctrl_->ptr() + offset_; }
	/// sp.ctrl_->t + sp.offset != ctrl_->t + offset
	inline bool operator!=(const Abstract_SmartPointer<T[]>& sp) const { return sp.ctrl_->ptr() + sp.offset() != ctrl_->ptr() + offset_; }
};

}

#endif // !SMART_PTR_Hl
