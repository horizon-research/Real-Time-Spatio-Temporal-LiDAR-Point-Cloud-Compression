all: pcc_encoder pcc_decoder pcc_stream_encoder pcc_stream_decoder pcc_test pcc_stream_test

# C flags
CC=g++
CXXFLAGS=-std=c++11 -O3 -g

# boost library
BOOST_LIBS=-lboost_program_options \
		 			 -lboost_system

INCLUDE=-I/usr/local/include -I `pkg-config --cflags opencv4` \
				-I/usr/include/boost \
				-I.

LIBS=-lstdc++fs `pkg-config opencv4 --cflags --libs` \
		 -L/usr/local/lib -Wl,-rpath=/usr/local/lib ${BOOST_LIBS} \
		 -L. -Wl,-rpath=.

# my PCC library
DEPS=utils.cpp encoder.cpp pcc_module.cpp decoder.cpp io.cpp

OBJS=utils.o encoder.o decoder.o pcc_module.o io.cpp
		 # euclidean.o compensation.o pcc_module.o multi_channel.o


objs:
	${CC} -c -fPIC ${DEPS} ${CXXFLAGS} ${INCLUDE}

pcc_encoder: objs
	${CC} -o $@ pcc_encoder.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_decoder: objs
	${CC} -o $@ pcc_decoder.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_stream_encoder: objs
	${CC} -o $@ pcc_stream_encoder.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_stream_decoder: objs
	${CC} -o $@ pcc_stream_decoder.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_test: objs
	${CC} -o $@ pcc_test.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_stream_test: objs
	${CC} -o $@ pcc_stream_test.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

