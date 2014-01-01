package com.qeoblaster.webconfig.client;

/**
 * Created by chenc52 on 12/31/13.
 */
public class TestData {
    public static final Device[] DEVICES = new Device[]{
            new Device("001", "Qeo Kinect", DeviceType.QEO),
            new Device("002", "ZWave light bulb", DeviceType.ZWAVE),
            new Device("003", "Other Device", DeviceType.OTHER)};
}
