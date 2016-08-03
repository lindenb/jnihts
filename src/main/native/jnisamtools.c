#include <htslib/hts.h>
#include <htslib/sam.h>
#include <stdlib.h>
#include "jnisamtools.h"

typedef struct JSamReader_t
	{
	htsFile htsfile;
	bam1_t *rec;
	}JSamReader,*JSamReaderPtr;

JNIEXPORT jlong JNICALL Java_com_github_lindenb_jnisamtools_NativeSamReaderImpl__1samopen
  (JNIEnv *env, jclass clazz, jstring path)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)malloc(sizeof(JSamReader));
  	ptr->rec =  bam_init1();
  	return (jlong)ptr;
  	}
  
  
JNIEXPORT void JNICALL Java_com_github_lindenb_jnisamtools_NativeSamReaderImpl__1samclose
  (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)ptrid;
  	if(ptr==0) return;
  	bam_destroy1(ptr->rec);
  	free(ptr);
  	}
  
