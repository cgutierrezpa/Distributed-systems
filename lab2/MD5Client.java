import md5.ws.MD5;
import md5.ws.MD5ImplService;

public class MD5Client{

	public static void main(String[] args) {

		MD5ImplService md5Service = new MD5ImplService();
		MD5 md5 = md5Service.getMD5ImplPort();
		try{
			System.out.println(md5.getMD5("mkyong"));
		}
		catch(Exception e){
			e.printStackTrace();
		}

    }

}