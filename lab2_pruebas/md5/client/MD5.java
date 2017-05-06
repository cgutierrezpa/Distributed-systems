
package md5.client;

import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebResult;
import javax.jws.WebService;
import javax.jws.soap.SOAPBinding;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.ws.Action;
import javax.xml.ws.FaultAction;


/**
 * This class was generated by the JAX-WS RI.
 * JAX-WS RI 2.2.4-b01
 * Generated source version: 2.2
 * 
 */
@WebService(name = "MD5", targetNamespace = "http://ws.md5/")
@SOAPBinding(style = SOAPBinding.Style.RPC)
@XmlSeeAlso({
    ObjectFactory.class
})
public interface MD5 {


    /**
     * 
     * @param arg0
     * @return
     *     returns java.lang.String
     * @throws Exception_Exception
     */
    @WebMethod
    @WebResult(partName = "return")
    @Action(input = "http://ws.md5/MD5/getMD5Request", output = "http://ws.md5/MD5/getMD5Response", fault = {
        @FaultAction(className = Exception_Exception.class, value = "http://ws.md5/MD5/getMD5/Fault/Exception")
    })
    public String getMD5(
        @WebParam(name = "arg0", partName = "arg0")
        String arg0)
        throws Exception_Exception
    ;

}