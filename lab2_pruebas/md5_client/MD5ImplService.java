
package md5_client;

import java.net.MalformedURLException;
import java.net.URL;
import javax.xml.namespace.QName;
import javax.xml.ws.Service;
import javax.xml.ws.WebEndpoint;
import javax.xml.ws.WebServiceClient;
import javax.xml.ws.WebServiceException;
import javax.xml.ws.WebServiceFeature;


/**
 * This class was generated by the JAX-WS RI.
 * JAX-WS RI 2.2.4-b01
 * Generated source version: 2.2
 * 
 */
@WebServiceClient(name = "MD5ImplService", targetNamespace = "http://ws.md5/", wsdlLocation = "http://localhost:8080/ws/md5?wsdl")
public class MD5ImplService
    extends Service
{

    private final static URL MD5IMPLSERVICE_WSDL_LOCATION;
    private final static WebServiceException MD5IMPLSERVICE_EXCEPTION;
    private final static QName MD5IMPLSERVICE_QNAME = new QName("http://ws.md5/", "MD5ImplService");

    static {
        URL url = null;
        WebServiceException e = null;
        try {
            url = new URL("http://localhost:8080/ws/md5?wsdl");
        } catch (MalformedURLException ex) {
            e = new WebServiceException(ex);
        }
        MD5IMPLSERVICE_WSDL_LOCATION = url;
        MD5IMPLSERVICE_EXCEPTION = e;
    }

    public MD5ImplService() {
        super(__getWsdlLocation(), MD5IMPLSERVICE_QNAME);
    }

    public MD5ImplService(WebServiceFeature... features) {
        super(__getWsdlLocation(), MD5IMPLSERVICE_QNAME, features);
    }

    public MD5ImplService(URL wsdlLocation) {
        super(wsdlLocation, MD5IMPLSERVICE_QNAME);
    }

    public MD5ImplService(URL wsdlLocation, WebServiceFeature... features) {
        super(wsdlLocation, MD5IMPLSERVICE_QNAME, features);
    }

    public MD5ImplService(URL wsdlLocation, QName serviceName) {
        super(wsdlLocation, serviceName);
    }

    public MD5ImplService(URL wsdlLocation, QName serviceName, WebServiceFeature... features) {
        super(wsdlLocation, serviceName, features);
    }

    /**
     * 
     * @return
     *     returns MD5
     */
    @WebEndpoint(name = "MD5ImplPort")
    public MD5 getMD5ImplPort() {
        return super.getPort(new QName("http://ws.md5/", "MD5ImplPort"), MD5.class);
    }

    /**
     * 
     * @param features
     *     A list of {@link javax.xml.ws.WebServiceFeature} to configure on the proxy.  Supported features not in the <code>features</code> parameter will have their default values.
     * @return
     *     returns MD5
     */
    @WebEndpoint(name = "MD5ImplPort")
    public MD5 getMD5ImplPort(WebServiceFeature... features) {
        return super.getPort(new QName("http://ws.md5/", "MD5ImplPort"), MD5.class, features);
    }

    private static URL __getWsdlLocation() {
        if (MD5IMPLSERVICE_EXCEPTION!= null) {
            throw MD5IMPLSERVICE_EXCEPTION;
        }
        return MD5IMPLSERVICE_WSDL_LOCATION;
    }

}