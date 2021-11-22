#include <iostream>
#include <vector>
#include <cstddef>
#include <utility>
#include <thread>

#define MEMTRACE 1
#include "memtrace.h"
#include "gtest_lite.h"
#include "hf2.hpp"

using namespace std;
using namespace HF2;

template<class T>
bool passByValueBool(SharedPointer<T> sp, std::size_t refcnt) {
	return (refcnt + 1U) == sp.referenceCount();
}
template<class T>
bool passByReferenceBool(SharedPointer<T>& sp, std::size_t refcnt) {
	return refcnt == sp.referenceCount();
}

void passByValueVoid(SharedPointer<int> sp) {
	cout << sp.referenceCount() << endl;
}

int main(void) {
	GTINIT(std::cin);

	TEST(SCALAR, ASSIGNMENT_AND_INIT) {
		int* t = new int;
		*t = 5;
		{
			SharedPointer<int> sp1{ t };
			EXPECT_EQ(1U, sp1.referenceCount());
			SharedPointer<int> sp2{ sp1 };
			EXPECT_TRUE(passByValueBool(sp1, sp1.referenceCount()));
			EXPECT_TRUE(passByReferenceBool(sp1, sp1.referenceCount()));
			EXPECT_EQ(2U, sp1.referenceCount());
			EXPECT_EQ(2U, sp2.referenceCount());
			EXPECT_EQ(5, *sp2);
			SharedPointer<int> sp3 = sp1;
			SharedPointer<int> sp4 = std::move(sp3);
			EXPECT_EQ(3U, sp4.referenceCount());
		}
		EXPECT_NE(5, *t); // itt már törölve kéne lennie
	} ENDM

	TEST(SCALAR, OPERATORS) {
		vector<int>* vec = new vector<int>{ 10, 20, 30 };
		{
			SharedPointer<vector<int>> sp1(vec);
			EXPECT_THROW(sp1.newPointer(vec), const char*); // vec-nek nem kéne törlődnie, ha ugyanazt állítom be
			EXPECT_THROW(sp1.reassignPointer(vec), const char*);
			EXPECT_EQ(3U, (*sp1).size());
			EXPECT_EQ(3U, sp1->size());
		}
		EXPECT_NE(3U, vec->size());
	} ENDM

	/** Áldott legyen a JPorta, hogy 🤬
	TEST(MULTITHREAD, REF_COUNT) {
		int* t = new int{ 5 };
		{
			SharedPointer<int> sp{ t };
			// a pointerben tárolt adat nem thread safe
			std::thread t1(passByValueVoid, sp);
			std::thread t2(passByValueVoid, sp);
			std::thread t3(passByValueVoid, sp);
			EXPECT_EQ(4U, sp.referenceCount());
			t1.join();
			t2.join();
			t3.join();
			EXPECT_EQ(1U, sp.referenceCount());

			// ha nem szól a memtrace akkor minden rendben
		}
	} ENDM*/

	// ++ ; -- ; [] ; * ; -> ; == ; !=
	TEST(VECTOR, OPERATORS) {
		int* t1 = new int[3];
		int* t2 = new int[5];
		{
			SharedPointer<int[]> vsp1{ t1, 3 };

			vsp1[2] = 1;
			EXPECT_EQ(1, vsp1[2]);
			EXPECT_EQ(t1[2], vsp1[2]);

			EXPECT_TRUE(++vsp1);	// 0 + 1
			EXPECT_TRUE(++vsp1);	// 1 + 1

			EXPECT_EQ(1, *vsp1);

			EXPECT_FALSE(++vsp1);	// 2 + 1 = 3 == size ==> 3 --> 0
			EXPECT_TRUE(++vsp1);	// 0 + 1

			EXPECT_TRUE(--vsp1);	// 1 - 1
			EXPECT_FALSE(--vsp1);	// 0 - 1 = -1 < 0 ==> -1 --> size - 1
			EXPECT_TRUE(--vsp1);	// 2 - 1
			EXPECT_TRUE(--vsp1);	// 1 - 1
			EXPECT_FALSE(--vsp1);	// 0 - 1 = -1 < 0 ==> -1 --> size - 1

			EXPECT_NO_THROW(vsp1.reassignPointer(t2,5));
		}

		vector<int>* vec = new vector<int>[3];
		{
			SharedPointer<vector<int>[]> vsp1(vec, 3);
			vsp1->push_back(2);
			++vsp1;
			vsp1->push_back(3);
			++vsp1;
			vsp1->push_back(4);
			++vsp1;
			vsp1->push_back(5);
			EXPECT_EQ(3, vsp1[1][0]);
			EXPECT_EQ(5, vsp1[0][1]);

			vsp1.setOffset(0);
			EXPECT_TRUE(vsp1 == vec);
			vsp1.setOffset(1); // === ++vsp1
			EXPECT_TRUE(vsp1 == vec + 1);
			// a többi operátor is effektíve így működik
		}
	} ENDM
	return 0;
}
