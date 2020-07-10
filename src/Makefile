all: pcc_encoder

# C flags
CC=g++
CXXFLAGS=-std=c++11 -O3 -g

# boost library
BOOST_LIBS=-lboost_program_options \
		 			 -lboost_system

INCLUDE=-I/usr/local/include -I `pkg-config --cflags opencv` \
				-I/usr/include/boost \
				-I.
			 	# -I/usr/local/include/OpenEXR \

LIBS=-lstdc++fs `pkg-config opencv --cflags --libs` \
		 -L/usr/local/lib -Wl,-rpath=/usr/local/lib ${BOOST_LIBS} \
		 -L. -Wl,-rpath=.

# my PCC library
DEPS=utils.cpp encoder.cpp pcc_module.cpp
		 # euclidean.cpp compensation.cpp pcc_module.cpp multi_channel.cpp

OBJS=utils.o encoder.o pcc_module.o
		 # euclidean.o compensation.o pcc_module.o multi_channel.o


objs:
	${CC} -c -fPIC ${DEPS} ${CXXFLAGS} ${INCLUDE}

pcc_encoder: objs
	${CC} -o $@ pcc_encoder.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

pcc_multi_channel: objs
	${CC} -o $@ pcc_multi_channel.cpp ${OBJS} ${LIBS} ${CXXFLAGS} ${INCLUDE}

