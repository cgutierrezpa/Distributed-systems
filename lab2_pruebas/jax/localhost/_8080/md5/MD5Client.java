import localhost._8080.md5.MD5Impl;
import localhost._8080.md5.MD5ImplService;

public class MD5Client{

	public static void main(String[] args) {

		MD5ImplService md5Service = new MD5ImplService();
		MD5Impl md5 = md5Service.getMD5ImplPort();

		System.out.println(md5.MD5("mkyong"));

    }

}