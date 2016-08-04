package com.github.lindenb.jnihts.samtools;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import htsjdk.samtools.DefaultSAMRecordFactory;
import htsjdk.samtools.SAMFileHeader;
import htsjdk.samtools.SAMRecord;
import htsjdk.samtools.SAMRecordFactory;
import htsjdk.samtools.SAMTextHeaderCodec;
import htsjdk.samtools.SamReader;
import htsjdk.samtools.util.BufferedLineReader;

class NativeSamReaderImpl implements NativeSamReader {
private long data = 0L;
private SAMFileHeader samFileHeader=null;
private SAMRecordFactory samRecordFactory=new DefaultSAMRecordFactory();
NativeSamReaderImpl(String path) {
	this.data = _samopen(path);
	
}

public SAMFileHeader getFileHeader() {
	if(samFileHeader==null) {
		final SAMTextHeaderCodec headerCodec = new SAMTextHeaderCodec();
		BufferedLineReader r=new BufferedLineReader(new ByteArrayInputStream(_headerAsText(data)));
		this.samFileHeader = headerCodec.decode(r, null);
		r.close();
	}
	
	return samFileHeader;
	}


@Override
public void close() {
	if(this.data!=0L) {
		NativeSamReaderImpl._samclose(this.data);
		}
	this.data = 0L;
	}


@Override
public SAMRecord advance() {
	int i = _next(this.data);
	if(i<0) {
		return null;
	}
	final SAMRecord rec= samRecordFactory.createSAMRecord(getFileHeader());
	rec.setReadName(new String(_readName(this.data)));
	rec.setFlags(_flags(this.data));
	rec.setReferenceIndex(_referenceIndex(this.data));
	rec.setAlignmentStart(_alignStart(this.data));
	rec.setReadBases(_sequence(this.data));
	byte b[] = _qualities(this.data);
	if(b!=null) {
		rec.setBaseQualities(b);
	}
	b = _cigar(this.data);
	if(b!=null) {
		rec.setCigarString(new String(b));
	}
	
	rec.setMateReferenceIndex(_mateReferenceIndex(this.data));
	rec.setMateAlignmentStart(_matepos(this.data));
	rec.setInferredInsertSize(_isize(this.data));
	return rec;
	}

private static native long _samopen(final String path);
private static native void _samclose(final long data);
private static native int _next(final long data);
private static native byte[] _headerAsText(final long data);
private static native int _flags(final long data);
private static native byte[] _readName(final long data);
private static native int _referenceIndex(final long data);
private static native int _alignStart(final long data);
private static native byte[] _sequence(final long data);
private static native byte[] _qualities(final long data);
private static native byte[] _cigar(final long data);
private static native int _mateReferenceIndex(final long data);
private static native int _matepos(final long data);
private static native int _isize(final long data);


}
