package server.endpoint;

import javax.xml.ws.Endpoint;
import server.ws.PrintStringImpl;
import java.net.Inet4Address;

public class PrintStringPublisher{
	public static void main(String[] args){
		String url = "http://";
		try{
			url += Inet4Address.getLocalHost().getHostAddress();
		}
		catch(Exception e){
			System.out.println("Exception when getting IP address.");
		}
		url += ":8080/ws/printString";
		System.out.println("Publishing PrintString service at endpoint: " + url);
		Endpoint.publish(url, new PrintStringImpl());
	}
}