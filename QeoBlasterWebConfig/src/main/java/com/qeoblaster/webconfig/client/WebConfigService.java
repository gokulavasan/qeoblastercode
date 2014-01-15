package com.qeoblaster.webconfig.client;

import com.google.gwt.user.client.rpc.RemoteService;
import com.google.gwt.user.client.rpc.RemoteServiceRelativePath;
import com.google.gwt.core.client.GWT;

import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
@RemoteServiceRelativePath("WebConfigService")
public interface WebConfigService extends RemoteService {
    /**
     * Utility/Convenience class.
     * Use WebConfigService.App.getInstance() to access static instance of WebConfigServiceAsync
     */
    ServerData getServerData();

    public static class App {
        private static final WebConfigServiceAsync ourInstance = (WebConfigServiceAsync) GWT.create(WebConfigService.class);

        public static WebConfigServiceAsync getInstance() {
            return ourInstance;
        }
    }
}
