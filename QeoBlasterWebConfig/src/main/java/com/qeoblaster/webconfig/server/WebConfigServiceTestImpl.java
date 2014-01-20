package com.qeoblaster.webconfig.server;

import com.google.gwt.user.server.rpc.RemoteServiceServlet;
import com.qeoblaster.webconfig.client.*;

import java.util.Arrays;
import java.util.Date;
import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
public class WebConfigServiceTestImpl extends RemoteServiceServlet implements WebConfigService {

    public static Device DEVICE1 = new Device(1, "Qeo Blaster Kinect", DeviceType.IR, null);
    public static Device DEVICE2 = new Device(2, "ZWave light bulb", DeviceType.ZWAVE, null);
    public static Device DEVICE3 = new Device(3, "ZigBee Switch", DeviceType.ZIGBEE, null);
    public static Device DEVICE4 = new Device(4, "Standard Qeo Device", DeviceType.QEO, null);

    public static final Device[] DEVICES = new Device[]{
            DEVICE1, DEVICE2, DEVICE3, DEVICE4

    };

    public static final Signal[] QEO_BLASTER_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(), "IR signal 1", "true", SignalType.INPUT_MSG, DEVICE1),
            new Signal(new Date().getTime(), "IR signal 2", "false", SignalType.OUTPUT_MSG, DEVICE1),
            new Signal(new Date().getTime(), "IR signal 3", "on", SignalType.STATE, DEVICE1)
    };

    public static final Signal[] ZWAVE_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(), "Z signal 1", "true", SignalType.INPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "Z signal 2", "false", SignalType.OUTPUT_MSG, DEVICE2),
            new Signal(new Date().getTime(), "Z signal 3", "on", SignalType.STATE, DEVICE2)
    };


    public static final Signal[] OTHER_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(), "O signal 1", "true", SignalType.INPUT_MSG, DEVICE3),
            new Signal(new Date().getTime(), "O signal 2", "false", SignalType.OUTPUT_MSG, DEVICE3),
            new Signal(new Date().getTime(), "O signal 3", "on", SignalType.STATE, DEVICE3)
    };

    public static final Signal[] QEO_SIGNALS = new Signal[]{
            new Signal(new Date().getTime(), "QEO signal 1", "true", SignalType.INPUT_MSG, DEVICE4),
            new Signal(new Date().getTime(), "QEO signal 2", "false", SignalType.OUTPUT_MSG, DEVICE4),
            new Signal(new Date().getTime(), "QEO signal 3", "on", SignalType.STATE, DEVICE4)
    };

    public static final Trigger[] TRIGGERS = new Trigger[]{
            new Trigger(new Date().getTime(), QEO_SIGNALS[0],ZWAVE_SIGNALS[1]),
            new Trigger(new Date().getTime(), ZWAVE_SIGNALS[2],OTHER_SIGNALS[2])
    };


    public static final Signal[] SIGNALS = new Signal[] {
            ZWAVE_SIGNALS[0],ZWAVE_SIGNALS[1],ZWAVE_SIGNALS[2],
            OTHER_SIGNALS[0],OTHER_SIGNALS[1],OTHER_SIGNALS[2],
            QEO_SIGNALS[0],QEO_SIGNALS[1],QEO_SIGNALS[2],
            QEO_BLASTER_SIGNALS[0],QEO_BLASTER_SIGNALS[1],QEO_BLASTER_SIGNALS[2]
    };

    static {
        DEVICE2.setSupportedSignal(Arrays.asList(ZWAVE_SIGNALS));
        DEVICE3.setSupportedSignal(Arrays.asList(OTHER_SIGNALS));
        DEVICE4.setSupportedSignal(Arrays.asList(QEO_SIGNALS));
    }

    @Override
    public ServerData getServerData() {
        ServerData ret = new ServerData();
        ret.signals = Arrays.asList(SIGNALS);
        ret.devices = Arrays.asList(DEVICES);
        ret.triggers = Arrays.asList(TRIGGERS);
        return ret;
    }

    @Override
    public boolean AddTrigger(Trigger trigger) {
        return false;
    }

    @Override
    public boolean RemoveTrigger(Trigger trigger) {
        return false;
    }
}