SHELL=/bin/bash
CC ?= gcc
JAVA ?= java
JAVAC ?= javac
JAVAH ?= javah
CFLAGS=-O3 -Wall  -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
BWAJNIQUALPACKAGE=com.github.lindenb.jnisamtools
JAVASRCDIR=src/main/java
JAVACLASSNAME= NativeSamReader NativeSamReaderFactory NativeSamReaderImpl
JAVACLASSSRC=$(addprefix src/main/java/com/github/lindenb/jnisamtools/,$(addsuffix .java,$(JAVACLASSNAME)))
JAVAQUALNAME=$(addprefix ${BWAJNIQUALPACKAGE}.,$(JAVACLASSNAME))



ifeq (${JAVA_HOME},)
$(error $${JAVA_HOME} is not defined)
endif

ifeq (${HTSDIR},)
$(error $${HTSDIR} is not defined)
endif

## find path where to find include files
JDK_JNI_INCLUDES?=$(addprefix -I,$(sort $(dir $(shell find ${JAVA_HOME}/include -type f -name "*.h"))))


ifeq (${JDK_JNI_INCLUDES},)
$(error Cannot find C header files under $${JAVA_HOME})
endif


# my C source code path
native.dir=src/main/native


CC?=gcc
.PHONY:all compile 



#create a shared dynamic library for BWA
${native.dir}/libjnisamtools.so : ${native.dir}/jnisamtools.o
	$(CC) -shared -o $@ $<  -L ${native.dir} -L${HTSDIR} -lhts -lm -lz -lpthread

#compile the JNI bindings
${native.dir}/jnisamtools.o: ${native.dir}/jnisamtools.c ${native.dir}/jnisamtools.h
	$(CC) -c $(CFLAGS) -o $@ $(CFLAGS) -fPIC  ${JDK_JNI_INCLUDES}  -I${HTSDIR} $<



#create JNI header
${native.dir}/jnisamtools.h : compile
	$(JAVAH) -o $@ -jni -classpath ${JAVASRCDIR} $(JAVAQUALNAME)
	
#compile java classes
compile: $(JAVACLASSSRC)
	$(JAVAC) -sourcepath ${JAVASRCDIR} -d ${JAVASRCDIR} $^

