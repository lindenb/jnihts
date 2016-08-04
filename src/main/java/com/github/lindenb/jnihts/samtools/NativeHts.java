package com.github.lindenb.jnihts.samtools;

public class NativeHts {
private static final boolean load() {
	try {
		System.loadLibrary("jnihts");
		return true;
	} catch(UnsatisfiedLinkError err) {
		err.printStackTrace();
		return false;
	}catch(SecurityException err) {
		err.printStackTrace();
		return false;
	}
} 
private static final boolean enabled = NativeHts.load();
	
	
public static boolean isEnabled() {
	return enabled;
}
}
