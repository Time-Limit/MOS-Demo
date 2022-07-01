.PHONY: install, 02_test

install:
	if [ ! -d "./googletest" ]; then \
		git clone https://github.com/google/googletest.git \
			&& cd googletest \
			&& mkdir build \
			&& cd build \
			&& cmake .. -DBUILD_GMOCK=OFF \
			&& make; \
	fi	

02_test:
	g++ -o 02_test 02_test.cc -I./ -I./googletest/googletest/include -L./googletest/build/lib -lgtest -lgtest_main -std=c++14 -g -O0 \
	&& ./02_test \
	&& rm ./02_test \
