package com.qeoblaster.webconfig.client;

import com.google.gwt.editor.client.Editor;
import com.sencha.gxt.core.client.ValueProvider;
import com.sencha.gxt.data.shared.LabelProvider;
import com.sencha.gxt.data.shared.ModelKeyProvider;
import com.sencha.gxt.data.shared.PropertyAccess;

/**
* Created by chenc52 on 1/14/14.
*/
interface DeviceProperties extends PropertyAccess<Device> {
    @Editor.Path("deviceID")
    ModelKeyProvider<Device> key();

    @Editor.Path("deviceName")
    ValueProvider<Device, String> name();

    @Editor.Path("type")
    ValueProvider<Device, DeviceType> type();

    @Editor.Path("deviceName")
    LabelProvider<Device> names();
}
