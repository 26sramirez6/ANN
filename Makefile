CC=clang
CFLAGS=-std=c++1z -pedantic -Wall -Werror -Wextra -Xclang -flto-visibility-public-std
INCLUDE=-I"$(BOOST_ROOT)"

source = $(wildcard *.cpp)
obj = $(source:.cpp=.o)
exe = $(basename $(source))

all: clean $(exe)
	
$(obj): %.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	
$(exe): %: %.o
	$(CC) $(CFLAGS) -o $@ $<
	./main
	
clean:
	rm -f $(obj) $(exe)