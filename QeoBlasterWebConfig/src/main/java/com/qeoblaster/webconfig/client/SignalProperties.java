package com.qeoblaster.webconfig.client;

import com.google.gwt.editor.client.Editor;
import com.sencha.gxt.core.client.ValueProvider;
import com.sencha.gxt.data.shared.LabelProvider;
import com.sencha.gxt.data.shared.ModelKeyProvider;
import com.sencha.gxt.data.shared.PropertyAccess;

/**
* Created by chenc52 on 1/14/14.
*/
interface SignalProperties extends PropertyAccess<Signal> {
    @Editor.Path("id")
    ModelKeyProvider<Signal> id();


    @Editor.Path("value")
    ValueProvider<Signal, String> value();


    @Editor.Path("name")
    ValueProvider<Signal, String> names();

    @Editor.Path("type")
    ValueProvider<Signal, SignalType> signalType();

    @Editor.Path("owner")
    ValueProvider<Signal, Device> owner();

    @Editor.Path("name")
    LabelProvider<Signal> nameLabel();
}
