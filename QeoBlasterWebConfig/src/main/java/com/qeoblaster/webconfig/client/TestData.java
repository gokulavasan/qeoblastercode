package com.qeoblaster.webconfig.client;

import java.util.Arrays;
import java.util.Date;

/**
 * Created by chenc52 on 12/31/13.
 */
public class TestData {

    public static Device DEVICE1 = new Device("001", "Qeo Blaster Kinect", DeviceType.QEO_IR, null);
    public static Device DEVICE2 = new Device("002", "ZWave light bulb", DeviceType.ZWAVE, null);
    public static Device DEVICE3 = new Device("003", "Other Device", DeviceType.OTHER, null);
    public static Device DEVICE4 = new Device("004", "Standard Qeo Device", DeviceType.QEO, null);

    public static final Device[] DEVICES = new Device[]{
            DEVICE1, DEVICE2, DEVICE3, DEVICE4

    };

    public static final Signal[] ZWAVE_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(), "Z signal 1", "true", SignalType.INPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "Z signal 2", "false", SignalType.OUTPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "Z signal 3", "on", SignalType.STATE, DEVICE2)
    };


    public static final Signal[] OTHER_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(),"O signal 1", "true", SignalType.INPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "O signal 2", "false", SignalType.OUTPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "O signal 3", "on", SignalType.STATE, DEVICE2)
    };

    public static final Signal[] QEO_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(),"QEO signal 1", "true", SignalType.INPUT_MSG, DEVICE4),
            new Signal(new Date().getTime(), "QEO signal 2", "false", SignalType.OUTPUT_MSG, DEVICE4),
            new Signal(new Date().getTime(), "QEO signal 3", "on", SignalType.STATE, DEVICE4)
    };


    static{
        DEVICE2.setSupportedSignal(Arrays.asList(ZWAVE_SIGNALS));
        DEVICE3.setSupportedSignal(Arrays.asList(OTHER_SIGNALS));
        DEVICE4.setSupportedSignal(Arrays.asList(QEO_SIGNALS));
    }
}

