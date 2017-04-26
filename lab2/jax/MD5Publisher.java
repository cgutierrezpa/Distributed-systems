import javax.xml.ws.Endpoint;

//Endpoint publisher
public class MD5Publisher{

	public static void main(String[] args) {
	   Endpoint.publish("http://localhost:8080/MD5", new MD5Impl());
    }

}