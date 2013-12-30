package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.user.client.ui.*;
import com.sencha.gxt.widget.core.client.container.CenterLayoutContainer;

/**
 * Created by christopher on 12/28/13.
 */
public class WebConfig implements EntryPoint {
    public void onModuleLoad() {
        MainPage.get().getCenter().setWidget(new LoginPage());
        RootLayoutPanel.get().add(MainPage.get());
    }
}
