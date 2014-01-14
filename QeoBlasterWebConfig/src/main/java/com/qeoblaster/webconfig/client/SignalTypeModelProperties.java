package com.qeoblaster.webconfig.client;

import com.google.gwt.editor.client.Editor;
import com.sencha.gxt.data.shared.LabelProvider;
import com.sencha.gxt.data.shared.ModelKeyProvider;
import com.sencha.gxt.data.shared.PropertyAccess;

/**
* Created by chenc52 on 1/14/14.
*/
interface SignalTypeModelProperties extends PropertyAccess<SignalTypeModel> {
    @Editor.Path("key")
    ModelKeyProvider<SignalTypeModel> key();

    @Editor.Path("name")
    LabelProvider<SignalTypeModel> name();

}
