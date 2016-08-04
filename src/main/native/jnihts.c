#include <htslib/hfile.h>
#include <htslib/hts.h>
#include <htslib/sam.h>
#include <htslib/kstring.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "jnihts.h"

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

JNIEXPORT jlong JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1samopen
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
  
  
JNIEXPORT void JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1samclose
  (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return;
  	bam_hdr_destroy(ptr->hdr);
  	hts_close(ptr->in);
  	bam_destroy1(ptr->rec);
  	free(ptr);
  	}

JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1next
 (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return -1;
  	return sam_read1(ptr->in, ptr->hdr, ptr->rec);
  	}
	
JNIEXPORT jbyteArray JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1headerAsText
   (JNIEnv *env, jclass clazz, jlong ptrid)
   	{
   	jbyteArray result;
   	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
 	
    result=(*env)->NewByteArray(env, ptr->hdr->l_text);
	(*env)->SetByteArrayRegion(env, result, 0, ptr->hdr->l_text, (jbyte*)ptr->hdr->text);
   	return result;
   	}
   
JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1flags
  (JNIEnv *env, jclass clazz, jlong ptrid)
  	{
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.flag);
  	}
  
JNIEXPORT jbyteArray JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1readName
   (JNIEnv *env, jclass clazz, jlong ptrid)
    {
    	jbyteArray result;
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	result=(*env)->NewByteArray(env, ptr->rec->core.l_qname);
	(*env)->SetByteArrayRegion(env, result, 0, ptr->rec->core.l_qname,(jbyte*) bam_get_qname(ptr->rec));
   	return result;
  	}
  	
 JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1referenceIndex
  (JNIEnv *env, jclass clazz, jlong ptrid)
  {
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.tid);
  }

 JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1alignStart
  (JNIEnv *env, jclass clazz, jlong ptrid)
  {
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.pos);
  }
 
 
 JNIEXPORT jbyteArray JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1sequence
   (JNIEnv *env, jclass clazz, jlong ptrid)
    {
    int i;
    uint8_t *s ;
    jbyteArray result;
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	s = bam_get_seq( ptr->rec);
  	result=(*env)->NewByteArray(env, ptr->rec->core.l_qseq);
  	for (i = 0; i < ptr->rec->core.l_qseq; ++i) {
  		 char c= "=ACMGRSVTWYHKDBN"[bam_seqi(s, i)];	
  		(*env)->SetByteArrayRegion(env, result, i, 1,(jbyte*)&c);
  		}
   	return result;
  	}
 
  JNIEXPORT jbyteArray JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1qualities
   (JNIEnv *env, jclass clazz, jlong ptrid)
    {
    int i;
    uint8_t *s ;
    jbyteArray result;
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	s = bam_get_qual( ptr->rec);
  	if (s[0] == 0xff) return 0;
  	result=(*env)->NewByteArray(env, ptr->rec->core.l_qseq);
  	for (i = 0; i < ptr->rec->core.l_qseq; ++i) {
  		char c = s[i] ;
  		(*env)->SetByteArrayRegion(env, result, i, 1,(jbyte*)&c);
  		}
   	return result;
  	}
  
    JNIEXPORT jbyteArray JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1cigar
   (JNIEnv *env, jclass clazz, jlong ptrid)
    {
    int i;
    uint8_t *s ;
    jbyteArray result;
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	if (ptr->rec->core.n_cigar) {
  		kstring_t str = { 0, 0, NULL };
  		uint32_t *cigar = bam_get_cigar(ptr->rec);
        for (i = 0; i < ptr->rec->core.n_cigar; ++i) {
            kputw(bam_cigar_oplen(cigar[i]), &str);
            kputc(bam_cigar_opchr(cigar[i]), &str);
        	}
	  	result=(*env)->NewByteArray(env, ks_len(&str));
		(*env)->SetByteArrayRegion(env, result, 0, ks_len(&str),(jbyte*)ks_str(&str));
		free(str.s);
	   	return result;
  		}
  	else
  		{
  		return 0;
  		}
  	}
 
 
  JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1matepos
  (JNIEnv *env, jclass clazz, jlong ptrid)
  {
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.mpos);
  }
 
   JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1isize
  (JNIEnv *env, jclass clazz, jlong ptrid)
  {
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.isize);
  }
  
   JNIEXPORT jint JNICALL Java_com_github_lindenb_jnihts_samtools_NativeSamReaderImpl__1mateReferenceIndex
  (JNIEnv *env, jclass clazz, jlong ptrid)
  {
  	JSamReaderPtr ptr=(JSamReaderPtr)(intptr_t)ptrid;
  	if(ptr==0) return 0;
  	return (jint)(ptr->rec->core.mtid);
  }