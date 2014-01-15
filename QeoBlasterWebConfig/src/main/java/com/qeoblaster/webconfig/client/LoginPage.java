package com.qeoblaster.webconfig.client;

import com.google.gwt.core.client.GWT;
import com.google.gwt.uibinder.client.UiBinder;
import com.google.gwt.uibinder.client.UiField;
import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.gwt.user.client.ui.IsWidget;
import com.google.gwt.user.client.ui.Widget;
import com.sencha.gxt.widget.core.client.button.TextButton;
import com.sencha.gxt.widget.core.client.container.VerticalLayoutContainer;
import com.sencha.gxt.widget.core.client.event.SelectEvent;
import com.sencha.gxt.widget.core.client.form.PasswordField;
import com.sencha.gxt.widget.core.client.form.TextField;

import java.util.List;

/**
 * Created by christopher on 12/28/13.
 */
public class LoginPage implements IsWidget {

    Widget root;

    interface LoginPageUiBinder extends UiBinder<Widget, LoginPage> {
    }

    private static LoginPageUiBinder ourUiBinder = GWT.create(LoginPageUiBinder.class);

    @UiField
    TextField userName;
    @UiField
    VerticalLayoutContainer.VerticalLayoutData verticalLayoutData;
    @UiField
    PasswordField password;
    @UiField
    TextButton loginButton;

    public LoginPage() {
        root = ourUiBinder.createAndBindUi(this);
        loginButton.addSelectHandler(new SelectEvent.SelectHandler() {
            @Override
            public void onSelect(SelectEvent selectEvent) {
                WebConfigService.App.getInstance().getServerData(new AsyncCallback<ServerData>() {
                    @Override
                    public void onFailure(Throwable throwable) {

                    }

                    @Override
                    public void onSuccess(ServerData serverData) {
                        ServerData.data = serverData;
                        MainPage.get().getCenter().setWidget(new MainMenu());
                        MainPage.get().getCenter().forceLayout();
                    }
                });
            }
        });
    }

    @Override
    public Widget asWidget() {
        return root;
    }
}