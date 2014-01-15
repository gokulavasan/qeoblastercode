package com.qeoblaster.webconfig.client;

import com.google.gwt.user.client.rpc.AsyncCallback;

import java.util.List;

/**
 * Created by christopher on 1/14/14.
 */
public interface WebConfigServiceAsync {
    /**
     * Utility/Convenience class.
     * Use WebConfigService.App.getInstance() to access static instance of WebConfigServiceAsync
     */

    /**
     * Utility/Convenience class.
     * Use WebConfigService.App.getInstance() to access static instance of WebConfigServiceAsync
     */
    void getServerData(AsyncCallback<ServerData> async);
}
