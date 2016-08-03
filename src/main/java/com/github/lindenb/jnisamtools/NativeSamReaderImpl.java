package com.github.lindenb.jnisamtools;

import java.io.IOException;

class NativeSamReaderImpl implements NativeSamReader {
private long data = 0L;

@Override
public void close() throws IOException {
	if(this.data!=0L) {
		NativeSamReaderImpl._samclose(this.data);
		}
	this.data = 0L;
	}


public boolean next() {
	int i = _next(this.data);
	return i==0;
	}

private static native long _samopen(final String path);
private static native void _samclose(final long data);
private static native int _next(final long data);
}
