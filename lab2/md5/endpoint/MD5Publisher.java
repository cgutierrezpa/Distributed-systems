package md5.endpoint;

import javax.xml.ws.Endpoint;
import md5.ws.MD5Impl;

//Endpoint publisher
public class MD5Publisher{

	public static void main(String[] args) {
		final String url = "http://localhost:8080/ws/md5";
		System.out.println("Publishing MD5 service at endpoint: " + url);
	    Endpoint.publish(url, new MD5Impl());
    }

}