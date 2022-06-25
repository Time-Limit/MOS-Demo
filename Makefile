.PHONY: install, 02-test

install:
	if [ ! -d "./googletest" ]; then \
		git clone https://github.com/google/googletest.git \
			&& cd googletest \
			&& mkdir build \
			&& cd build \
			&& cmake .. -DBUILD_GMOCK=OFF \
			&& make; \
	fi	

02-test:
	g++ -o 02-test 02-test.cc -I./ -I./googletest/googletest/include -L./googletest/build/lib -lgtest -lgtest_main -std=c++14 -g -O0 \
	&& ./02-test \
	&& rm ./02-test \
