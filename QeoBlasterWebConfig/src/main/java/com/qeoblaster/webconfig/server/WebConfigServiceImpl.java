package com.qeoblaster.webconfig.server;

import com.google.gwt.user.server.rpc.RemoteServiceServlet;
import com.qeoblaster.webconfig.client.ServerData;
import com.qeoblaster.webconfig.client.WebConfigService;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Created by gammagec on 1/15/14.
 */
public class WebConfigServiceImpl extends RemoteServiceServlet implements WebConfigService {

    @Override
    public ServerData getServerData() {
        String hostName = "127.0.0.1";
        int portNumber = 8113;
        try {
            Socket socket = new Socket(hostName, portNumber);
            System.out.println("Connected To Socket Server");
            PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            String line = in.readLine();
            System.out.println("read line: " + line);
            out.println("JavaReady");

            out.println("GET_NUM_DEVICES");
            int numDevices = Integer.valueOf(in.readLine());
            System.out.println("Client got " + numDevices + " number of devices");

        } catch (UnknownHostException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
