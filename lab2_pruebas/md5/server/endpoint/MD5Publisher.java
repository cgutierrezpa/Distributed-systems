package md5.server.endpoint;

import javax.xml.ws.Endpoint;
import md5.server.ws.MD5Impl;
import java.net.Inet4Address;

//Endpoint publisher
public class MD5Publisher{

	public static void main(String[] args) {
		String url = "http://";
		try{
			url += Inet4Address.getLocalHost().getHostAddress();
		}
		catch(Exception e){
			System.out.println("Exception when getting IP address.");
		}
		url += ":8080/ws/md5";
		System.out.println("Publishing MD5 service at endpoint: " + url);
	    Endpoint.publish(url, new MD5Impl());
    }

}