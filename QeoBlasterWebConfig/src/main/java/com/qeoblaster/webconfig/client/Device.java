package com.qeoblaster.webconfig.client;

import com.google.gwt.regexp.shared.RegExp;
import com.google.gwt.regexp.shared.SplitResult;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


/**
 * Created by chenc52 on 12/31/13.
 */
public class Device implements Serializable {
    private long deviceID;
    private String deviceName;
    private DeviceType type;
    private List<Signal> supportedSignal;
    static private long lastSignalId = 0;

    public Device() {
    }

    private enum Mode {
        EP, SP, ES, SS
    };

    public Device(int id, String data) {
        deviceID = id;
        supportedSignal = new ArrayList<Signal>();
        RegExp reg = RegExp.compile("_");
        SplitResult res = reg.split(data);
        deviceName = res.get(0);

        RegExp qeoReg = RegExp.compile(".*Qeo.*");
        RegExp zwaveReg = RegExp.compile(".*ZWave.*");
        RegExp zigbeeReg = RegExp.compile(".*ZigBee.*");
        RegExp irReg = RegExp.compile(".*IR.*");
        if(qeoReg.test(deviceName)) {
            type = DeviceType.QEO;
        } else if(zwaveReg.test(deviceName)) {
            type = DeviceType.ZWAVE;
        } else if(zigbeeReg.test(deviceName)) {
            type = DeviceType.ZIGBEE;
        } else if(irReg.test(deviceName)) {
            type = DeviceType.IR;
        }
        int len = res.length();
        Mode mode = null;
        for(int i = 1; i < len; i++) {
            String val = res.get(i);
            if(val.length() == 0)
                continue;
            if(val.equals("EP")) {
                mode = Mode.EP;
                continue;
            } else if(val.equals("SP")) {
                mode = Mode.SP;
                continue;
            } else if(val.equals("ES")) {
                mode = Mode.ES;
                continue;
            } else if(val.equals("SS")) {
                mode = Mode.SS;
                continue;
            }
            switch(mode) {
                case EP:
                    parseEP(val);
                    break;
                case SP:
                    parseSP(val);
                    break;
                case ES:
                    parseES(val);
                    break;
                case SS:
                    parseSS(val);
                    break;
            }
        }
    }

    private void parseEP(String val) {
        RegExp reg = RegExp.compile(":");
        SplitResult publishedEvents = reg.split(val);
        System.out.println("EP: " + val);
        for(int i = 0; i < publishedEvents.length(); i++) {
            String signalName = publishedEvents.get(i);
            Signal signal = new Signal();
            signal.setOwner(this);
            signal.setType(SignalType.OUTPUT_MSG);
            signal.setName(signalName);
            signal.setId(lastSignalId++);
            supportedSignal.add(signal);
        }
    }

    private void parseSP(String val) {
        RegExp reg = RegExp.compile(":");
        SplitResult publishedEvents = reg.split(val);
        System.out.println("SP: " + val);
        for(int i = 0; i < publishedEvents.length(); i++) {
            String signalName = publishedEvents.get(i);
            Signal signal = new Signal();
            signal.setOwner(this);
            signal.setType(SignalType.STATE);
            signal.setName(signalName);
            signal.setId(lastSignalId++);
            supportedSignal.add(signal);
        }
    }

    private void parseES(String val) {
        RegExp reg = RegExp.compile(":");
        SplitResult publishedEvents = reg.split(val);
        System.out.println("ES: " + val);
        for(int i = 0; i < publishedEvents.length(); i++) {
            String signalName = publishedEvents.get(i);
            Signal signal = new Signal();
            signal.setOwner(this);
            signal.setType(SignalType.INPUT_MSG);
            signal.setName(signalName);
            signal.setId(lastSignalId++);
            supportedSignal.add(signal);
        }
    }
    private void parseSS(String val) {
        RegExp reg = RegExp.compile(":");
        SplitResult publishedEvents = reg.split(val);
        System.out.println("SS: " + val);
        for(int i = 0; i < publishedEvents.length(); i++) {
            String signalName = publishedEvents.get(i);
            Signal signal = new Signal();
            signal.setOwner(this);
            signal.setType(SignalType.STATE);
            signal.setName(signalName);
            signal.setId(lastSignalId++);
            supportedSignal.add(signal);
        }
    }

    public Device(long deviceID, String deviceName, DeviceType type, List<Signal> signals) {
        this.deviceID = deviceID;
        this.deviceName = deviceName;
        this.type = type;
        setSupportedSignal(signals);
    }

    @Override
    public String toString() {
        return deviceName + " (ID:" + deviceID + ")";
    }

    public long getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(long deviceID) {
        this.deviceID = deviceID;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    public DeviceType getType() {
        return type;
    }

    public void setType(DeviceType type) {
        this.type = type;
    }


    public List<Signal> getSupportedSignal() {
        return supportedSignal;
    }

    public void setSupportedSignal(List<Signal> supportedSignal) {
        this.supportedSignal = supportedSignal;
        if(supportedSignal != null) {
            for(Signal signal : supportedSignal) {
                signal.setOwner(this);
            }
        }
    }
}
