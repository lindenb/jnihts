#include <htslib/hfile.h>
#include <htslib/hts.h>
#include <htslib/sam.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "jnisamtools.h"

#define VERIFY_NOT_NULL(a) if((a)==0) do {fprintf(stderr,"Throwing error from %s %d\n",__FILE__,__LINE__); throwIOException(env,"Method returned Null");} while(0)

static void throwIOException(JNIEnv *env,const char* msg)
	{
	jclass newExcCls = (*env)->FindClass(env,"java/io/IOException");
	(*env)->ThrowNew(env, newExcCls, msg);
}

typedef struct JSamReader_t
	{
	htsFormat fmt;
    hFILE *fp;
	samFile* in;
	bam_hdr_t* hdr;
	bam1_t *rec;
	}JSamReader,*JSamReaderPtr;

JNIEXPORT jlong JNICALL Java_com_github_lindenb_jnisamtools_NativeSamReaderImpl__1samopen
  (JNIEnv *env, jclass clazz, jstring path)
  	{
  	htsFormat fmt;
  	const char* str;
  	JSamReaderPtr ptr=(JSamReaderPtr)malloc(sizeof(JSamReader));
  	VERIFY_NOT_NULL(ptr);
  	
  	ptr->rec =  bam_init1();
  	str = ( const char *) (*env)->GetStringUTFChars(env, path, NULL);
	VERIFY_NOT_NULL(str);
	ptr->fp = hopen(str, "r");
	
	if (ptr->fp == NULL) {
        fprintf(stderr, "htsfile: can't open \"%s\": %s\n", str, strerror(errno));
        return (jlong)0L;
        }
	 if (hts_detect_format(ptr->fp, &fmt) < 0) {
            fprintf(stderr, "htsfile: detecting \"%s\" format failed: %s\n",str, strerror(errno));
             return (jlong)0L;
        }

	
	ptr->in = hts_hopen(ptr->fp, str, "r");
	ptr->hdr = sam_hdr_read(ptr->in);
	
  	(*env)->ReleaseStringUTFChars(env,path,str);
  	return (jlong)(intptr_t)ptr;
  	}
  
  
JNIEXPORT void JNICALL Java_com_github_lindenb_jnisamtools_NativeSamReaderImpl__1samclose
  (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return;
  	bam_hdr_destroy(ptr->hdr);
  	hts_close(ptr->in);
  	bam_destroy1(ptr->rec);
  	free(ptr);
  	}

JNIEXPORT jint JNICALL Java_com_github_lindenb_jnisamtools_NativeSamReaderImpl__1next
 (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return -1;
  	return sam_read1(ptr->in, ptr->hdr, ptr->rec);
  	}
	
