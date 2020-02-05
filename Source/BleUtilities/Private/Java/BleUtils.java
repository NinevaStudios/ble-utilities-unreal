// Copyright (c) 2020 Nineva Studios

package com.ninevastudios.androidbleunrealapi;

import android.support.annotation.Keep;

import java.util.UUID;

@Keep
public class BleUtils {

	public static UUID intToUUID(int i) {
		final long MSB = 0x0000000000001000L;
		final long LSB = 0x800000805f9b34fbL;
		return new UUID(MSB | ((long) i << 32), LSB);
	}
}
