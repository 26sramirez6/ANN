/*
 * VectorPrint.h
 *
 *  Created on: Jun 14, 2019
 *      Author: 26sra
 */

#ifndef FINAL_VECTORPRINT_H_
#define FINAL_VECTORPRINT_H_

/*
 * hw5_2.cpp
 *
 *  Created on: May 11, 2019
 *      Author: 26sra
 */

#include<iostream>
#include<iterator>
#include<vector>
#include<algorithm>
#include<string>
#include<boost/function_output_iterator.hpp>
using std::cout;
using std::endl;
using std::ostream_iterator;
using std::string;
using std::copy;
using std::ostream;
using std::vector;
using boost::make_function_output_iterator;


template<class ElementType, typename CharT = char>
struct ostream_joiner
{
	ostream & stream_;
	const CharT * delim_;
	bool begin_;
	ostream_joiner(ostream& stream, const CharT * delim):
		stream_(stream), delim_(delim), begin_(true) {

	}

    void operator()(const ElementType& element) {
    	if (!this->begin_) {
			this->stream_ << this->delim_;
		}
    	this->begin_ = false;
        this->stream_ << element;
    }
};

template<typename T>
ostream& operator<< (ostream& stream, const vector<T>& v) {
	ostream_joiner<T> joiner(cout, ", ");
	auto out_it = make_function_output_iterator(joiner);
	copy ( v.begin(), v.end(), out_it  );
	return stream;
}


#endif /* FINAL_VECTORPRINT_H_ */
