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
                    addEP(val);
                    break;
                case SP:
                    addSP(val);
                    break;
                case ES:
                    addES(val);
                    break;
                case SS:
                    addSS(val);
                    break;
            }
        }
    }

    private void addEP(String val) {
        String signalName = val;
        Signal signal = new Signal();
        signal.setOwner(this);
        signal.setType(SignalType.OUTPUT_MSG);
        RegExp replaceAmp = RegExp.compile("&");
        signal.setName(replaceAmp.replace(signalName, "_"));
        signal.setId(lastSignalId++);
        supportedSignal.add(signal);
    }

    private void addSP(String val) {
        String signalName = val;
        Signal signal = new Signal();
        signal.setOwner(this);
        signal.setType(SignalType.STATE);
        RegExp replaceAmp = RegExp.compile("&");
        signal.setName(replaceAmp.replace(signalName, "_"));
        signal.setId(lastSignalId++);
        supportedSignal.add(signal);
    }

    private void addES(String val) {
        String signalName = val;
        Signal signal = new Signal();
        signal.setOwner(this);
        signal.setType(SignalType.INPUT_MSG);
        RegExp replaceAmp = RegExp.compile("&");
        signal.setName(replaceAmp.replace(signalName, "_"));
        signal.setId(lastSignalId++);
        supportedSignal.add(signal);
    }
    private void addSS(String val) {
        String signalName = val;
        Signal signal = new Signal();
        signal.setOwner(this);
        signal.setType(SignalType.STATE);
        RegExp replaceAmp = RegExp.compile("&");
        signal.setName(replaceAmp.replace(signalName, "_"));
        signal.setId(lastSignalId++);
        supportedSignal.add(signal);
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
