/*
 * ANN.h
 *
 *  Created on: Jun 13, 2019
 *      Author: 26sra
 */
#include <initializer_list>
#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <tuple>
#include <utility>
#include <chrono>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "Matrix.h"

using std::initializer_list;
using std::array;
using std::unique_ptr;
using std::max_element;
using std::accumulate;
using std::ostream;
using std::exp;
using std::cout;
using std::endl;
using std::ostringstream;
using std::tuple;
using std::streamsize;
using std::setw;
using std::pair;
using std::make_pair;
using std::max;
using std::vector;

template<int ...I>
struct IntSequence {};

template<>
struct IntSequence<> {
	static constexpr int sum = 0;
	static constexpr int max = 0;
	static constexpr int size = 0;
//	static constexpr initializer_list<int> list{};
};

template<int I>
struct IntSequence<I> {
	static constexpr int sum = I;
	static constexpr int max = I;
	static constexpr int size = 1;
//	static constexpr initializer_list<int> list{I};
};

template<int I, int... Is>
struct IntSequence<I, Is...> {
	static constexpr int sum = I + IntSequence<Is...>::sum;
	static constexpr int max = I > IntSequence<Is...>::max ?
			I : IntSequence<Is...>::max;
	static constexpr int size = 1 + IntSequence<Is...>::size;
//	static constexpr initializer_list<int> list{I, Is...};
};

template<int Index, class Sequence>
struct IntAtIndex {};

template<int I, int... Is>
struct IntAtIndex<0, IntSequence<I, Is...>> {
	static constexpr int value = I;
};

template<int Index, int I, int... Is>
struct IntAtIndex<Index, IntSequence<I, Is...>> :
	public IntAtIndex<Index-1, IntSequence<Is...>> {};

template<class First, class Second>
struct Append;

template<int... Ts, int... Us>
struct Append<IntSequence<Ts...>, IntSequence<Us...>> {
	using type = IntSequence<Ts..., Us...>;
};

template<class Sequence>
struct PopBack {};

template<int Last>
struct PopBack<IntSequence<Last>> {
   using type = IntSequence<>;
};

template<int H, int... Is>
struct PopBack<IntSequence<H, Is...>> {
	using type = typename Append<IntSequence<H>,
			typename PopBack<IntSequence<Is...> >::type >::type;
};

enum class Trim {
	Entry,
	Front,
	Back,
	Both,
	None
};

template<int Start, int End, class Sequence, Trim Complete=Trim::Entry>
struct Slice;

template<int Start, int End, int Head, int... Is>
struct Slice<Start, End, IntSequence<Head, Is...>, Trim::Entry >{
	using type = typename Slice<
			Start,
			End,
			IntSequence<Head, Is...>,
			Start==0 && End>=sizeof...(Is)+1 ? Trim::None :
			Start==0 						 ? Trim::Back :
			End>=sizeof...(Is)+1		     ? Trim::Front :
											   Trim::Both
	>::type;
};

template<int... Is>
struct Slice<0, sizeof...(Is), IntSequence<Is...>, Trim::None >{
	using type = IntSequence<Is...>;
};

template<int Start, int Head, int... Is>
struct Slice<Start, sizeof...(Is)+1, IntSequence<Head, Is...>, Trim::Front >{
	using type = typename Slice<
		Start-1,
		sizeof...(Is),
		IntSequence<Is...>,
		Start==1 ? Trim::None : Trim::Front
	>::type;
};

template<int End, int Head, int... Is>
struct Slice<0, End, IntSequence<Head, Is...>, Trim::Back >{
	using type = typename Slice<
		0,
		End,
		typename PopBack<IntSequence<Head, Is...>>::type,
		End>=sizeof...(Is) ? Trim::None : Trim::Back
	>::type;
};

template<int Start, int End, int Head, int... Is>
struct Slice<Start, End, IntSequence<Head, Is...>, Trim::Both >{
	static constexpr int size = sizeof...(Is);
	using type = typename Slice<
		Start-1,
		End-1,
		typename PopBack<IntSequence<Is...>>::type,
		Start==1 && End>=sizeof...(Is) 		? Trim::None :
		Start==1             	 		   	? Trim::Back :
		End>=sizeof...(Is) 			  		? Trim::Front :
											  Trim::Both
	>::type;
};


template<typename T, int cols>
T sigmoid(Matrix<T, 1, cols> &l, Matrix<T, 1, cols> &r) {
	T dot = 0;
	for (int i=0; i<cols; ++i) {
		dot += l(0,i)*r(0,i);
	}
	return 1/(1+exp(-dot));
}


template<typename T, int InputLayer, int... Layers>
class ANN {
public:
	using layers_ = IntSequence<InputLayer, Layers...>;
	using hiddenLayers_ = typename PopBack<IntSequence<Layers...>>::type;
	using nonInputLayers_ = typename Slice<1, layers_::size, layers_>::type;
	using nonOutputLayers_ = typename Slice<0, layers_::size-1, layers_>::type;
	static constexpr int nodeCount_ = layers_::sum - InputLayer;
	static constexpr int classCount_ = IntAtIndex<layers_::size-1, layers_>::value;
	static constexpr int layerCount_ = layers_::size;
	static constexpr int maxNodesLayer_ = layers_::max;
	Matrix<T, nodeCount_, maxNodesLayer_+1> weights_;
	vector<int> arch_; //{ {InputLayer, Layers...} };
	vector<int> nodeCum_; //{{0,}};

	ANN() : arch_{InputLayer, Layers...}, nodeCum_{0,} {
		srand(0);
		for (int i=1; i<ANN::layerCount_; ++i) {
			nodeCum_.push_back(nodeCum_[i-1] + arch_[i]);
		}

		Matrix< T, 1, maxNodesLayer_ > single;
		XavierInitializer();
		single.RowAssignment(0, 3.);
		ForwardPropagateSingle(single);
		cout << "here" << endl;
	}

private:

	inline double
	Random(double low, double high) {
		return low + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(high-low)));
	}

	void
	XavierInitializer() {
		int node = 0;
		for (int i=0; i<layerCount_-1; ++i) {
			double nin = arch_[i];
			double nout = i!=layerCount_-1 ? arch_[i+1] : 1;
			double root = sqrt( 6./(nin+nout) );
			for (int j=0; j<arch_[i]; ++j) {
				for (int k=0; k<nin + 1; ++k) {
					weights_(node, k) = Random(-root, root);
				}
			}
		}
	}

	Matrix< T, layerCount_, maxNodesLayer_ + 1 >
	ForwardPropagateSingle(const Matrix< T, 1, maxNodesLayer_ >& single) {
		Matrix< T, layerCount_, maxNodesLayer_ + 1 > ret;
		ret.ColAssignment(0, 1);
		pair<int, int> p(1, ret.m);;
		ret.RowAssignment(0, p, single);
		int node = 0;
		for (int l=0; l<layerCount_-1; ++l) {
			Matrix<T, 1, maxNodesLayer_+1> a = ret.RowGet(l);
			for (int n=0; n<arch_[l+1]; ++n) {
				ret(l+1, n+1) = sigmoid(a, weights_.RowGet(node));
				++node;
			}
		}
		return ret;
	}

};
