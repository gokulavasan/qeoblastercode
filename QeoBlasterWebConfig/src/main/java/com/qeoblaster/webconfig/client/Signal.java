package com.qeoblaster.webconfig.client;

/**
 * Created by chenc52 on 12/31/13.
 */
public class Signal {
    private long id;
    private String name;
    private String value;
    private SignalType type;
    private Device owner;

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

    public Device getOwner() {
        return owner;
    }

    public void setOwner(Device owner) {
        this.owner = owner;
    }


    public Signal(long id,String name, String value, SignalType type, Device owner) {
        this.id = id;
        this.name = name;
        this.value = value;
        this.type = type;
        this.owner = owner;
    }

    public Signal() {

    }

    public long getId() {
        return id;
    }

    public void setId(long id) {
        this.id = id;
    }
}
