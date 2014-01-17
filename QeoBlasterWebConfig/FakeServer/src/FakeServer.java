import java.io.*;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * Created by gammagec on 1/15/14.
 */
public class FakeServer {
    static public void main(String[] args) {
        System.out.println("FakeServer 2.0");

        ServerSocket serverSocket = null;
        Socket socket = null;
        BufferedReader in = null;
        PrintWriter out = null;
        for(;;) {
            try {
                serverSocket = new ServerSocket(8113);
                socket = serverSocket.accept();
                System.out.println("Got Connection");
                out = new PrintWriter(socket.getOutputStream(), true);
                in = new BufferedReader(
                        new InputStreamReader(socket.getInputStream()));
                out.println("SwitchAppReady");
                String resp = in.readLine();
                System.out.println("Got Response : " + resp);
                for(;;) {
                    String cmd = in.readLine();
                    System.out.println("Got cmd: " + cmd);
                    if(cmd.equals("GET_NUM_DEVICES")) {
                        out.println("4");
                    } else if(cmd.equals("GET_DEVICE_IDS")) {
                        out.println("0_1_2_3");
                    } else if(cmd.startsWith("GET_DEVICE_INFO_")) {
                        if(cmd.endsWith("0")) {
                            String r = "DVD Player IR_ES_HECDvD50:KEY&PLAY_ES_HECDvD50:KEY&PAUSE_";
                            out.println(r);
                        } else if(cmd.endsWith("1")) {
                            String r = "ZigBee Switch_EP_SWITCHON_EP_SWITCHOFF_";
                            out.println(r);
                        } else if(cmd.endsWith("2")) {
                            String r = "ZWave light Bulb_ES_TURNON_ES_TURNOFF_";
                            out.println(r);
                        } else if(cmd.endsWith("3")) {
                            String r = "Qeo Blaster Kinect_EP_LEFTCIRCLE_EP_RIGHTCIRCLE_";
                            out.println(r);
                        }
                    } else if(cmd.startsWith("SET_MAP_")) {

                    } else if(cmd.startsWith("SET_UNMAP_")) {

                    } else if(cmd.equals("GET_ALL_MAP")) {
                        StringBuilder r = new StringBuilder();
                        r.append("3_E_LEFTCIRCLE_0_E_HECDvD50:KEY&PLAY");
                        r.append(",");
                        r.append("1_E_SWITCHON_2_E_TURNON");
                        r.append(",");
                        r.append("1_E_SWITCHOFF_2_E_TURNOFF");
                        out.println(r.toString());
                    } else if(cmd.equals("END_CONVERSATION")) {
                        break;
                    }
                }
            } catch(UnknownHostException e) {
                e.printStackTrace();
            } catch(IOException e) {
                e.printStackTrace();
            } finally {
                if(in != null) {
                    try {
                        in.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if(out != null) {
                    out.close();
                }
                if(socket != null) {
                    try {
                        socket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                if(serverSocket != null) {
                    try {
                        serverSocket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
        }
    }

}
