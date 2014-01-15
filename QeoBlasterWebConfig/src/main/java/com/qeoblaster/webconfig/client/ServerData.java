package com.qeoblaster.webconfig.client;

import com.google.gwt.user.client.rpc.IsSerializable;

import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
public class ServerData implements IsSerializable {
    public List<Signal> signals;
    public List<Device> devices;
    public List<Trigger> triggers;

    static public transient ServerData data;
}
