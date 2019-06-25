/*
 * Matrix.h
 *
 *  Created on: Jun 13, 2019
 *      Author: 26sra
 */

#ifndef FINAL_MATRIX_H_
#define FINAL_MATRIX_H_

#include <initializer_list>
#include <algorithm>
#include <array>
#include <memory>
#include <numeric>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <chrono>
#include <limits.h>

#undef minor
using std::initializer_list;
using std::array;
using std::unique_ptr;
using std::max_element;
using std::accumulate;
using std::ostream;
using std::endl;
using std::ostringstream;
using std::streamsize;
using std::setw;
using std::max;
using std::pair;
using std::vector;


template<typename T, int rows, int cols = rows>
class Matrix {
private:
	static size_t
	accumulateMax(size_t acc, T d) {
		ostringstream ostr;
		ostr << d;
		return std::max(acc, ostr.str().size());
	}

	static size_t
	accumulateMaxRow(size_t acc, array<T, cols> row) {
		return std::max(acc, accumulate(row.begin(), row.end(), static_cast<size_t>(0), accumulateMax));
	}

	size_t
	longestElementSize() const {
		return accumulate(data.begin(), data.end(), 0, accumulateMaxRow);
	}

	array<array<T, cols>, rows> data;

public:
	int m = rows;
	int n = cols;
	Matrix() : data{} {}
	Matrix(const Matrix<T, rows, cols>& other) : data{other} {}
	Matrix(array<array<T, cols>, rows>& other) : data{other} {}

	Matrix(initializer_list<initializer_list<T>> init) {
		auto dp = data.begin();
		for (auto row : init) {
			std::copy(row.begin(), row.end(), dp->begin());
			dp++;
		}
	}

	T &operator()(int x, int y) {
		return data[x<0 ? rows+x : x][y<0 ? cols+y : y];
	}

	T operator()(int x, int y) const {
		return data[x<0 ? rows+x : x][y<0 ? cols+y : y];
	}

	inline friend ostream &
	operator<<(ostream &os, const Matrix<T, rows, cols> &m) {
	size_t width = m.longestElementSize() + 2;
	os << "[ " << endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			os << setw(static_cast<streamsize>(width)) << m(i, j);
		}
		os << endl;
	}
	os << "]" << endl;
	return os;
	}

	inline int
	RowArgMax(int r) const {
		int maxIndex = 0;
		T maxVal = data[0][0];
		for (int i=0; i<cols; ++i) {
			if (this->data[r][i]) {
				if (maxVal < data[r][i]) {
					maxVal = data[r][i];
					maxIndex = i;
				}
			}
		}
		return maxIndex;
	}

	inline void
	RowAssignment(int r, T val) {
		for (int i=0; i<cols; ++i) {
			this->data[r][i] = val;
		}
	}

	template<int C>
	inline void
	RowAssignment(int r, const pair<int, int>& p,
			const Matrix<T , 1, C>& row) {
		for (int i=get<0>(p); i<get<1>(p); ++i) {
			this->data[r][i] = row(0, i);
		}
	}

	Matrix<T, 1, cols>&
	RowGet(int r) {
		return Matrix<T,1,cols>(this->data[r]);
	}

	inline void
	ColAssignment(int c, T val) {
		for (int i=0; i<rows; ++i) {
			this->data[i][c] = val;
		}
	}
};


// element-wise subtraction
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator-(Matrix<T, a, b> const &l, Matrix<T, a, b> const &r)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)-r(i,j);
		}
	}
	return result;
}

// broadcast subtraction
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator-(Matrix<T, a, b> const &l, T& val)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)-val;
		}
	}
	return result;
}

// element-wise addition
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator+(Matrix<T, a, b> const &l, Matrix<T, a, b> const &r)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)+r(i,j);
		}
	}
	return result;
}

// broadcast addition
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator+(Matrix<T, a, b> const &l, T& val)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)+val;
		}
	}
	return result;
}

// element-wise
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator*(Matrix<T, a, b> const &l, Matrix<T, a, b> const &r)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)*r(i,j);
		}
	}
	return result;
}

// broadcast
template<typename T, int a, int b>
inline Matrix<T, a, b>
operator*(Matrix<T, a, b> const &l, T& val)
{
	Matrix<T, a, b> result;
	for (int i = 0; i < a; i++) {
		for (int j = 0; j < b; j++) {
			result(i,j) = l(i,j)*val;
		}
	}
	return result;
}

#endif /* FINAL_MATRIX_H_ */
