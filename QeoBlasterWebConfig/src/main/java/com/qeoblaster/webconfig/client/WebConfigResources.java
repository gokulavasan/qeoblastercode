package com.qeoblaster.webconfig.client;

import com.google.gwt.resources.client.ClientBundle;
import com.google.gwt.resources.client.ImageResource;

/**
 * Created by christopher on 12/28/13.
 */
public interface WebConfigResources extends ClientBundle {
    @Source("logo.png")
    @ImageResource.ImageOptions(width = 400, height = 200)
    ImageResource logo();

    @Source("plus-24.png")
    ImageResource plus24();

    @Source("cross-24.png")
    ImageResource cross24();
}
