package com.qeoblaster.webconfig.client;

/**
 * Created by chenc52 on 12/31/13.
 */
public class Signal {
    private String name;
    private String value;
    private SignalType type;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public SignalType getType() {
        return type;
    }

    public void setType(SignalType type) {
        this.type = type;
    }
}
