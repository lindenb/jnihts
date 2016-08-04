package com.github.lindenb.jnihts.samtools;

import java.io.Closeable;

import htsjdk.samtools.SAMFileHeader;
import htsjdk.samtools.SAMRecord;

public interface NativeSamReader extends Closeable{
	public SAMFileHeader getFileHeader() ;
	public SAMRecord advance();
}
