package com.github.lindenb.jnihts.samtools;

import java.io.File;

public class NativeSamReaderFactory
	{
	
	public NativeSamReader open(String samFile)
		{
		if(!NativeHts.isEnabled()) return null;
		NativeSamReaderImpl reader = new NativeSamReaderImpl(samFile);
		return reader;
		}
	}
