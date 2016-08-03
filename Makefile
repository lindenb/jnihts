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
## http://stackoverflow.com/questions/9551416
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)



ifeq (${JAVA_HOME},)
$(error $${JAVA_HOME} is not defined)
endif

htslib.version?=0f2a88a03852085174aabee6e1f5bda76d66311f

## find path where to find include files
JDK_JNI_INCLUDES?=$(addprefix -I,$(sort $(dir $(shell find ${JAVA_HOME}/include -type f -name "*.h"))))


ifeq (${JDK_JNI_INCLUDES},)
$(error Cannot find C header files under $${JAVA_HOME})
endif

lib.dir=lib
htsjdk.jars  =  \
	$(lib.dir)/org/tukaani/xz/1.0/xz-1.0.jar \
	$(lib.dir)/commons-logging/commons-logging/1.1.1/commons-logging-1.1.1.jar \
	$(lib.dir)/org/apache/commons/commons-compress/1.4.1/commons-compress-1.4.1.jar \
	$(lib.dir)/gov/nih/nlm/ncbi/ngs-java/1.2.4/ngs-java-1.2.4.jar \
	$(lib.dir)/org/apache/commons/commons-jexl/2.1.1/commons-jexl-2.1.1.jar \
	$(lib.dir)/com/github/samtools/htsjdk/2.5.1/htsjdk-2.5.1.jar \
	$(lib.dir)/org/tukaani/xz/1.5/xz-1.5.jar \
	$(lib.dir)/org/xerial/snappy/snappy-java/1.0.3-rc3/snappy-java-1.0.3-rc3.jar

# my C source code path
native.dir=src/main/native


CC?=gcc
.PHONY:all compile 



#create a shared dynamic library for BWA
${native.dir}/libjnisamtools.so : ${native.dir}/jnisamtools.o htslib-${htslib.version}/libhts.so
	$(CC) -shared -o $@ $<  -L ${native.dir} -Lhtslib-${htslib.version} -lhts -lm -lz -lpthread

#compile the JNI bindings
${native.dir}/jnisamtools.o: ${native.dir}/jnisamtools.c ${native.dir}/jnisamtools.h
	$(CC) -c $(CFLAGS) -o $@ $(CFLAGS) -fPIC  ${JDK_JNI_INCLUDES}  -Ihtslib-${htslib.version} $<

#create JNI header
${native.dir}/jnisamtools.h : compile
	$(JAVAH) -o $@ -jni -classpath ${JAVASRCDIR} $(JAVAQUALNAME)
	
#compile java classes
compile: $(JAVACLASSSRC) ${htsjdk.jars}
	$(JAVAC) -cp $(subst ${SPACE},:,${htsjdk.jars}) -sourcepath  ${JAVASRCDIR} -d ${JAVASRCDIR}  $(JAVACLASSSRC)


${htsjdk.jars} : 
	mkdir -p $(dir $@) && wget -O "$@" "http://central.maven.org/maven2/$(patsubst ${lib.dir}/%,%,$@)"


htslib-${htslib.version}/libhts.so :
	rm -rf "${htslib.version}.zip" "htslib-${htslib.version}"
	wget -O "${htslib.version}.zip"  "https://github.com/samtools/htslib/archive/${htslib.version}.zip"
	unzip -o "${htslib.version}.zip" && (cd "htslib-${htslib.version}" && make ) && rm -f "${htslib.version}.zip"

clean:
	rm -rf ${htsjdk.jars}  htslib-${htslib.version}/libhts.so

